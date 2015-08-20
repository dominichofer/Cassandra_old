#include "move.h"

CMoveList MoveList[61];

void CMoveList::Initialize(uint64_t P, uint64_t O, uint64_t& NodeCounter, uint64_t BitBoardPossible, int depth, int alpha, const CHashTableValueType& ttValue, const bool pvs)
{
	//if (!pvs) depth -= 2;

	const uint64_t BBEmpties = ~(P | O);
	const uint64_t empties = Empties(P, O);

	m_it = m_Moves;
	m_end = m_Moves + PopCount(BitBoardPossible);
	
	sort_alpha = MAX(-64, alpha - 8);
	sort_depth;
	int min_depth = 9;
    if (empties <= 27) min_depth += (30 - empties) / 3;
    if (depth >= min_depth)
	{
		sort_depth = (depth - 15) / 3;
		if (ttValue.beta < alpha) sort_depth -= 2; 
		sort_depth = BIND(sort_depth, 0, 6);
    }
	else
        sort_depth = -1;


	// PV and AV first
	if ((ttValue.depth > sort_depth) && (ttValue.PV != 64) && (BitBoardPossible & (1ULL << ttValue.PV))) // OPTIMIZATION: Remove  && (BitBoardPossible & (1ULL << ttValue.PV))
	{
		m_it->Value = 1 << 30;
		m_it->move = ttValue.PV;
		BitBoardPossible ^= (1ULL << ttValue.PV);
		uint64_t flipped = flip(P, O, m_it->move);
		m_it->P = O ^ flipped;
		m_it->O = P ^ flipped ^ (1ULL << m_it->move);
		++m_it;
	}
	if ((ttValue.depth > sort_depth) && (ttValue.AV != 64) && (BitBoardPossible & (1ULL << ttValue.AV))) // OPTIMIZATION: Remove  && (BitBoardPossible & (1ULL << ttValue.AV))
	{
		m_it->Value = 1 << 29;
		m_it->move = ttValue.AV;
		BitBoardPossible ^= (1ULL << ttValue.AV);
		uint64_t flipped = flip(P, O, m_it->move);
		m_it->P = O ^ flipped;
		m_it->O = P ^ flipped ^ (1ULL << m_it->move);
		++m_it;
	}

	m_P = P;
	m_O = O;
	m_BitBoardPossible = BitBoardPossible;
	m_NodeCounter = &NodeCounter;
	
	m_generated = m_it;
	m_it = m_Moves;

	if (m_generated == m_Moves) // None was generated
		GenerateRest();

	return;
}

void CMoveList::GenerateRest()
{
	static const int A = 9;
	static const int C = 8;
	static const int H = 7;
	static const int D = 6;
	static const int I = 5;
	static const int G = 4;
	static const int F = 3;
	static const int B = 2;
	static const int E = 1;
	static const int J = 0;
	static const int FIELD_VALUE[64] = {
		A, B, C, D, D, C, B, A,
		B, E, F, G, G, F, E, B,
		C, F, H, I, I, H, F, C,
		D, G, I, J, J, I, G, D,
		D, G, I, J, J, I, G, D,
		C, F, H, I, I, H, F, C,
		B, E, F, G, G, F, E, B,
		A, B, C, D, D, C, B, A,
	};
	static const int PARITY_VALUE[16] = { 0, 20, 0, 10, 1, 10, 2, 10, 3, 5, 3, 4, 3, 4, 3, 4 };
	uint64_t P = m_P;
	uint64_t O = m_O;
	uint64_t BitBoardPossible = m_BitBoardPossible;
	const uint64_t BBEmpties = ~(P | O);
	const uint64_t empties = Empties(P, O);

	// Fill MoveList
	while (BitBoardPossible)
	{
		m_generated->move = BitScanLSB(BitBoardPossible);
		RemoveLSB(BitBoardPossible);
		uint64_t flipped = flip(P, O, m_generated->move);
		m_generated->P = O ^ flipped;
		m_generated->O = P ^ flipped ^ (1ULL << m_generated->move);
		uint64_t PossMoves = PossibleMoves(m_generated->P, m_generated->O);

		m_generated->Value = FIELD_VALUE[m_generated->move];
		m_generated->Value += PARITY_VALUE[PopCount(BBEmpties & quadrant_mask[m_generated->move])];
		m_generated->Value -= PopCount(PossMoves) << 17;
		m_generated->Value -= PopCount(PossMoves & 0x8100000000000081UL) << 18;
		if (sort_depth < 0)
			m_generated->Value += PopCount(StableStonesCornerAndCo(m_generated->O)) << 12;
		else
			m_generated->Value += PopCount(StableEdges(m_generated->O, m_generated->P) & m_generated->O) << 12;
		m_generated->Value -= PopCount(OpponentsExposed(m_generated->P, m_generated->O)) << 6;
		switch (sort_depth)
		{
		case -1:
			break;
		case 0:
			m_generated->Value -= EvaluateFeatures(m_generated->P, m_generated->O) << 16;
			break;
		case 1:
		case 2:
			m_generated->Value -= PVS(m_generated->P, m_generated->O, *m_NodeCounter, -64, -sort_alpha, NO_SELECTIVITY, sort_depth, empties-1) << 17;
			break;
		default:
			m_generated->Value -= PVS(m_generated->P, m_generated->O, *m_NodeCounter, -64, -sort_alpha, NO_SELECTIVITY, sort_depth, empties-1) << 18;
			CHashTableValueType ttValue2;
			if (TT.LookUp(m_generated->P, m_generated->O, ttValue2)) m_generated->Value += 1 << 21;
			break;
		}
		++m_generated;
	}
	sort();
}

CMoveList::CMove* CMoveList::Next()
{ 
	++m_it;
	if (m_it == m_generated && m_generated != m_end) GenerateRest();
	return m_it;
}