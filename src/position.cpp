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
