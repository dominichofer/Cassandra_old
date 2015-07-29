#pragma once
#include <stdint.h>
#include "macros_hell.h"

uint64_t HasMoves(const uint64_t P, const uint64_t O);
uint64_t PossibleMoves(const uint64_t P, const uint64_t O);

// SSE2 specific code
#ifdef HAS_SSE2
	__m128i PossibleMoves(const __m128i P, const __m128i O);
	inline __m128i PossibleMoves(const uint64_t P1, const uint64_t O1,
	                             const uint64_t P2, const uint64_t O2)
	{ 
		return PossibleMoves(_mm_set_epi64x(P1, P2), _mm_set_epi64x(O1, O2));
	}
#endif

// AVX2 specific code
#ifdef HAS_AVX2
	__m256i PossibleMoves(const __m256i P, const __m256i O);
	inline __m256i PossibleMoves(const uint64_t P1, const uint64_t O1,
								 const uint64_t P2, const uint64_t O2,
								 const uint64_t P3, const uint64_t O3,
								 const uint64_t P4, const uint64_t O4)
	{
		return PossibleMoves(_mm256_set_epi64x(P1, P2, P3, P4), _mm256_set_epi64x(O1, O2, O3, O4));
	}
#endif

// AVX-512 specific code
#ifdef HAS_AVX512
	__m512i PossibleMoves(const __m512i P, const __m512i O);
	inline __m512i PossibleMoves(const uint64_t P1, const uint64_t O1,
								 const uint64_t P2, const uint64_t O2,
								 const uint64_t P3, const uint64_t O3,
								 const uint64_t P4, const uint64_t O4,
								 const uint64_t P5, const uint64_t O5,
								 const uint64_t P6, const uint64_t O6,
								 const uint64_t P7, const uint64_t O7,
								 const uint64_t P8, const uint64_t O8)
	{
		return PossibleMoves(_mm512_set_epi64x(P1, P2, P3, P4, P5, P6, P7, P8), _mm512_set_epi64x(O1, O2, O3, O4, O5, O6, O7, O8));
	}
#endif

	
// Two PossibleMoves at once
inline void PossibleMoves(const uint64_t P1, const uint64_t O1, uint64_t & Possible1, 
                          const uint64_t P2, const uint64_t O2, uint64_t & Possible2)
{
	#ifdef HAS_SSE2
		const __m128i Possible = PossibleMoves(P1, O1, P2, O2);
		Possible1 = _mm_extract_epi64(Possible, 1);
		Possible2 = _mm_extract_epi64(Possible, 0);
	#else
		Possible1 = PossibleMoves(P1, O1);
		Possible2 = PossibleMoves(P2, O2);
	#endif
}

// Four PossibleMoves at once
inline void PossibleMoves(const uint64_t P1, const uint64_t O1, uint64_t & Possible1, 
                          const uint64_t P2, const uint64_t O2, uint64_t & Possible2, 
                          const uint64_t P3, const uint64_t O3, uint64_t & Possible3, 
                          const uint64_t P4, const uint64_t O4, uint64_t & Possible4)
{
	#if defined(HAS_AVX2)
		const __m256i Possible = PossibleMoves(P1, O1, P2, O2, P3, O3, P4, O4);
		Possible1 = _mm256_extract_epi64(Possible, 3);
		Possible2 = _mm256_extract_epi64(Possible, 2);
		Possible3 = _mm256_extract_epi64(Possible, 1);
		Possible4 = _mm256_extract_epi64(Possible, 0);
	#elif defined(HAS_SSE2)
		__m128i Possible = PossibleMoves(P1, O1, P2, O2);
		Possible1 = _mm_extract_epi64(Possible, 1);
		Possible2 = _mm_extract_epi64(Possible, 0);
		Possible = PossibleMoves(P3, O3, P4, O4);
		Possible3 = _mm_extract_epi64(Possible, 1);
		Possible4 = _mm_extract_epi64(Possible, 0);
	#else
		Possible1 = PossibleMoves(P1, O1);
		Possible2 = PossibleMoves(P2, O2);
		Possible3 = PossibleMoves(P3, O3);
		Possible4 = PossibleMoves(P4, O4);
	#endif
}