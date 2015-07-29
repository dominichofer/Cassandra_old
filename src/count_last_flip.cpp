#include "count_last_flip.h"

namespace CountLastFlip
{
	uint8_t CLF_0[256];
	uint8_t CLF_1[256];
	uint8_t CLF_2[256];
	uint8_t CLF_3[256];
	uint8_t CLF_4[256];
	uint8_t CLF_5[256];
	uint8_t CLF_6[256];
	uint8_t CLF_7[256];

	uint8_t flip_loop_dir(const uint8_t P, const uint8_t O, const uint8_t move, const int dX)
	{
		uint8_t flips = 0;
		uint8_t tmp;
		int i = move + dX; // Starting index in x direction

		while ((i >= 0) && (i < 8)) // In between boundaries
		{
			tmp = 1ULL << i; // The bit to look at
			if (O & tmp) // The bit belongs to the opponent
				flips |= tmp; // Add to possible flips
			else if (P & tmp) // The bit belongs to the player
				return flips; // All possible flips become real flips
			else // The bit belongs to no player
				return 0; // There are no possible flips
			i += dX; // Advance in direction
		}
		return 0;
	}

	uint8_t CountFlip(const uint8_t i, const uint8_t move)
	{
		if (i & (1 << move)) return 0;
		uint64_t mask = 0xFFULL ^ (1ULL << move);
		uint8_t P = i;
		uint8_t O = i ^ mask;
		return 2 * PopCount(flip_loop_dir(P, O, move, +1) | flip_loop_dir(P, O, move, -1));
	}

	void Initialize()
	{
		for (int i = 0; i < 256; i++) CLF_0[i] = CountFlip(i, 0);
		for (int i = 0; i < 256; i++) CLF_1[i] = CountFlip(i, 1);
		for (int i = 0; i < 256; i++) CLF_2[i] = CountFlip(i, 2);
		for (int i = 0; i < 256; i++) CLF_3[i] = CountFlip(i, 3);
		for (int i = 0; i < 256; i++) CLF_4[i] = CountFlip(i, 4);
		for (int i = 0; i < 256; i++) CLF_5[i] = CountFlip(i, 5);
		for (int i = 0; i < 256; i++) CLF_6[i] = CountFlip(i, 6);
		for (int i = 0; i < 256; i++) CLF_7[i] = CountFlip(i, 7);
	}

	uint8_t CLF_A1(const uint64_t P) { return CLF_0[P & 0xFFULL] + CLF_0[PExt(P, 0x0101010101010101ULL)] + CLF_0[PExt(P, 0x8040201008040201ULL)]; }
	uint8_t CLF_B1(const uint64_t P) { return CLF_1[P & 0xFFULL] + CLF_0[PExt(P, 0x0202020202020202ULL)] + CLF_0[PExt(P, 0x0080402010080402ULL)]; }
	uint8_t CLF_C1(const uint64_t P) { return CLF_2[P & 0xFFULL] + CLF_0[PExt(P, 0x0404040404040404ULL)] + CLF_2[((P & 0x0000804020110A04ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_D1(const uint64_t P) { return CLF_3[P & 0xFFULL] + CLF_0[PExt(P, 0x0808080808080808ULL)] + CLF_3[((P & 0x0000008041221408ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_E1(const uint64_t P) { return CLF_4[P & 0xFFULL] + CLF_0[PExt(P, 0x1010101010101010ULL)] + CLF_4[((P & 0x0000000182442810ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_F1(const uint64_t P) { return CLF_5[P & 0xFFULL] + CLF_0[PExt(P, 0x2020202020202020ULL)] + CLF_5[((P & 0x0000010204885020ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_G1(const uint64_t P) { return CLF_6[P & 0xFFULL] + CLF_0[PExt(P, 0x4040404040404040ULL)] + CLF_0[PExt(P, 0x0001020408102040ULL)]; }
	uint8_t CLF_H1(const uint64_t P) { return CLF_7[P & 0xFFULL] + CLF_0[PExt(P, 0x8080808080808080ULL)] + CLF_0[PExt(P, 0x0102040810204080ULL)]; }
	
	uint8_t CLF_A2(const uint64_t P) { return CLF_0[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x0101010101010101ULL)] + CLF_0[PExt(P, 0x4020100804020100ULL)]; }
	uint8_t CLF_B2(const uint64_t P) { return CLF_1[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x0202020202020202ULL)] + CLF_0[PExt(P, 0x8040201008040200ULL)]; }
	uint8_t CLF_C2(const uint64_t P) { return CLF_2[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x0404040404040404ULL)] + CLF_2[((P & 0x00804020110A0400ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_D2(const uint64_t P) { return CLF_3[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x0808080808080808ULL)] + CLF_3[((P & 0x0000804122140800ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_E2(const uint64_t P) { return CLF_4[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x1010101010101010ULL)] + CLF_4[((P & 0x0000018244281000ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_F2(const uint64_t P) { return CLF_5[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x2020202020202020ULL)] + CLF_5[((P & 0x0001020488502000ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_G2(const uint64_t P) { return CLF_6[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x4040404040404040ULL)] + CLF_0[PExt(P, 0x0102040810204000ULL)]; }
	uint8_t CLF_H2(const uint64_t P) { return CLF_7[BExtr(P, 8, 8)] + CLF_1[PExt(P, 0x8080808080808080ULL)] + CLF_0[PExt(P, 0x0204081020408000ULL)]; }
	
	uint8_t CLF_A3(const uint64_t P) { return CLF_0[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x0101010101010101ULL)] + CLF_2[PExt(P, 0x2010080402010204ULL)]; }
	uint8_t CLF_B3(const uint64_t P) { return CLF_1[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x0202020202020202ULL)] + CLF_2[PExt(P, 0x4020100804020408ULL)]; }
	uint8_t CLF_C3(const uint64_t P) { return CLF_2[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x0404040404040404ULL)] + CLF_2[PExt(P, 0x8040201008040201ULL)] + CLF_2[PExt(P, 0x0000000102040810ULL)]; }
	uint8_t CLF_D3(const uint64_t P) { return CLF_3[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x0808080808080808ULL)] + CLF_2[PExt(P, 0x0080402010080402ULL)] + CLF_2[PExt(P, 0x0000010204081020ULL)]; }
	uint8_t CLF_E3(const uint64_t P) { return CLF_4[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x1010101010101010ULL)] + CLF_2[PExt(P, 0x0000804020100804ULL)] + CLF_2[PExt(P, 0x0001020408102040ULL)]; }
	uint8_t CLF_F3(const uint64_t P) { return CLF_5[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x2020202020202020ULL)] + CLF_2[PExt(P, 0x0000008040201008ULL)] + CLF_2[PExt(P, 0x0102040810204080ULL)]; }
	uint8_t CLF_G3(const uint64_t P) { return CLF_6[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x4040404040404040ULL)] + CLF_2[PExt(P, 0x0204081020402010ULL)]; }
	uint8_t CLF_H3(const uint64_t P) { return CLF_7[BExtr(P, 16, 8)] + CLF_2[PExt(P, 0x8080808080808080ULL)] + CLF_2[PExt(P, 0x0408102040804020ULL)]; }
	
	uint8_t CLF_A4(const uint64_t P) { return CLF_0[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x0101010101010101ULL)] + CLF_3[PExt(P, 0x1008040201020408ULL)]; }
	uint8_t CLF_B4(const uint64_t P) { return CLF_1[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x0202020202020202ULL)] + CLF_3[PExt(P, 0x2010080402040810ULL)]; }
	uint8_t CLF_C4(const uint64_t P) { return CLF_2[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x0404040404040404ULL)] + CLF_2[PExt(P, 0x4020100804020100ULL)] + CLF_3[PExt(P, 0x0000010204081020ULL)]; }
	uint8_t CLF_D4(const uint64_t P) { return CLF_3[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x0808080808080808ULL)] + CLF_3[PExt(P, 0x8040201008040201ULL)] + CLF_3[PExt(P, 0x0001020408102040ULL)]; }
	uint8_t CLF_E4(const uint64_t P) { return CLF_4[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x1010101010101010ULL)] + CLF_3[PExt(P, 0x0080402010080402ULL)] + CLF_3[PExt(P, 0x0102040810204080ULL)]; }
	uint8_t CLF_F4(const uint64_t P) { return CLF_5[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x2020202020202020ULL)] + CLF_3[PExt(P, 0x00008040201008040LL)] + CLF_2[PExt(P, 0x0204081020408000ULL)]; }
	uint8_t CLF_G4(const uint64_t P) { return CLF_6[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x4040404040404040ULL)] + CLF_3[PExt(P, 0x0408102040201008ULL)]; }
	uint8_t CLF_H4(const uint64_t P) { return CLF_7[BExtr(P, 24, 8)] + CLF_3[PExt(P, 0x8080808080808080ULL)] + CLF_3[PExt(P, 0x0810204080402010ULL)]; }
	
	uint8_t CLF_A5(const uint64_t P) { return CLF_0[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x0101010101010101ULL)] + CLF_4[PExt(P, 0x0804020102040810ULL)]; }
	uint8_t CLF_B5(const uint64_t P) { return CLF_1[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x0202020202020202ULL)] + CLF_4[PExt(P, 0x1008040204081020ULL)]; }
	uint8_t CLF_C5(const uint64_t P) { return CLF_2[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x0404040404040404ULL)] + CLF_2[PExt(P, 0x2010080402010000ULL)] + CLF_4[PExt(P, 0x0001020408102040ULL)]; }
	uint8_t CLF_D5(const uint64_t P) { return CLF_3[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x0808080808080808ULL)] + CLF_3[PExt(P, 0x4020100804020100ULL)] + CLF_4[PExt(P, 0x0102040810204080ULL)]; }
	uint8_t CLF_E5(const uint64_t P) { return CLF_4[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x1010101010101010ULL)] + CLF_4[PExt(P, 0x8040201008040201ULL)] + CLF_3[PExt(P, 0x0204081020408000ULL)]; }
	uint8_t CLF_F5(const uint64_t P) { return CLF_5[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x2020202020202020ULL)] + CLF_4[PExt(P, 0x0080402010080402ULL)] + CLF_2[PExt(P, 0x0408102040800000ULL)]; }
	uint8_t CLF_G5(const uint64_t P) { return CLF_6[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x4040404040404040ULL)] + CLF_4[PExt(P, 0x0810204020100804ULL)]; }
	uint8_t CLF_H5(const uint64_t P) { return CLF_7[BExtr(P, 32, 8)] + CLF_4[PExt(P, 0x8080808080808080ULL)] + CLF_4[PExt(P, 0x1020408040201008ULL)]; }
	
	uint8_t CLF_A6(const uint64_t P) { return CLF_0[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x0101010101010101ULL)] + CLF_5[PExt(P, 0x0402010204081020ULL)]; }
	uint8_t CLF_B6(const uint64_t P) { return CLF_1[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x0202020202020202ULL)] + CLF_5[PExt(P, 0x0804020408102040ULL)]; }
	uint8_t CLF_C6(const uint64_t P) { return CLF_2[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x0404040404040404ULL)] + CLF_2[PExt(P, 0x1008040201000000ULL)] + CLF_5[PExt(P, 0x0102040810204080ULL)]; }
	uint8_t CLF_D6(const uint64_t P) { return CLF_3[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x0808080808080808ULL)] + CLF_3[PExt(P, 0x2010080402010000ULL)] + CLF_4[PExt(P, 0x0204081020408000ULL)]; }
	uint8_t CLF_E6(const uint64_t P) { return CLF_4[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x1010101010101010ULL)] + CLF_4[PExt(P, 0x4020100804020100ULL)] + CLF_3[PExt(P, 0x0408102040800000ULL)]; }
	uint8_t CLF_F6(const uint64_t P) { return CLF_5[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x2020202020202020ULL)] + CLF_5[PExt(P, 0x8040201008040201ULL)] + CLF_2[PExt(P, 0x0810204080000000ULL)]; }
	uint8_t CLF_G6(const uint64_t P) { return CLF_6[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x4040404040404040ULL)] + CLF_5[PExt(P, 0x1020402010080402ULL)]; }
	uint8_t CLF_H6(const uint64_t P) { return CLF_7[BExtr(P, 40, 8)] + CLF_5[PExt(P, 0x8080808080808080ULL)] + CLF_5[PExt(P, 0x2040804020100804ULL)]; }
	
	uint8_t CLF_A7(const uint64_t P) { return CLF_0[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x0101010101010101ULL)] + CLF_6[PExt(P, 0x0001020408102040ULL)]; }
	uint8_t CLF_B7(const uint64_t P) { return CLF_1[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x0202020202020202ULL)] + CLF_6[PExt(P, 0x0002040810204080ULL)]; }
	uint8_t CLF_C7(const uint64_t P) { return CLF_2[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x0404040404040404ULL)] + CLF_2[((P & 0x00040A1120408000ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_D7(const uint64_t P) { return CLF_3[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x0808080808080808ULL)] + CLF_3[((P & 0x0008142241800000ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_E7(const uint64_t P) { return CLF_4[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x1010101010101010ULL)] + CLF_4[((P & 0x0010284482010000ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_F7(const uint64_t P) { return CLF_5[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x2020202020202020ULL)] + CLF_5[((P & 0x0020508804020100ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_G7(const uint64_t P) { return CLF_6[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x4040404040404040ULL)] + CLF_6[PExt(P, 0x0040201008040201ULL)]; }
	uint8_t CLF_H7(const uint64_t P) { return CLF_7[BExtr(P, 48, 8)] + CLF_6[PExt(P, 0x8080808080808080ULL)] + CLF_6[PExt(P, 0x0080402010080402ULL)]; }
	
	uint8_t CLF_A8(const uint64_t P) { return CLF_0[P >> 56] + CLF_7[PExt(P, 0x0101010101010101ULL)] + CLF_7[PExt(P, 0x0102040810204080ULL)]; }
	uint8_t CLF_B8(const uint64_t P) { return CLF_1[P >> 56] + CLF_7[PExt(P, 0x0202020202020202ULL)] + CLF_6[PExt(P, 0x0204081020408000ULL)]; }
	uint8_t CLF_C8(const uint64_t P) { return CLF_2[P >> 56] + CLF_7[PExt(P, 0x0404040404040404ULL)] + CLF_2[((P & 0x040A112040800000ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_D8(const uint64_t P) { return CLF_3[P >> 56] + CLF_7[PExt(P, 0x0808080808080808ULL)] + CLF_3[((P & 0x0814224180000000ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_E8(const uint64_t P) { return CLF_4[P >> 56] + CLF_7[PExt(P, 0x1010101010101010ULL)] + CLF_4[((P & 0x1028448201000000ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_F8(const uint64_t P) { return CLF_5[P >> 56] + CLF_7[PExt(P, 0x2020202020202020ULL)] + CLF_5[((P & 0x2050880402010000ULL) * 0x0101010101010101ULL) >> 56]; }
	uint8_t CLF_G8(const uint64_t P) { return CLF_6[P >> 56] + CLF_7[PExt(P, 0x4040404040404040ULL)] + CLF_6[PExt(P, 0x4020100804020100ULL)]; }
	uint8_t CLF_H8(const uint64_t P) { return CLF_7[P >> 56] + CLF_7[PExt(P, 0x8080808080808080ULL)] + CLF_7[PExt(P, 0x8040201008040201ULL)]; }
	
	uint8_t (*CLF_fp[])(const uint64_t) = {
		CLF_A1, CLF_B1, CLF_C1, CLF_D1,	CLF_E1, CLF_F1, CLF_G1, CLF_H1,
		CLF_A2, CLF_B2, CLF_C2, CLF_D2,	CLF_E2, CLF_F2, CLF_G2, CLF_H2,
		CLF_A3, CLF_B3, CLF_C3, CLF_D3,	CLF_E3, CLF_F3, CLF_G3, CLF_H3,
		CLF_A4, CLF_B4, CLF_C4, CLF_D4,	CLF_E4, CLF_F4, CLF_G4, CLF_H4,
		CLF_A5, CLF_B5, CLF_C5, CLF_D5,	CLF_E5, CLF_F5, CLF_G5, CLF_H5,
		CLF_A6, CLF_B6, CLF_C6, CLF_D6,	CLF_E6, CLF_F6, CLF_G6, CLF_H6,
		CLF_A7, CLF_B7, CLF_C7, CLF_D7,	CLF_E7, CLF_F7, CLF_G7, CLF_H7,
		CLF_A8, CLF_B8, CLF_C8, CLF_D8,	CLF_E8, CLF_F8, CLF_G8, CLF_H8
	};
}