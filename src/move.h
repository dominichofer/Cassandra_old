#pragma once
#include "position.h"
#include "hashtable.h"
#include "utility.h"
#include "features.h"
#include "game.h"
#include <algorithm>
#include <vector>

class CSearch;

class CMoveList
{
public:
	struct CMove
	{
		uint64_t P, O;
		unsigned long move;
		int Value;
	};

	CMoveList(uint64_t P, uint64_t O, uint64_t& NodeCounter, uint64_t BitBoardPossible, int depth, const CHashTableValueType & ttValue, const bool PVS);
	CMoveList(uint64_t P, uint64_t O, uint64_t BitBoardPossible, const bool PVS);

	inline std::vector<CMove>::iterator        begin()       { return m_Moves.begin(); }
	inline std::vector<CMove>::const_iterator cbegin() const { return m_Moves.cbegin(); }
	inline std::vector<CMove>::iterator          end()       { return m_Moves.end(); }
	inline std::vector<CMove>::const_iterator   cend() const { return m_Moves.cend(); }
	inline std::size_t size() const { return m_Moves.size(); }

	inline unsigned long BestMove() { return m_Moves[0].move; }
	inline unsigned long NextBestMove() { return size() > 1 ? m_Moves[1].move : 64; }

	inline void sort() { std::sort(m_Moves.begin(), m_Moves.end(), [](const CMove& Move1, const CMove& Move2){ return Move1.Value > Move2.Value; }); }

private:
	std::vector<CMove> m_Moves;
};

class CMoveListLight
{
public:
	CMoveListLight(const uint64_t P, const uint64_t O, uint64_t BitBoardPossible);

	inline std::vector<uint8_t>::iterator        begin()       { return m_Moves.begin(); }
	inline std::vector<uint8_t>::const_iterator cbegin() const { return m_Moves.cbegin(); }
	inline std::vector<uint8_t>::iterator          end()       { return m_Moves.end(); }
	inline std::vector<uint8_t>::const_iterator   cend() const { return m_Moves.cend(); }
	inline std::size_t size() const { return m_Moves.size(); }

	inline uint8_t BestMove() { return m_Moves[0]; }
	inline uint8_t NextBestMove() { return size() > 1 ? m_Moves[1] : 64; }
	
private:
	std::vector<uint8_t> m_Moves;
};