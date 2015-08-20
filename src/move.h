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
	
	CMove* m_end;
private:
	uint64_t m_P, m_O, m_BitBoardPossible;
	uint64_t *m_NodeCounter;
	int sort_alpha;
	int sort_depth;
	CMove* m_Moves;
	CMove* m_it;
	CMove* m_generated;

public:
	CMoveList() : m_Moves(new CMove[32]), m_end(m_Moves), m_it(m_Moves) {}
	~CMoveList() { delete[] m_Moves; }

	void Initialize(uint64_t P, uint64_t O, uint64_t& NodeCounter, uint64_t BitBoardPossible, int depth, int alpha, const CHashTableValueType & ttValue, const bool PVS);
	void GenerateRest();

	inline CMove* First() { return m_it = m_Moves; }
	CMove* Next();

	inline std::size_t size() const { return std::distance(m_Moves, m_end); }

	inline unsigned long BestMove() { return size() ? m_Moves[0].move : 64; }
	inline unsigned long NextBestMove() { return size() > 1 ? m_Moves[1].move : 64; }

	inline void sort() { std::sort(m_Moves, m_end, [](const CMove& Move1, const CMove& Move2){ return Move1.Value > Move2.Value; }); }
};

extern CMoveList MoveList[61];