#include "possiblemoves.h"

// ################################
// ## If there is no useful SIMD ##
// ################################
#if !defined(HAS_SSE2) && !defined(HAS_AVX2) && !defined(HAS_AVX152)
	template <const int dir>
	inline uint64_t get_some_moves(const uint64_t P, const uint64_t mask)
	{
		// kogge-stone parallel prefix
		// 12 x SHIFT, 9 x AND, 7 x OR
		// = 28 OPs
		uint64_t flip_l, flip_r;
		uint64_t mask_l, mask_r;

		flip_l  = mask & (P << dir);
		flip_r  = mask & (P >> dir);

		flip_l |= mask & (flip_l << dir);
		flip_r |= mask & (flip_r >> dir);

		mask_l  = mask & (mask << dir);	mask_r = mask_l >> dir;

		flip_l |= mask_l & (flip_l << (dir * 2));
		flip_r |= mask_r & (flip_r >> (dir * 2));

		flip_l |= mask_l & (flip_l << (dir * 2));
		flip_r |= mask_r & (flip_r >> (dir * 2));

		flip_l <<= dir;
		flip_r >>= dir;

		return flip_l | flip_r;
	}

	uint64_t HasMoves(const uint64_t P, const uint64_t O)
	{
		const uint64_t empties = ~(P | O);
		if (get_some_moves<1>(P, O & 0x7E7E7E7E7E7E7E7EULL) & empties) return 1;
		if (get_some_moves<8>(P, O & 0x00FFFFFFFFFFFF00ULL) & empties) return 1;
		if (get_some_moves<7>(P, O & 0x007E7E7E7E7E7E00ULL) & empties) return 1;
		if (get_some_moves<9>(P, O & 0x007E7E7E7E7E7E00ULL) & empties) return 1;
		return 0;
	}

	uint64_t PossibleMoves(const uint64_t P, const uint64_t O)
	{
		// 48 x SHIFT, 42 x AND, 32 x OR, 1 x NOT
		// = 123 OPs
		uint64_t mask1, mask2, mask3, mask4, mask5, mask6, mask7, mask8;
		uint64_t flip1, flip2, flip3, flip4, flip5, flip6, flip7, flip8;

		// 1 x AND
		const uint64_t maskO = O & 0x7E7E7E7E7E7E7E7EULL;

		// 8 x SHIFT, 8 x AND
		flip1 = maskO & (P << 1);
		flip2 = maskO & (P >> 1);
		flip3 =     O & (P << 8);
		flip4 =     O & (P >> 8);
		flip5 = maskO & (P << 7);
		flip6 = maskO & (P >> 7);
		flip7 = maskO & (P << 9);
		flip8 = maskO & (P >> 9);

		// 8 x SHIFT, 8 x AND, 8 x OR
		flip1 |= maskO & (flip1 << 1);
		flip2 |= maskO & (flip2 >> 1);
		flip3 |=     O & (flip3 << 8);
		flip4 |=     O & (flip4 >> 8);
		flip5 |= maskO & (flip5 << 7);
		flip6 |= maskO & (flip6 >> 7);
		flip7 |= maskO & (flip7 << 9);
		flip8 |= maskO & (flip8 >> 9);

		// 8 x SHIFT, 8 x AND
		mask1 = maskO & (maskO << 1); mask2 = mask1 >> 1;
		mask3 =     O & (    O << 8); mask4 = mask3 >> 8;
		mask5 = maskO & (maskO << 7); mask6 = mask5 >> 7;
		mask7 = maskO & (maskO << 9); mask8 = mask7 >> 9;

		// 8 x SHIFT, 8 x AND, 8 x OR
		flip1 |= mask1 & (flip1 <<  2);
		flip2 |= mask2 & (flip2 >>  2);
		flip3 |= mask3 & (flip3 << 16);
		flip4 |= mask4 & (flip4 >> 16);
		flip5 |= mask5 & (flip5 << 14);
		flip6 |= mask6 & (flip6 >> 14);
		flip7 |= mask7 & (flip7 << 18);
		flip8 |= mask8 & (flip8 >> 18);

		// 8 x SHIFT, 8 x AND, 8 x OR
		flip1 |= mask1 & (flip1 <<  2);
		flip2 |= mask2 & (flip2 >>  2);
		flip3 |= mask3 & (flip3 << 16);
		flip4 |= mask4 & (flip4 >> 16);
		flip5 |= mask5 & (flip5 << 14);
		flip6 |= mask6 & (flip6 >> 14);
		flip7 |= mask7 & (flip7 << 18);
		flip8 |= mask8 & (flip8 >> 18);
	
		// 8 x SHIFT
		flip1 <<= 1;
		flip2 >>= 1;
		flip3 <<= 8;
		flip4 >>= 8;
		flip5 <<= 7;
		flip6 >>= 7;
		flip7 <<= 9;
		flip8 >>= 9;

		// 1 x AND, 8 x OR, 1 x NOT
		return ~(P | O) & (flip1 | flip2 | flip3 | flip4 | flip5 | flip6 | flip7 | flip8);
	}
#endif
