#include "flip_parallel.h"

uint8_t OUTFLANK[8][64];
uint8_t FLIPS[8][256];
uint64_t STRETCH[256];
uint64_t MASK_D[64];
uint64_t MASK_C[64];

void Initialize_Outflank()
{
	uint8_t O, k, outf;
	for (uint8_t j = 0; j < 8; j++)
	{
		for (uint8_t i = 0; i < 64; i++)
		{
			outf = 0;
			O = i << 1;

			k = j + 1;
			while (O & (1 << k))
				k++;
			if (k != j + 1) // There was an outflank
				outf |= 1 << k;

			k = j - 1;
			while (O & (1 << k))
				k--;
			if (k != j - 1) // There was an outflank
				outf |= 1 << k;

			OUTFLANK[j][i] = outf;
		}
	}
}

void Initialize_Flips()
{
	for (unsigned int i = 0; i < 8; i++)
		for (unsigned int j = 0; j < 256; j++)
			FLIPS[i][j] = 0;

	// 0000 000X
	FLIPS[0][0x04] = 0x02;
	FLIPS[0][0x08] = 0x06;
	FLIPS[0][0x10] = 0x0E;
	FLIPS[0][0x20] = 0x1E;
	FLIPS[0][0x40] = 0x3E;
	FLIPS[0][0x80] = 0x7E;

	// 0000 00X0
	FLIPS[1][0x08] = 0x04;
	FLIPS[1][0x10] = 0x0C;
	FLIPS[1][0x20] = 0x1C;
	FLIPS[1][0x40] = 0x3C;
	FLIPS[1][0x80] = 0x7C;

	// 0000 0X00
	FLIPS[2][0x01] = 0x02;
	FLIPS[2][0x10] = 0x08;
	FLIPS[2][0x11] = 0x0A;
	FLIPS[2][0x20] = 0x18;
	FLIPS[2][0x21] = 0x1A;
	FLIPS[2][0x40] = 0x38;
	FLIPS[2][0x41] = 0x3A;
	FLIPS[2][0x80] = 0x78;
	FLIPS[2][0x81] = 0x7A;

	// 0000 X000
	FLIPS[3][0x01] = 0x06;
	FLIPS[3][0x02] = 0x04;
	FLIPS[3][0x20] = 0x10;
	FLIPS[3][0x21] = 0x16;
	FLIPS[3][0x22] = 0x14;
	FLIPS[3][0x40] = 0x30;
	FLIPS[3][0x41] = 0x36;
	FLIPS[3][0x42] = 0x34;
	FLIPS[3][0x80] = 0x70;
	FLIPS[3][0x81] = 0x76;
	FLIPS[3][0x82] = 0x74;

	// 000X 0000
	FLIPS[4][0x01] = 0x0E;
	FLIPS[4][0x02] = 0x0C;
	FLIPS[4][0x04] = 0x08;
	FLIPS[4][0x40] = 0x20;
	FLIPS[4][0x41] = 0x2E;
	FLIPS[4][0x42] = 0x2C;
	FLIPS[4][0x44] = 0x28;
	FLIPS[4][0x80] = 0x60;
	FLIPS[4][0x81] = 0x6E;
	FLIPS[4][0x82] = 0x6C;
	FLIPS[4][0x84] = 0x68;

	// 00X0 0000
	FLIPS[5][0x01] = 0x1E;
	FLIPS[5][0x02] = 0x1C;
	FLIPS[5][0x04] = 0x18;
	FLIPS[5][0x08] = 0x10;
	FLIPS[5][0x80] = 0x40;
	FLIPS[5][0x81] = 0x5E;
	FLIPS[5][0x82] = 0x5C;
	FLIPS[5][0x84] = 0x58;
	FLIPS[5][0x88] = 0x50;

	// 0X00 0000
	FLIPS[6][0x01] = 0x3E;
	FLIPS[6][0x02] = 0x3C;
	FLIPS[6][0x04] = 0x38;
	FLIPS[6][0x08] = 0x30;
	FLIPS[6][0x10] = 0x20;

	// X000 0000
	FLIPS[7][0x01] = 0x7E;
	FLIPS[7][0x02] = 0x7C;
	FLIPS[7][0x04] = 0x78;
	FLIPS[7][0x08] = 0x70;
	FLIPS[7][0x10] = 0x60;
	FLIPS[7][0x20] = 0x40;
}

void Initialize_Stretch()
{
	for (uint64_t i = 0; i < 256; i++)
		STRETCH[i] = ((i * 0x0102040810204080ULL) & 0x0101010101010101ULL) * 0xFFULL;
}

void Initialize_Mask_d()
{
	for (unsigned int i = 0; i < 64; i++)
	{
		unsigned int L = i % 8;
		unsigned int N = i / 8;
		if (L > N) MASK_D[i] = 0x8040201008040201ULL >> ((L-N)*8);
			  else MASK_D[i] = 0x8040201008040201ULL << ((N-L)*8);
	}
}

void Initialize_Mask_c()
{
	for (unsigned int i = 0; i < 64; i++)
	{
		unsigned int L = i % 8;
		unsigned int N = i / 8;
		if (N + L > 7) MASK_C[i] = 0x0102040810204080ULL << ( (N+L-7)*8);
				  else MASK_C[i] = 0x0102040810204080ULL >> (-(N+L-7)*8);
	}
}



uint64_t flip_h(uint64_t P, uint64_t O, const uint8_t move)
{
	O = (O >> ((move & 0xF8) + 1)) & 0x3FULL;
	P = (P >>  (move & 0xF8)     ) & 0xFFULL;
	const uint64_t outflank = OUTFLANK[move & 7][O] & P;
	return static_cast<uint64_t>(FLIPS[move & 7][outflank]) << (move & 0xF8);
}

uint64_t flip_v(uint64_t P, uint64_t O, const uint8_t move)
{
	O = ((O & (0x0001010101010100ULL << (move & 7))) * (0x0102040810204080ULL >> (move & 7))) >> 57;
	P = ((P & (0x0101010101010101ULL << (move & 7))) * (0x0102040810204080ULL >> (move & 7))) >> 56;
	const uint64_t outflank = OUTFLANK[(move >> 3)][O] & P;
	return STRETCH[FLIPS[(move >> 3)][outflank]] & (0x0101010101010101ULL << (move & 7));
}

uint64_t flip_d(uint64_t P, uint64_t O, const uint8_t move)
{
	O = ((O & MASK_D[move] & 0x007E7E7E7E7E7E00ULL) * 0x0101010101010101ULL) >> 57;
	P = ((P & MASK_D[move]                        ) * 0x0101010101010101ULL) >> 56;
	const uint64_t outflank = OUTFLANK[move & 7][O] & P;
	return (FLIPS[move & 7][outflank] * 0x0101010101010101ULL) & MASK_D[move];
}

uint64_t flip_c(uint64_t P, uint64_t O, const uint8_t move)
{
	O = ((O & MASK_C[move] & 0x007E7E7E7E7E7E00ULL) * 0x0101010101010101ULL) >> 57;
	P = ((P & MASK_C[move]                        ) * 0x0101010101010101ULL) >> 56;
	const uint64_t outflank = OUTFLANK[move & 7][O] & P;
	return (FLIPS[move & 7][outflank] * 0x0101010101010101ULL) & MASK_C[move];
}

namespace Flip_Parallel
{
	void Initialize()
	{
		Initialize_Outflank();
		Initialize_Flips();
		Initialize_Stretch();
		Initialize_Mask_d();
		Initialize_Mask_c();
	}
}

uint64_t flip(const uint64_t P, const uint64_t O, const uint8_t move)
{
    return flip_h(P, O, move)
         | flip_v(P, O, move)
         | flip_d(P, O, move)
         | flip_c(P, O, move);
}
