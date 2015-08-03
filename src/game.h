#pragma once
#include <cassert>
#include <cstdint>
#include "macros_hell.h"
#include "hashtable.h"
#include "move.h"
#include "line.h"
#include "position.h"
#include "features.h"
#include "flip_fast.h"
#include "count_last_flip.h"
#include "selectivity.h"

int ZWS  (const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha,                 const int selectivity, const int depth, const int empties);
int PVS  (const uint64_t P, const uint64_t O, uint64_t& NodeCounter,       int alpha, const int beta, const int selectivity, const int depth, const int empties, CLine* pline = nullptr);
int Eval (const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int beta, const int selectivity, const int depth, const int empties, CLine* pline = nullptr);

// Use Transposition Table Value
inline bool UseTTValue(const CHashTableValueType& ttValue, const int alpha, const int beta, const int depth, const int selectivity, int & score)
{
	if ((ttValue.depth >= depth) && (ttValue.selectivity <= selectivity))
	{
		if (ttValue.alpha >= beta)         { score = ttValue.alpha; return true; }
		if (ttValue.beta <= alpha)         { score = ttValue.beta;  return true; }
		if (ttValue.alpha == ttValue.beta) { score = ttValue.alpha; return true; }
	}
	return false;
}

// Update Transposition Table
inline void UpdateTT(const uint64_t P, const uint64_t O, const uint64_t NodeCounter, const int alpha, const int beta, const int score, const int depth, const int selectivity, const uint8_t BestMove, const uint8_t PV, const uint8_t AV)
{ 
	if (score >= beta)
		TT.Update(P, O, CHashTableValueType(NodeCounter, depth, selectivity, score, +64, BestMove, BestMove == PV ? AV : PV));
	else if (score <= alpha)
		TT.Update(P, O, CHashTableValueType(NodeCounter, depth, selectivity, -64, score, BestMove, BestMove == PV ? AV : PV));
	else
		TT.Update(P, O, CHashTableValueType(NodeCounter, depth, selectivity, score, score, BestMove, BestMove == PV ? AV : PV));
}

// Update Transposition Table
inline void UpdateTTPV(const uint64_t P, const uint64_t O, const uint64_t NodeCounter, const int alpha, const int beta, const int score, const int depth, const int selectivity, const uint8_t BestMove, const uint8_t PV, const uint8_t AV)
{ 
	if (score >= beta)
		TTPV.Update(P, O, CHashTableValueType(NodeCounter, depth, selectivity, score, +64, BestMove, BestMove == PV ? AV : PV));
	else if (score <= alpha)
		TTPV.Update(P, O, CHashTableValueType(NodeCounter, depth, selectivity, -64, score, BestMove, BestMove == PV ? AV : PV));
	else
		TTPV.Update(P, O, CHashTableValueType(NodeCounter, depth, selectivity, score, score, BestMove, BestMove == PV ? AV : PV));
}

// Look Up Transposition Table
inline bool LookUpTT(const uint64_t P, const uint64_t O, CHashTableValueType& ttValue)
{ 
	return TT.LookUp(P, O, ttValue);
}

// Look Up Transposition Table
inline bool LookUpTTPV(const uint64_t P, const uint64_t O, CHashTableValueType& ttValue)
{ 
	return TTPV.LookUp(P, O, ttValue);
}

inline bool StabilityCutoff_ZWS(const uint64_t P, const uint64_t O, const int alpha, int& score)
{
	static const char stability_cutoff_limits[64] = {
		 99, 99, 99, 99,  6,  8, 10, 12,
		 14, 16, 20, 22, 24, 26, 28, 30,
		 32, 34, 36, 38, 40, 42, 44, 46,
		 48, 48, 50, 50, 52, 52, 54, 54,
		 56, 56, 58, 58, 60, 60, 62, 62,
		 64, 64, 64, 64, 64, 64, 64, 64,
		 99, 99, 99, 99, 99, 99, 99, 99,
	};
	if (alpha >= stability_cutoff_limits[Empties(P, O)]) //Worth checking stability
	{
		int value = 64 - 2 * PopCount(StableStones(P, O));
		if (value <= alpha)
		{
			score = value;
			return true;
		}
	}
	return false;
}

inline bool StabilityCutoff_PVS(const uint64_t P, const uint64_t O, const int alpha, int& score)
{
	static const char stability_cutoff_limits[64] = {
		 99, 99, 99, 99, -2,  0,  2,  4,
		  6,  8, 12, 14, 16, 18, 20, 22,
		 24, 26, 28, 30, 32, 34, 36, 38,
		 40, 40, 42, 42, 44, 44, 46, 46,
		 48, 48, 50, 50, 52, 52, 54, 54,
		 56, 56, 58, 58, 60, 60, 62, 62,
		 99, 99, 99, 99, 99, 99, 99, 99,
	};
	if (alpha >= stability_cutoff_limits[Empties(P, O)]) //Worth checking stability
	{
		int value = 64 - 2 * PopCount(StableStones(P, O));
		if (value <= alpha)
		{
			score = value;
			return true;
		}
	}
	return false;
}

inline int EvalGameOver(const uint64_t P, const int empties)
{
	int Diff = (PopCount(P) << 1) + empties - 64;
	     if (Diff > 0) return Diff + empties;
	else if (Diff < 0) return Diff - empties;
	else               return Diff;
}

template <unsigned int empties>
inline int EvalGameOver(const uint64_t P)
{
	if (empties % 2 == 0)
	{
		int Diff = (PopCount(P) << 1) + empties - 64;
		     if (Diff > 0) return Diff + empties;
		else if (Diff < 0) return Diff - empties;
		else               return Diff;
	}
	else
	{
		int Diff = (PopCount(P) << 1) + empties - 64;
		if (Diff > 0) return Diff + empties;
		else          return Diff - empties;
	}
}
template <> inline int EvalGameOver<0>(const uint64_t P) { return (PopCount(P) << 1) - 64; }

namespace Midgame
{
	int ZWS_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	int ZWS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	int ZWS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	int ZWS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	
	int PVS_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	int PVS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter,       int alpha, const int beta, CLine* pline = nullptr);
	int PVS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter,       int alpha, const int beta, CLine* pline = nullptr);
	int PVS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter,       int alpha, const int beta, CLine* pline = nullptr);
}

namespace Endgame
{		
	int ZWS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x);
	int ZWS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x1, const unsigned int x2);
	int ZWS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x1, const unsigned int x2, const unsigned int x3);
	int ZWS_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x1, const unsigned int x2, const unsigned int x3, const unsigned int x4);
	int ZWS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	int ZWS_2(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	int ZWS_3(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	int ZWS_4(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha);
	int ZWS_A(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const int empties);

	int PVS_0(const uint64_t P, const uint64_t O, uint64_t& NodeCounter);
	int PVS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, const unsigned int x, CLine* pline = nullptr);
	int PVS_A(const uint64_t P, const uint64_t O, uint64_t& NodeCounter,       int alpha, const int beta, const int empties, CLine* pline = nullptr);
	
	inline int PVS_1(const uint64_t P, const uint64_t O, uint64_t& NodeCounter, const int alpha, CLine* pline = nullptr) 
	{ return PVS_1(P, O, NodeCounter, alpha, BitScanLSB(~(P | O)), pline); }
}