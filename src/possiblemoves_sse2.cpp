#include "possiblemoves.h"

// ###########################
// ## If there only is SSE2 ##
// ###########################
#if defined(HAS_SSE2) && !defined(HAS_AVX2) && !defined(HAS_AVX152)
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
		// 30 x SHIFT, 28 x AND, 21 x OR, 1 x NOT, 2 x BSWAP
		// = 82 OPs
		uint64_t mask1, mask2, mask6, mask7, mask8;
		uint64_t flip1, flip2, flip6, flip7, flip8;
		__m128i mask3, mask4, mask5;
		__m128i flip3, flip4, flip5;
	
		// 2 x MOV, 2 x BSWAP
		const __m128i PP = _mm_set_epi64x(BSwap(P), P);
		const __m128i OO = _mm_set_epi64x(BSwap(O), O);

		// 2 x AND
		const uint64_t maskO  = O  &                 0x7E7E7E7E7E7E7E7EULL;
		const __m128i  maskOO = OO & _mm_set1_epi64x(0x7E7E7E7E7E7E7E7EULL);

		// 5 x SHIFT, 5 x AND
		flip1 = maskO  & (P << 1);
		flip2 = maskO  & (P >> 1);
		flip3 =     OO & _mm_slli_epi64(PP, 8);
		flip4 = maskOO & _mm_slli_epi64(PP, 7);
		flip5 = maskOO & _mm_srli_epi64(PP, 7);

		// 5 x SHIFT, 5 x AND, 5 x OR
		flip1 |= maskO  & (flip1 << 1);
		flip2 |= maskO  & (flip2 >> 1);
		flip3 |=     OO & _mm_slli_epi64(flip3, 8);
		flip4 |= maskOO & _mm_slli_epi64(flip4, 7);
		flip5 |= maskOO & _mm_srli_epi64(flip5, 7);

		// 5 x SHIFT, 5 x AND
		mask1 = maskO  & (maskO << 1);              mask2 = (mask1 >> 1);
		mask3 =     OO & _mm_slli_epi64(    OO, 8);
		mask4 = maskOO & _mm_slli_epi64(maskOO, 7); mask5 = _mm_srli_epi64(mask4, 7);

		// 5 x SHIFT, 5 x AND, 5 x OR
		flip1 |= mask1 & (flip1 << 2);
		flip2 |= mask2 & (flip2 >> 2);
		flip3 |= mask3 & _mm_slli_epi64(flip3, 16);
		flip4 |= mask4 & _mm_slli_epi64(flip4, 14);
		flip5 |= mask5 & _mm_srli_epi64(flip5, 14);

		// 5 x SHIFT, 5 x AND, 5 x OR
		flip1 |= mask1 & (flip1 << 2);
		flip2 |= mask2 & (flip2 >> 2);
		flip3 |= mask3 & _mm_slli_epi64(flip3, 16);
		flip4 |= mask4 & _mm_slli_epi64(flip4, 14);
		flip5 |= mask5 & _mm_srli_epi64(flip5, 14);
	
		// 5 x SHIFT
		flip1 <<= 1;
		flip2 >>= 1;
		flip3 = _mm_slli_epi64(flip3, 8);
		flip4 = _mm_slli_epi64(flip4, 7);
		flip5 = _mm_srli_epi64(flip5, 7);

		// 2 x OR
		flip3 |= flip4 | flip5;

		// 1 x AND, 4 x OR, 1 x NOT
		return ~(P | O) & (flip1 | flip2 | BSwap(_mm_extract_epi64(flip3, 1)) | _mm_extract_epi64(flip3, 0));
	}


	// SSE2 specific code
	__m128i PossibleMoves(const __m128i P, const __m128i O)
	{
		// 48 x SHIFT, 42 x AND, 32 x OR, 1 x NOT
		// = 123 OPs
		__m128i mask1, mask2, mask3, mask4, mask5, mask6, mask7, mask8;
		__m128i flip1, flip2, flip3, flip4, flip5, flip6, flip7, flip8;

		// 1 x AND
		const __m128i maskO = O & _mm_set1_epi64x(0x7E7E7E7E7E7E7E7EULL);

		// 8 x SHIFT, 8 x AND
		flip1 = maskO & _mm_slli_epi64(P, 1);
		flip2 = maskO & _mm_srli_epi64(P, 1);
		flip3 =     O & _mm_slli_epi64(P, 8);
		flip4 =     O & _mm_srli_epi64(P, 8);
		flip5 = maskO & _mm_slli_epi64(P, 7);
		flip6 = maskO & _mm_srli_epi64(P, 7);
		flip7 = maskO & _mm_slli_epi64(P, 9);
		flip8 = maskO & _mm_srli_epi64(P, 9);

		// 8 x SHIFT, 8 x AND, 8 x OR
		flip1 |= maskO & _mm_slli_epi64(flip1, 1);
		flip2 |= maskO & _mm_srli_epi64(flip2, 1);
		flip3 |=     O & _mm_slli_epi64(flip3, 8);
		flip4 |=     O & _mm_srli_epi64(flip4, 8);
		flip5 |= maskO & _mm_slli_epi64(flip5, 7);
		flip6 |= maskO & _mm_srli_epi64(flip6, 7);
		flip7 |= maskO & _mm_slli_epi64(flip7, 9);
		flip8 |= maskO & _mm_srli_epi64(flip8, 9);

		// 8 x SHIFT, 8 x AND
		mask1 = maskO & _mm_slli_epi64(maskO, 1);
		mask2 = maskO & _mm_srli_epi64(maskO, 1);
		mask3 =     O & _mm_slli_epi64(    O, 8);
		mask4 =     O & _mm_srli_epi64(    O, 8);
		mask5 = maskO & _mm_slli_epi64(maskO, 7);
		mask6 = maskO & _mm_srli_epi64(maskO, 7);
		mask7 = maskO & _mm_slli_epi64(maskO, 9);
		mask8 = maskO & _mm_srli_epi64(maskO, 9);

		// 8 x SHIFT, 8 x AND, 8 x OR
		flip1 |= mask1 & _mm_slli_epi64(flip1,  2);
		flip2 |= mask2 & _mm_srli_epi64(flip2,  2);
		flip3 |= mask3 & _mm_slli_epi64(flip3, 16);
		flip4 |= mask4 & _mm_srli_epi64(flip4, 16);
		flip5 |= mask5 & _mm_slli_epi64(flip5, 14);
		flip6 |= mask6 & _mm_srli_epi64(flip6, 14);
		flip7 |= mask7 & _mm_slli_epi64(flip7, 18);
		flip8 |= mask8 & _mm_srli_epi64(flip8, 18);

		// 8 x SHIFT, 8 x AND, 8 x OR
		flip1 |= mask1 & _mm_slli_epi64(flip1,  2);
		flip2 |= mask2 & _mm_srli_epi64(flip2,  2);
		flip3 |= mask3 & _mm_slli_epi64(flip3, 16);
		flip4 |= mask4 & _mm_srli_epi64(flip4, 16);
		flip5 |= mask5 & _mm_slli_epi64(flip5, 14);
		flip6 |= mask6 & _mm_srli_epi64(flip6, 14);
		flip7 |= mask7 & _mm_slli_epi64(flip7, 18);
		flip8 |= mask8 & _mm_srli_epi64(flip8, 18);

		// 8 x SHIFT, 1 x AND, 8 x OR, 1 x NOT
		return ~(P | O) & (
			_mm_slli_epi64(flip1, 1) | _mm_srli_epi64(flip2, 1) |
			_mm_slli_epi64(flip3, 8) | _mm_srli_epi64(flip4, 8) |
			_mm_slli_epi64(flip5, 7) | _mm_srli_epi64(flip6, 7) |
			_mm_slli_epi64(flip7, 9) | _mm_srli_epi64(flip8, 9)
			);
	}
#endif
