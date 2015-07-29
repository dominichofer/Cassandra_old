#include "possiblemoves.h"

// #########################
// ## If there is AVX-512 ##
// #########################
#if defined(HAS_AVX152)
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
		// 6 x SHIFT, 7 x AND, 5 x OR, 1 x NOT
		// = 19 OPs
		
		// 1 x AND
		const __m512i PP = _mm512_set1_epi64x(P);
		const __m512i maskO = _mm512_set1_epi64x(O) & _mm512_set_epi64x(0x7E7E7E7E7E7E7E7EULL, 0x00FFFFFFFFFFFF00ULL, 0x007E7E7E7E7E7E00ULL, 0x007E7E7E7E7E7E00ULL, 0x7E7E7E7E7E7E7E7EULL, 0x00FFFFFFFFFFFF00ULL, 0x007E7E7E7E7E7E00ULL, 0x007E7E7E7E7E7E00ULL);
		const __m512i shift1 = _mm512_set_epi64x(1, 8, 7, 9, -1, -8, -7, -9);
		const __m512i shift2 = shift1 + shift1;
		__m512i mask;
		__m512i flip;

		// 6 x SHIFT, 5 x AND, 3 x OR
		flip  = maskO & _mm512_rolv_epi64(PP, shift1);
		flip |= maskO & _mm512_rolv_epi64(flip, shift1);
		mask  = maskO & _mm512_rolv_epi64(maskO, shift1);
		flip |= mask  & _mm512_rolv_epi64(flip, shift2);
		flip |= mask  & _mm512_rolv_epi64(flip, shift2);
		flip  =         _mm512_rolv_epi64(flip, shift1);
				
		// 1 x NOT, 2 x OR, 1 x AND
		return ~(P | O) & _mm512_reduce_or_epi64(flip);
	}

	
	// AVX-512 specific code
	__m512i PossibleMoves(const __m512i P, const __m512i O)
	{
		// 48 x SHIFT, 42 x AND, 32 x OR, 1 x NOT
		// = 123 OPs
		__m512i mask1, mask2, mask3, mask4, mask5, mask6, mask7, mask8;
		__m512i flip1, flip2, flip3, flip4, flip5, flip6, flip7, flip8;

		// 1 x AND
		const __m512i maskO = O & _mm512_set1_epi64x(0x7E7E7E7E7E7E7E7EULL);

		// 8 x SHIFT, 8 x AND
		flip1 = maskO & _mm512_slli_epi64(P, 1);
		flip2 = maskO & _mm512_srli_epi64(P, 1);
		flip3 =     O & _mm512_slli_epi64(P, 8);
		flip4 =     O & _mm512_srli_epi64(P, 8);
		flip5 = maskO & _mm512_slli_epi64(P, 7);
		flip6 = maskO & _mm512_srli_epi64(P, 7);
		flip7 = maskO & _mm512_slli_epi64(P, 9);
		flip8 = maskO & _mm512_srli_epi64(P, 9);

		// 8 x SHIFT, 8 x AND, 8 x OR
		flip1 |= maskO & _mm512_slli_epi64(flip1, 1);
		flip2 |= maskO & _mm512_srli_epi64(flip2, 1);
		flip3 |=     O & _mm512_slli_epi64(flip3, 8);
		flip4 |=     O & _mm512_srli_epi64(flip4, 8);
		flip5 |= maskO & _mm512_slli_epi64(flip5, 7);
		flip6 |= maskO & _mm512_srli_epi64(flip6, 7);
		flip7 |= maskO & _mm512_slli_epi64(flip7, 9);
		flip8 |= maskO & _mm512_srli_epi64(flip8, 9);

		// 8 x SHIFT, 8 x AND
		mask1 = maskO & _mm512_slli_epi64(maskO, 1);
		mask2 = maskO & _mm512_srli_epi64(maskO, 1);
		mask3 =     O & _mm512_slli_epi64(    O, 8);
		mask4 =     O & _mm512_srli_epi64(    O, 8);
		mask5 = maskO & _mm512_slli_epi64(maskO, 7);
		mask6 = maskO & _mm512_srli_epi64(maskO, 7);
		mask7 = maskO & _mm512_slli_epi64(maskO, 9);
		mask8 = maskO & _mm512_srli_epi64(maskO, 9);

		// 8 x SHIFT, 8 x AND, 8 x OR
		flip1 |= mask1 & _mm512_slli_epi64(flip1,  2);
		flip2 |= mask2 & _mm512_srli_epi64(flip2,  2);
		flip3 |= mask3 & _mm512_slli_epi64(flip3, 16);
		flip4 |= mask4 & _mm512_srli_epi64(flip4, 16);
		flip5 |= mask5 & _mm512_slli_epi64(flip5, 14);
		flip6 |= mask6 & _mm512_srli_epi64(flip6, 14);
		flip7 |= mask7 & _mm512_slli_epi64(flip7, 18);
		flip8 |= mask8 & _mm512_srli_epi64(flip8, 18);

		// 8 x SHIFT, 8 x AND, 8 x OR
		flip1 |= mask1 & _mm512_slli_epi64(flip1,  2);
		flip2 |= mask2 & _mm512_srli_epi64(flip2,  2);
		flip3 |= mask3 & _mm512_slli_epi64(flip3, 16);
		flip4 |= mask4 & _mm512_srli_epi64(flip4, 16);
		flip5 |= mask5 & _mm512_slli_epi64(flip5, 14);
		flip6 |= mask6 & _mm512_srli_epi64(flip6, 14);
		flip7 |= mask7 & _mm512_slli_epi64(flip7, 18);
		flip8 |= mask8 & _mm512_srli_epi64(flip8, 18);

		// 8 x SHIFT, 1 x AND, 8 x OR, 1 x NOT
		return ~(P | O) & (
			_mm512_slli_epi64(flip1, 1) | _mm512_srli_epi64(flip2, 1) |
			_mm512_slli_epi64(flip3, 8) | _mm512_srli_epi64(flip4, 8) |
			_mm512_slli_epi64(flip5, 7) | _mm512_srli_epi64(flip6, 7) |
			_mm512_slli_epi64(flip7, 9) | _mm512_srli_epi64(flip8, 9)
			);
	}
#endif
