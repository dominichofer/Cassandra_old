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

	CMoveList() {}
	CMoveList(uint64_t P, uint64_t O, uint64_t& NodeCounter, uint64_t BitBoardPossible, int depth, int alpha, const CHashTableValueType & ttValue, const bool PVS);

	inline std::vector<CMove>::iterator        begin()       { return m_Moves.begin(); }
	inline std::vector<CMove>::const_iterator cbegin() const { return m_Moves.cbegin(); }
	inline std::vector<CMove>::iterator          end()       { return m_Moves.end(); }
	inline std::vector<CMove>::const_iterator   cend() const { return m_Moves.cend(); }
	inline std::size_t size() const { return m_Moves.size(); }

	inline unsigned long BestMove() { return size() ? m_Moves[0].move : 64; }
	inline unsigned long NextBestMove() { return size() > 1 ? m_Moves[1].move : 64; }

	inline void sort() { std::sort(m_Moves.begin(), m_Moves.end(), [](const CMove& Move1, const CMove& Move2){ return Move1.Value > Move2.Value; }); }

private:
	std::vector<CMove> m_Moves;
};