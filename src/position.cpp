#include "position.h"

uint64_t FlipDiagonal(uint64_t b)
{
	// \ # # # # # # #
	// # \ # # # # # #
	// # # \ # # # # #
	// # # # \ # # # #
	// # # # # \ # # #
	// # # # # # \ # #
	// # # # # # # \ #
	// # # # # # # # \.
	uint64_t t;
	t  = (b ^ (b >>  7)) & 0x00AA00AA00AA00AAULL;
	b ^=  t ^ (t <<  7);
	t  = (b ^ (b >> 14)) & 0x0000CCCC0000CCCCULL;
	b ^=  t ^ (t << 14);
	t  = (b ^ (b >> 28)) & 0x00000000F0F0F0F0ULL;
	b ^=  t ^ (t << 28);
	return b;
}
uint64_t FlipCodiagonal(uint64_t b)
{
	// # # # # # # # /
	// # # # # # # / #
	// # # # # # / # #
	// # # # # / # # #
	// # # # / # # # #
	// # # / # # # # #
	// # / # # # # # #
	// / # # # # # # #
	uint64_t t;
	t  =  b ^ (b << 36);
	b ^= (t ^ (b >> 36)) & 0xF0F0F0F00F0F0F0FULL;
	t  = (b ^ (b << 18)) & 0xCCCC0000CCCC0000ULL;
	b ^=  t ^ (t >> 18);
	t  = (b ^ (b <<  9)) & 0xAA00AA00AA00AA00ULL;
	b ^=  t ^ (t >>  9);
	return b;
}
uint64_t FlipVertical(uint64_t b)
{
	// # # # # # # # #
	// # # # # # # # #
	// # # # # # # # #
	// # # # # # # # #
	// ---------------
	// # # # # # # # #
	// # # # # # # # #
	// # # # # # # # #
	// # # # # # # # #
	return BSwap(b);
	//b = ((b >>  8) & 0x00FF00FF00FF00FFULL) | ((b <<  8) & 0xFF00FF00FF00FF00ULL);
	//b = ((b >> 16) & 0x0000FFFF0000FFFFULL) | ((b << 16) & 0xFFFF0000FFFF0000ULL);
	//b = ((b >> 32) & 0x00000000FFFFFFFFULL) | ((b << 32) & 0xFFFFFFFF00000000ULL);
	//return b;
}
uint64_t FlipHorizontal(uint64_t b)
{
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	// # # # #|# # # #
	b = ((b >> 1) & 0x5555555555555555ULL) | ((b << 1) & 0xAAAAAAAAAAAAAAAAULL);
	b = ((b >> 2) & 0x3333333333333333ULL) | ((b << 2) & 0xCCCCCCCCCCCCCCCCULL);
	b = ((b >> 4) & 0x0F0F0F0F0F0F0F0FULL) | ((b << 4) & 0xF0F0F0F0F0F0F0F0ULL);
	return b;
}

std::string board1D(const uint64_t P, const uint64_t O)
{
	std::string s;
	for (unsigned int i = 0; i < 8; i++)
		for (unsigned int j = 0; j < 8; j++)
		{
			bool b_P = P & (0x8000000000000000ULL >> (i * 8 + j));
			bool b_O = O & (0x8000000000000000ULL >> (i * 8 + j));
			     if (b_P && b_O) s.append("#");
			else if (b_P)        s.append("X");
			else if (b_O)        s.append("O");
			else                 s.append("-");
		}
	return s;
}

std::string board2D(const uint64_t P, const uint64_t O)
{
	std::string s = "  H G F E D C B A  \n";
	for (unsigned int i = 0; i < 8; i++)
	{
		s.append(std::to_string(8-i));
		for (unsigned int j = 0; j < 8; j++)
		{
			bool b_P = P & (0x8000000000000000ULL >> (i * 8 + j));
			bool b_O = O & (0x8000000000000000ULL >> (i * 8 + j));
			     if (b_P && b_O) s.append(" #");
			else if (b_P)        s.append(" X");
			else if (b_O)        s.append(" O");
			else                 s.append(" -");
		}
		s.append(" " + std::to_string(8-i) + "\n");
	}
	return s.append("  H G F E D C B A  ");
}

std::string board2D(const uint64_t P, const uint64_t O, const uint64_t possibleMoves)
{
	std::string s = "  H G F E D C B A  \n";
	for (unsigned int i = 0; i < 8; i++)
	{
		s.append(std::to_string(8-i));
		for (unsigned int j = 0; j < 8; j++)
		{
			bool b_P = P & (0x8000000000000000ULL >> (i * 8 + j));
			bool b_O = O & (0x8000000000000000ULL >> (i * 8 + j));
			bool b_M = possibleMoves & (0x8000000000000000ULL >> (i * 8 + j));
			     if (b_P && b_O) s.append(" #");
			else if (b_P)        s.append(" X");
			else if (b_O)        s.append(" O");
			else if (b_M)        s.append(" .");
			else                 s.append(" -");
		}
		s.append(" " + std::to_string(8-i) + "\n");
	}
	return s.append("  H G F E D C B A  ");
}

uint64_t StableStonesFullEdges(const uint64_t P, const uint64_t O)
{
	uint64_t mask = 0;
    uint64_t Occ = P | O;
    if ((Occ & 0x00000000000000FFULL) == 0x00000000000000FFULL) mask |= 0x00000000000000FFULL;
    if ((Occ & 0x0101010101010101ULL) == 0x0101010101010101ULL) mask |= 0x0101010101010101ULL;
    if ((Occ & 0xFF00000000000000ULL) == 0xFF00000000000000ULL) mask |= 0xFF00000000000000ULL;
    if ((Occ & 0x8080808080808080ULL) == 0x8080808080808080ULL) mask |= 0x8080808080808080ULL;
    return O & mask;
}
uint64_t StableStonesFullEdgesSecondOrder(const uint64_t P, const uint64_t O)
{
	uint64_t mask = 0;
	uint64_t Occ = P | O;
	if ((Occ & 0x000000000000FFFFULL) == 0x000000000000FFFFULL) mask |= (O << 7) & (O << 8) & (O << 9) & 0x0000000000007E00ULL;
	if ((Occ & 0x0303030303030303ULL) == 0x0303030303030303ULL) mask |= (O << 9) & (O << 1) & (O >> 7) & 0x0002020202020200ULL;
	if ((Occ & 0xFFFF000000000000ULL) == 0xFFFF000000000000ULL) mask |= (O >> 7) & (O >> 8) & (O >> 9) & 0x007E000000000000ULL;
	if ((Occ & 0xC0C0C0C0C0C0C0C0ULL) == 0xC0C0C0C0C0C0C0C0ULL) mask |= (O << 7) & (O >> 1) & (O >> 9) & 0x0040404040404000ULL;
    return O & mask;
}

uint64_t StableStonesTriangles(const uint64_t O)
{
    uint64_t ret = 0;
	if ((O & 0x0000000000000001ULL) == 0x0000000000000001ULL) ret |= 0x0000000000000001ULL;
	if ((O & 0x0000000000000103ULL) == 0x0000000000000103ULL) ret |= 0x0000000000000103ULL;
	if ((O & 0x0000000000010307ULL) == 0x0000000000010307ULL) ret |= 0x0000000000010307ULL;
	if ((O & 0x000000000103070FULL) == 0x000000000103070FULL) ret |= 0x000000000103070FULL;
	if ((O & 0x0000000103070F1FULL) == 0x0000000103070F1FULL) ret |= 0x0000000103070F1FULL;
	if ((O & 0x00000103070F1F3FULL) == 0x00000103070F1F3FULL) ret |= 0x00000103070F1F3FULL;
	if ((O & 0x000103070F1F3F7FULL) == 0x000103070F1F3F7FULL) ret |= 0x000103070F1F3F7FULL;
	if ((O & 0x0103070F1F3F7FFFULL) == 0x0103070F1F3F7FFFULL) ret |= 0x0103070F1F3F7FFFULL;

	if ((O & 0x0000000000000080ULL) == 0x0000000000000080ULL) ret |= 0x0000000000000080ULL;
	if ((O & 0x00000000000080C0ULL) == 0x00000000000080C0ULL) ret |= 0x00000000000080C0ULL;
	if ((O & 0x000000000080C0E0ULL) == 0x000000000080C0E0ULL) ret |= 0x000000000080C0E0ULL;
	if ((O & 0x0000000080C0E0F0ULL) == 0x0000000080C0E0F0ULL) ret |= 0x0000000080C0E0F0ULL;
	if ((O & 0x00000080C0E0F0F8ULL) == 0x00000080C0E0F0F8ULL) ret |= 0x00000080C0E0F0F8ULL;
	if ((O & 0x000080C0E0F0F8FCULL) == 0x000080C0E0F0F8FCULL) ret |= 0x000080C0E0F0F8FCULL;
	if ((O & 0x0080C0E0F0F8FCFEULL) == 0x0080C0E0F0F8FCFEULL) ret |= 0x0080C0E0F0F8FCFEULL;
	if ((O & 0x80C0E0F0F8FCFEFFULL) == 0x80C0E0F0F8FCFEFFULL) ret |= 0x80C0E0F0F8FCFEFFULL;

	if ((O & 0x8000000000000000ULL) == 0x8000000000000000ULL) ret |= 0x8000000000000000ULL;
	if ((O & 0xC080000000000000ULL) == 0xC080000000000000ULL) ret |= 0xC080000000000000ULL;
	if ((O & 0xE0C0800000000000ULL) == 0xE0C0800000000000ULL) ret |= 0xE0C0800000000000ULL;
	if ((O & 0xF0E0C08000000000ULL) == 0xF0E0C08000000000ULL) ret |= 0xF0E0C08000000000ULL;
	if ((O & 0xF8F0E0C080000000ULL) == 0xF8F0E0C080000000ULL) ret |= 0xF8F0E0C080000000ULL;
	if ((O & 0xFCF8F0E0C0800000ULL) == 0xFCF8F0E0C0800000ULL) ret |= 0xFCF8F0E0C0800000ULL;
	if ((O & 0xFEFCF8F0E0C08000ULL) == 0xFEFCF8F0E0C08000ULL) ret |= 0xFEFCF8F0E0C08000ULL;
	if ((O & 0xFFFEFCF8F0E0C080ULL) == 0xFFFEFCF8F0E0C080ULL) ret |= 0xFFFEFCF8F0E0C080ULL;

	if ((O & 0x0100000000000000ULL) == 0x0100000000000000ULL) ret |= 0x0100000000000000ULL;
	if ((O & 0x0301000000000000ULL) == 0x0301000000000000ULL) ret |= 0x0301000000000000ULL;
	if ((O & 0x0703010000000000ULL) == 0x0703010000000000ULL) ret |= 0x0703010000000000ULL;
	if ((O & 0x0F07030100000000ULL) == 0x0F07030100000000ULL) ret |= 0x0F07030100000000ULL;
	if ((O & 0x1F0F070301000000ULL) == 0x1F0F070301000000ULL) ret |= 0x1F0F070301000000ULL;
	if ((O & 0x3F1F0F0703010000ULL) == 0x3F1F0F0703010000ULL) ret |= 0x3F1F0F0703010000ULL;
	if ((O & 0x7F3F1F0F07030100ULL) == 0x7F3F1F0F07030100ULL) ret |= 0x7F3F1F0F07030100ULL;
	if ((O & 0xFF7F3F1F0F070301ULL) == 0xFF7F3F1F0F070301ULL) ret |= 0xFF7F3F1F0F070301ULL;
	return ret;
}

uint64_t StableStonesSkyline(uint64_t O)
{
    int StablesOld, StablesNew;
    uint64_t StableStones;
	StableStones = 0x0000000000000000ULL;
		
	for (int directions = 0; directions < 8; ++directions)
	{
		StablesOld = BitScanLSB(~(O & 0x00000000000000FFULL));
		StableStones |= 0x00000000000000FFULL >> 8-StablesOld;
		for (int counter = 0; (StablesOld > 1) && (counter < 64); counter += 8)
		{
			StablesNew = BitScanLSB(~((O >> counter) & 0x00000000000000FFULL));
			if ((StablesOld != 8) || (StablesNew != 8))
				StablesOld = MIN(StablesOld-1, StablesNew);
			StableStones |= (0x00000000000000FFULL >> 8-StablesOld) << counter;
		}

		switch (directions)
		{
		case 0:
		case 2:
		case 5:
			O = FlipVertical(O);
			StableStones = FlipVertical(StableStones);
			break;
		case 1:
		case 4:
		case 6:
			O = FlipHorizontal(O);
			StableStones = FlipHorizontal(StableStones);
			break;
		case 3:
			O = FlipDiagonal(O);
			StableStones = FlipDiagonal(StableStones);
			break;
		}
	}

    StableStones = FlipDiagonal(StableStones); //Fliping back

    return StableStones;
}



namespace Stability
{
	uint8_t edge_stables[256][256];

	void Initialize()
	{
		unsigned long move;
		unsigned int stables;
		uint64_t flipped;

		memset(edge_stables, 0, 256 * 256 * sizeof(uint8_t));

		for (unsigned int empty = 0; empty < 9; empty++)
			for (unsigned int P = 0; P < 256; P++)
				for (unsigned int O = 0; O < 256; O++)
				{
					if (P & O) continue;
					unsigned int empties = (P | O) ^ 0xFF;
					if (PopCount(empties) == empty)
					{
						stables = 0xFF;
						while (empties)
						{
							move = BitScanLSB(empties); 
							empties &= empties - 1; // RemoveLSB

							// Player plays
							flipped = flip(P, O, move) & 0xFF;
							stables &= edge_stables[P ^ flipped ^ (1 << move)][O ^ flipped] & ~flipped & ~(1 << move);

							// Opponent plays
							flipped = flip(O, P, move) & 0xFF;
							stables &= edge_stables[P ^ flipped][O ^ flipped ^ (1 << move)] & ~flipped & ~(1 << move);

							if (stables == 0) continue;
						}
						edge_stables[P][O] = stables;
					}
				}
		assert(edge_stables[0x80][0x01] == 0x81);
		assert(edge_stables[0xC0][0x00] == 0xC0);
		assert(edge_stables[0x03][0x00] == 0x03);
		assert(edge_stables[0x00][0xC0] == 0xC0);
		assert(edge_stables[0x00][0x03] == 0x03);
		assert(edge_stables[0xC0][0x03] == 0xC3);
		assert(edge_stables[0xC7][0x28] == 0xC7);
		assert(edge_stables[0xA8][0x50] == 0xC0);
		assert(edge_stables[0x16][0x28] == 0x08);
	}
	
	uint64_t FullLineHorizontal(const uint64_t discs)
	{
		// 6 x AND, 6 x CMP, 6 x OR
		uint64_t full = 0;
		if ((discs & 0x00FF000000000000ULL) == 0x00FF000000000000ULL) full |= 0x00FF000000000000ULL;
		if ((discs & 0x0000FF0000000000ULL) == 0x0000FF0000000000ULL) full |= 0x0000FF0000000000ULL;
		if ((discs & 0x000000FF00000000ULL) == 0x000000FF00000000ULL) full |= 0x000000FF00000000ULL;
		if ((discs & 0x00000000FF000000ULL) == 0x00000000FF000000ULL) full |= 0x00000000FF000000ULL;
		if ((discs & 0x0000000000FF0000ULL) == 0x0000000000FF0000ULL) full |= 0x0000000000FF0000ULL;
		if ((discs & 0x000000000000FF00ULL) == 0x000000000000FF00ULL) full |= 0x000000000000FF00ULL;
		return full;
	}
	
	uint64_t FullLineVertival(const uint64_t discs)
	{
		// 6 x AND, 6 x CMP, 6 x OR
		uint64_t full = 0;
		if ((discs & 0x4040404040404040ULL) == 0x4040404040404040ULL) full |= 0x4040404040404040ULL;
		if ((discs & 0x2020202020202020ULL) == 0x2020202020202020ULL) full |= 0x2020202020202020ULL;
		if ((discs & 0x1010101010101010ULL) == 0x1010101010101010ULL) full |= 0x1010101010101010ULL;
		if ((discs & 0x0808080808080808ULL) == 0x0808080808080808ULL) full |= 0x0808080808080808ULL;
		if ((discs & 0x0404040404040404ULL) == 0x0404040404040404ULL) full |= 0x0404040404040404ULL;
		if ((discs & 0x0202020202020202ULL) == 0x0202020202020202ULL) full |= 0x0202020202020202ULL;
		return full;
	}
	
	uint64_t FullLineDiagonal(const uint64_t discs)
	{
		// 11 x AND, 11 x CMP, 11 x OR
		uint64_t full = 0;
		if ((discs & 0x0402010000000000ULL) == 0x0402010000000000ULL) full |= 0x0402010000000000ULL;
		if ((discs & 0x0804020100000000ULL) == 0x0804020100000000ULL) full |= 0x0804020100000000ULL;
		if ((discs & 0x1008040201000000ULL) == 0x1008040201000000ULL) full |= 0x1008040201000000ULL;
		if ((discs & 0x2010080402010000ULL) == 0x2010080402010000ULL) full |= 0x2010080402010000ULL;
		if ((discs & 0x4020100804020100ULL) == 0x4020100804020100ULL) full |= 0x4020100804020100ULL;
		if ((discs & 0x8040201008040201ULL) == 0x8040201008040201ULL) full |= 0x8040201008040201ULL;
		if ((discs & 0x0080402010080402ULL) == 0x0080402010080402ULL) full |= 0x0080402010080402ULL;
		if ((discs & 0x0000804020100804ULL) == 0x0000804020100804ULL) full |= 0x0000804020100804ULL;
		if ((discs & 0x0000008040201008ULL) == 0x0000008040201008ULL) full |= 0x0000008040201008ULL;
		if ((discs & 0x0000000080402010ULL) == 0x0000000080402010ULL) full |= 0x0000000080402010ULL;
		if ((discs & 0x0000000000804020ULL) == 0x0000000000804020ULL) full |= 0x0000000000804020ULL;
		return full;
	}
	
	uint64_t FullLineCodiagonal(const uint64_t discs)
	{
		// 11 x AND, 11 x CMP, 11 x OR
		uint64_t full = 0;
		if ((discs & 0x2040800000000000ULL) == 0x2040800000000000ULL) full |= 0x2040800000000000ULL;
		if ((discs & 0x1020408000000000ULL) == 0x1020408000000000ULL) full |= 0x1020408000000000ULL;
		if ((discs & 0x0810204080000000ULL) == 0x0810204080000000ULL) full |= 0x0810204080000000ULL;
		if ((discs & 0x0408102040800000ULL) == 0x0408102040800000ULL) full |= 0x0408102040800000ULL;
		if ((discs & 0x0204081020408000ULL) == 0x0204081020408000ULL) full |= 0x0204081020408000ULL;
		if ((discs & 0x0102040810204080ULL) == 0x0102040810204080ULL) full |= 0x0102040810204080ULL;
		if ((discs & 0x0001020408102040ULL) == 0x0001020408102040ULL) full |= 0x0001020408102040ULL;
		if ((discs & 0x0000010204081020ULL) == 0x0000010204081020ULL) full |= 0x0000010204081020ULL;
		if ((discs & 0x0000000102040810ULL) == 0x0000000102040810ULL) full |= 0x0000000102040810ULL;
		if ((discs & 0x0000000001020408ULL) == 0x0000000001020408ULL) full |= 0x0000000001020408ULL;
		if ((discs & 0x0000000000010204ULL) == 0x0000000000010204ULL) full |= 0x0000000000010204ULL;
		return full;
	}
}

uint64_t StableEdges(const uint64_t P, const uint64_t O)
{
	return Stability::edge_stables[P & 0xFF][O & 0xFF]
		| (static_cast<uint64_t>(Stability::edge_stables[P >> 56][O >> 56]) << 56)
		| PDep(Stability::edge_stables[PExt(P, 0x8080808080808080ULL)][PExt(O, 0x8080808080808080ULL)], 0x8080808080808080ULL)
		| PDep(Stability::edge_stables[PExt(P, 0x0101010101010101ULL)][PExt(O, 0x0101010101010101ULL)], 0x0101010101010101ULL);
}

uint64_t StableStonesPlayer(const uint64_t P, const uint64_t O)
{
	const uint64_t discs = P | O;
	uint64_t full_h = Stability::FullLineHorizontal(discs);
	uint64_t full_v = Stability::FullLineVertival(discs);
	uint64_t full_d = Stability::FullLineDiagonal(discs);
	uint64_t full_c = Stability::FullLineCodiagonal(discs);
	uint64_t new_stables = StableEdges(P, O) & P;
	new_stables |= full_h & full_v & full_d & full_c & P & 0x007E7E7E7E7E7E00ULL;

	uint64_t stables = 0;
	while (new_stables & ~stables)
	{
		stables |= new_stables;
		uint64_t stables_h = (stables >> 1) | (stables << 1) | full_h;
		uint64_t stables_v = (stables >> 8) | (stables << 8) | full_v;
		uint64_t stables_d = (stables >> 9) | (stables << 9) | full_d;
		uint64_t stables_c = (stables >> 7) | (stables << 7) | full_c;
		new_stables = stables_h & stables_v & stables_d & stables_c & P & 0x007E7E7E7E7E7E00ULL;
	}
	return stables;
}

//uint64_t StableStones(const uint64_t P, const uint64_t O)
//{
//	return StableStonesPlayer(P, O) | StableStonesPlayer(O, P);
//}