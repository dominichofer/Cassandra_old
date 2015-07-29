#include "move.h"

CMoveList::CMoveList(uint64_t P, uint64_t O, uint64_t& NodeCounter, uint64_t BitBoardPossible, int depth, const CHashTableValueType& ttValue, const bool PVS)
{
	//if (PVS) depth += 2;
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
	static const int PARITY_VALUE[16] = { 0, 20, 0, 10, 0, 10, 0, 10, 0, 5, 0, 1, 0, 1, 0, 1 };
	static const int sort_depth[61] = {
	//   0   1   2   3   4   5   6   7   8   9
		-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
		-1, -1,  0,  0,  0,  1,  1,  1,  2,  2,
		 2,  3,  3,  3,  4,  4,  4,  5,  5,  5,
		 6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
		 6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
		 6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
		 6
	};
	const uint64_t BBEmpties = ~(P | O);
	const uint64_t empties = Empties(P, O);

	while (BitBoardPossible)
	{
		CMove Move;
		Move.move = BitScanLSB(BitBoardPossible);
		RemoveLSB(BitBoardPossible);
		uint64_t flipped = flip(P, O, Move.move);
		Move.P = O ^ flipped;
		Move.O = P ^ flipped ^ (1ULL << Move.move);
		     if (flipped == O) Move.Value = 1 << 31;
		else if ((ttValue.depth > sort_depth[depth]) && (Move.move == ttValue.PV)) Move.Value = 1 << 30;
		else if ((ttValue.depth > sort_depth[depth]) && (Move.move == ttValue.AV)) Move.Value = 1 << 29;
		else
		{
			uint64_t PossMoves = PossibleMoves(Move.P, Move.O);
			Move.Value = FIELD_VALUE[Move.move];
			Move.Value += PARITY_VALUE[PopCount(BBEmpties & quadrant_mask[Move.move])];
			Move.Value -= PopCount(PossMoves) << 17;
			Move.Value -= PopCount(PossMoves & 0x8100000000000081UL) << 18;
			Move.Value += PopCount(StableStonesCornerAndCo(Move.O)) << 12;
			Move.Value -= PopCount(OpponentsExposed(Move.P, Move.O)) << 6;
			switch (sort_depth[depth])
			{
			case -1:
				break;
			case 0:
				Move.Value -= EvaluateFeatures(Move.P, Move.O) << 16;
				break;
			case 1:
			case 2:
				Move.Value -= Eval(Move.P, Move.O, NodeCounter, -64, 64, NO_SELECTIVITY, sort_depth[depth], empties-1) << 17;
				break;
			default:
				Move.Value -= Eval(Move.P, Move.O, NodeCounter, -64, 64, NO_SELECTIVITY, sort_depth[depth], empties-1) << 18;
				CHashTableValueType ttValue2;
				if (TT.LookUp(Move.P, Move.O, ttValue2)) Move.Value += 1 << 21;
				break;
			}
		}
		m_Moves.push_back(Move);
	}
	sort();
	return;
}
CMoveList::CMoveList(uint64_t P, uint64_t O, uint64_t BitBoardPossible, const bool PVS)
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
	static const int PARITY_VALUE[16] = { 0, 20, 0, 10, 0, 10, 0, 10, 0, 5, 0, 1, 0, 1, 0, 1 };
	const uint64_t BBEmpties = ~(P | O);

	while (BitBoardPossible)
	{
		CMove Move;
		Move.move = BitScanLSB(BitBoardPossible);
		RemoveLSB(BitBoardPossible);
		uint64_t flipped = flip(P, O, Move.move);
		Move.P = O ^ flipped;
		Move.O = P ^ flipped ^ (1ULL << Move.move);
		if (flipped == O) Move.Value = 1 << 31;
		else
		{
			uint64_t PossMoves = PossibleMoves(Move.P, Move.O);
			Move.Value = FIELD_VALUE[Move.move];
			Move.Value += PARITY_VALUE[PopCount(BBEmpties & quadrant_mask[Move.move])];
			Move.Value -= PopCount(PossMoves) << 17;
			Move.Value -= PopCount(PossMoves & 0x8100000000000081UL) << 18;
			Move.Value += PopCount(StableStonesCornerAndCo(Move.O)) << 12;
			Move.Value -= PopCount(OpponentsExposed(Move.P, Move.O)) << 6;
			if (PVS)
				Move.Value -= EvaluateFeatures(Move.P, Move.O) << 16;
		}
		m_Moves.push_back(Move);
	}
	sort();
	return;
}

CMoveListLight::CMoveListLight(const uint64_t P, const uint64_t O, uint64_t BitBoardPossible)
{
	static const uint64_t BB_A = 0x8100000000000081ULL;
	static const uint64_t BB_B = 0x4281000000008142ULL;
	static const uint64_t BB_C = 0x2400810000810024ULL;
	static const uint64_t BB_D = 0x1800008181000018ULL;
	static const uint64_t BB_E = 0x0042000000004200ULL;
	static const uint64_t BB_F = 0x0024420000422400ULL;
	static const uint64_t BB_G = 0x0018004242001800ULL;
	static const uint64_t BB_H = 0x0000240000240000ULL;
	static const uint64_t BB_I = 0x0000182424180000ULL;
	static const uint64_t BB_J = 0x0000001818000000ULL;
	static const uint64_t quadrant[16] = {
		0x0000000000000000ULL, //0000
		0x000000000F0F0F0FULL, //0001
		0x00000000F0F0F0F0ULL, //0010
		0x00000000FFFFFFFFULL, //0011
		0x0F0F0F0F00000000ULL, //0100
		0x0F0F0F0F0F0F0F0FULL, //0101
		0x0F0F0F0FF0F0F0F0ULL, //0110
		0x0F0F0F0FFFFFFFFFULL, //0111
		0xF0F0F0F000000000ULL, //1000
		0xF0F0F0F00F0F0F0FULL, //1001
		0xF0F0F0F0F0F0F0F0ULL, //1010
		0xF0F0F0F0FFFFFFFFULL, //1011
		0xFFFFFFFF00000000ULL, //1100
		0xFFFFFFFF0F0F0F0FULL, //1101
		0xFFFFFFFFF0F0F0F0ULL, //1110
		0xFFFFFFFFFFFFFFFFULL  //1111
	};
	static const int PARITY_VALUE[16] = { 0, 20, 0, 10, 0, 10, 0, 10, 0, 5, 0, 1, 0, 1, 0, 1 };
	const uint64_t BBEmpties = ~(P | O);
	const uint64_t BBParity = quadrant[Parity(BBEmpties)];
	uint64_t BBTmp;

	BBTmp = BitBoardPossible &  BBParity & BB_A;                        while (BBTmp) { m_Moves.push_back(BitScanLSB(BBTmp)); RemoveLSB(BBTmp); }
	BBTmp = BitBoardPossible &  BBParity & BB_C;                        while (BBTmp) { m_Moves.push_back(BitScanLSB(BBTmp)); RemoveLSB(BBTmp); }
	BBTmp = BitBoardPossible &  BBParity & BB_H;                        while (BBTmp) { m_Moves.push_back(BitScanLSB(BBTmp)); RemoveLSB(BBTmp); }
	BBTmp = BitBoardPossible &  BBParity & (BB_D | BB_I | BB_G);        while (BBTmp) { m_Moves.push_back(BitScanLSB(BBTmp)); RemoveLSB(BBTmp); }
	BBTmp = BitBoardPossible &  BBParity & (BB_F | BB_B | BB_E | BB_J); while (BBTmp) { m_Moves.push_back(BitScanLSB(BBTmp)); RemoveLSB(BBTmp); }
	BBTmp = BitBoardPossible & ~BBParity & BB_A;                        while (BBTmp) { m_Moves.push_back(BitScanLSB(BBTmp)); RemoveLSB(BBTmp); }
	BBTmp = BitBoardPossible & ~BBParity & BB_C;                        while (BBTmp) { m_Moves.push_back(BitScanLSB(BBTmp)); RemoveLSB(BBTmp); }
	BBTmp = BitBoardPossible & ~BBParity & BB_H;                        while (BBTmp) { m_Moves.push_back(BitScanLSB(BBTmp)); RemoveLSB(BBTmp); }
	BBTmp = BitBoardPossible & ~BBParity & (BB_D | BB_I | BB_G);        while (BBTmp) { m_Moves.push_back(BitScanLSB(BBTmp)); RemoveLSB(BBTmp); }
	BBTmp = BitBoardPossible & ~BBParity & (BB_F | BB_B | BB_E | BB_J); while (BBTmp) { m_Moves.push_back(BitScanLSB(BBTmp)); RemoveLSB(BBTmp); }

	assert(m_Moves.size() == PopCount(BitBoardPossible));
	return;
}
