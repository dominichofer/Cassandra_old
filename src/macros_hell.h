#pragma once
#include <cstdint>

#if defined(_MSC_VER)
	#include <intrin.h>
#elif defined(__GNUC__)
	#include <x86intrin.h>
#else
	#error compiler not supported
#endif

// Predefined macros hell:
//
// __GNUC__           Compiler is gcc, Clang or Intel on Linux
// __INTEL_COMPILER   Compiler is Intel
// _MSC_VER           Compiler is MSVC or Intel on Windows
// _WIN32             Building on Windows (any)
// _WIN64             Building on Windows 64 bit

// #####################################
// ##    CPU specific optimizations   ##
// #####################################
//#define __core2__
//#define __corei7__
//#define __corei7_avx__
//#define __core_avx_i__
//#define __core_avx2__
//#define __bdver1__

//#define HAS_MMX
//#define HAS_SSE
//#define HAS_SSE2
//#define HAS_SSE3
//#define HAS_SSSE3
//#define HAS_SSE4_1
//#define HAS_SSE4_2
//#define HAS_SSE4a
//#define HAS_AVX
//#define HAS_AVX2
//#define HAS_BMI1
//#define HAS_BMI2
//#define HAS_ABM
//#define HAS_TBM
//#define HAS_POPCNT
//#define HAS_LZCNT

#define CACHE_LINE_SIZE 64
// #####################################

// CPUs instruction sets
#ifdef __core2__
	#define HAS_MMX
	#define HAS_SSE
	#define HAS_SSE2
	#define HAS_SSE3
	#define HAS_SSSE3
	#define HAS_SSE4_1
#endif
#ifdef __corei7__
	#define HAS_MMX
	#define HAS_SSE
	#define HAS_SSE2
	#define HAS_SSE3
	#define HAS_SSSE3
	#define HAS_SSE4_1
	#define HAS_SSE4_2
#endif
#if defined(__corei7_avx__) || defined(__core_avx_i__)
	#define HAS_MMX
	#define HAS_SSE
	#define HAS_SSE2
	#define HAS_SSE3
	#define HAS_SSSE3
	#define HAS_SSE4_1
	#define HAS_SSE4_2
#endif
#ifdef __core_avx2__
	#define HAS_MMX
	#define HAS_SSE
	#define HAS_SSE2
	#define HAS_SSE3
	#define HAS_SSSE3
	#define HAS_SSE4_1
	#define HAS_SSE4_2
	#define HAS_AVX
	#define HAS_AVX2
	#define HAS_BMI1
	#define HAS_BMI2
	#define HAS_POPCNT
	#define HAS_LZCNT
#endif
#ifdef __bdver1__
	#define HAS_MMX
	#define HAS_SSE
	#define HAS_SSE2
	#define HAS_SSE3
	#define HAS_SSSE3
	#define HAS_SSE4_1
	#define HAS_SSE4_2
	#define HAS_SSE4a
	#define HAS_AVX
	#define HAS_BMI1
	#define HAS_ABM
	#define HAS_TBM
#endif

// CPU instruction implications
#ifdef HAS_BMI1
	#define HAS_BEXTR	// Bit Field Extract
	#define HAS_BLSI	// Extract Lowest Set Isolated Bit  (x & -x)
	#define HAS_BLSMASK	// Get mask up to lowest set bit    (x ^ (x - 1))
	#define HAS_BLSR	// Reset lowest set bit             (x & (x - 1))
	#define HAS_LZCNT	// Leading Zero Count
	#define HAS_TZCNT	// Trailing Zero Count
#endif
#ifdef HAS_BMI2
	#define HAS_BZHI // Zero high bits starting with specified bit position
	#define HAS_PDEP // Parallel bits deposit
	#define HAS_PEXT // Parallel bits extract
#endif
#ifdef HAS_ABM
	#define HAS_POPCNT // Population count
	#define HAS_LZCNT  // Leading Zero Count
#endif
#ifdef HAS_TBM
	#define HAS_BEXTR	// Bit Field Extract
	#define HAS_BLCFILL	// Fill from lowest clear bit               ( x &  (x + 1))
	#define HAS_BLCI	// Isolate lowest clear bit                 ( x | ~(x + 1))
	#define HAS_BLCIC	// Isolate lowest clear bit and complement  (~x &  (x + 1))
	#define HAS_BLCMASK	// Mask from lowest clear bit               ( x ^  (x + 1))
	#define HAS_BLCS	// Set lowest clear bit                     ( x |  (x + 1))
	#define HAS_BLSFILL	// Fill from lowest set bit                 ( x |  (x - 1))
	#define HAS_BLSIC	// Isolate lowest set bit and complement    (~x |  (x - 1))
	#define HAS_T1MSKC	// Inverse mask from trailing ones          (~x |  (x + 1))
	#define HAS_TZMSK	// Mask from trailing zeros                 (~x &  (x - 1))
#endif


// alignas work-around
#if (__GNUC__ == 4) && (__GNUC_MINOR__ <= 7)
	#define alignas(A) __attribute__((aligned(A)))
#endif

// CACHE_LINE_ALIGNMENT
#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
	#define CACHE_LINE_ALIGNMENT __declspec(align(CACHE_LINE_SIZE))
#else
	#define CACHE_LINE_ALIGNMENT __attribute__((aligned(CACHE_LINE_SIZE)))
#endif


// FORCE_INLINE
#if defined(_MSC_VER)
	#define FORCE_INLINE __forceinline
#elif defined(__GNUC__)
	#define FORCE_INLINE inline __attribute__((always_inline))
#else
	#define FORCE_INLINE inline
#endif


// BitScan
// OUT: mask == 0 results in undefined index
#if defined(_MSC_VER)
	#pragma intrinsic(_BitScanForward64)
	#pragma intrinsic(_BitScanForward)
	#pragma intrinsic(_BitScanReverse64)
	#pragma intrinsic(_BitScanReverse)

	FORCE_INLINE unsigned char BitScanLSB(unsigned long * index, const uint64_t mask) { return _BitScanForward64(index, mask); }
	FORCE_INLINE unsigned long BitScanLSB(const uint64_t mask){	unsigned long index; _BitScanForward64(&index, mask); return index; }
	template <typename T> FORCE_INLINE unsigned char bsf(unsigned long * index, const T mask) { return BitScanLSB(index, mask); }

	FORCE_INLINE unsigned char BitScanMSB(unsigned long * index, const uint64_t mask) { return _BitScanReverse64(index, mask); }
	FORCE_INLINE unsigned long BitScanMSB(const uint64_t mask){ unsigned long index; _BitScanReverse64(&index, mask); return index; }
	template <typename T> FORCE_INLINE unsigned char bsr(unsigned long * index, const T mask) { return BitScanMSB(index, mask); }
#elif defined(__GNUC__)
	FORCE_INLINE unsigned char BitScanLSB(unsigned long * index, const uint64_t mask) { *index = __builtin_ctzll(mask); return mask; }
	FORCE_INLINE unsigned long BitScanLSB(const uint64_t mask){ return __builtin_ctzll(mask); }
	template <typename T> FORCE_INLINE unsigned char bsf(unsigned long * index, const T mask) { return BitScanLSB(index, mask); }

	FORCE_INLINE unsigned char BitScanMSB(unsigned long * index, const uint64_t mask) { *index = __builtin_clzll(mask) ^ 63; return mask; }
	FORCE_INLINE unsigned long BitScanMSB(const uint64_t mask){ return __builtin_clzll(mask) ^ 63; }
	template <typename T> FORCE_INLINE unsigned char bsr(unsigned long * index, const T mask) { return BitScanMSB(index, mask); }
#endif


// Count Leading Zeros
#if defined(_MSC_VER)
	FORCE_INLINE uint64_t CountLeadingZeros(const uint64_t mask) { return _lzcnt_u64(mask); }
#elif defined(__GNUC__)
	FORCE_INLINE uint64_t CountLeadingZeros(const uint64_t mask) { return __builtin_clzll(mask); }
#endif
	template <typename T> FORCE_INLINE T clz(const T mask) { return CountLeadingZeros(mask); }

// Count Trailing Zeros
#if defined(_MSC_VER)
	FORCE_INLINE uint64_t CountTrailingZeros(const uint64_t mask) { return _tzcnt_u64(mask); }
#elif defined(__GNUC__)
	FORCE_INLINE uint64_t CountTrailingZeros(const uint64_t mask) { return __builtin_ctzll(mask); }
#endif
	template <typename T> FORCE_INLINE T ctz(const T mask) { return CountTrailingZeros(mask); }


#ifdef HAS_BLSI
	FORCE_INLINE uint64_t GetLSB(const uint64_t b) { return _blsi_u64(b); }
#else
	FORCE_INLINE uint64_t GetLSB(const uint64_t b) { return b & -b; }
#endif

FORCE_INLINE uint64_t GetMSB(const uint64_t b) { return 0x8000000000000000ULL >> CountLeadingZeros(b); }

#ifdef HAS_BLSR
	FORCE_INLINE void RemoveLSB(uint64_t & b) { b = _blsr_u64(b); }
#else
	FORCE_INLINE void RemoveLSB(uint64_t & b) { b &= b - 1; }
#endif

FORCE_INLINE void RemoveMSB(uint64_t & b) { b ^= GetMSB(b); }


// PopCount
#ifdef HAS_POPCNT
	#if defined(_MSC_VER) || defined(__INTEL_COMPILER)
		FORCE_INLINE uint64_t PopCount(const uint64_t b) { return _mm_popcnt_u64(b); }
		FORCE_INLINE uint64_t PopCount_max15(const uint64_t b) { return PopCount(b); }
	#else
		FORCE_INLINE uint64_t PopCount(uint64_t b){ return __builtin_popcountll(b); }
		FORCE_INLINE uint64_t PopCount_max15(uint64_t b) { return PopCount(b); }
	#endif
#else
	inline uint64_t PopCount(uint64_t b){
		b -=  (b >> 1) & 0x5555555555555555ULL;
		b  = ((b >> 2) & 0x3333333333333333ULL) + (b & 0x3333333333333333ULL);
		b  = ((b >> 4) + b) & 0x0F0F0F0F0F0F0F0FULL;
		return (b * 0x0101010101010101ULL) >> 56;
	}
	inline uint64_t PopCount_max15(uint64_t b){
		b -=  (b >> 1) & 0x5555555555555555ULL;
		b  = ((b >> 2) & 0x3333333333333333ULL) + (b & 0x3333333333333333ULL);
		return (b * 0x1111111111111111ULL) >> 60;
	}
#endif


// BExtr
#if defined(HAS_BEXTR) || defined(HAS_TBM)
	FORCE_INLINE uint64_t BExtr(const uint64_t src, const unsigned int start, unsigned int len) { return _bextr_u64(src, start, len); }
#else
	FORCE_INLINE uint64_t BExtr(const uint64_t src, const unsigned int start, unsigned int len) { return (src >> start) & ((1ULL << len)-1); }
#endif


// BZHI
#ifdef HAS_BZHI
	FORCE_INLINE uint64_t BZHI(const uint64_t src, const uint32_t index) { return _bzhi_u64(src, index); }
#else
	FORCE_INLINE uint64_t BZHI(const uint64_t src, const uint32_t index) { return src & ((1ULL << index) - 1); }
#endif

#ifdef HAS_BLCFILL
#else
#endif

#ifdef HAS_BLCI
#else
#endif

#ifdef HAS_BLCIC
#else
#endif

#ifdef HAS_BLCMASK
#else
#endif

#ifdef HAS_BLCS
#else
#endif

#ifdef HAS_BLSFILL
#else
#endif

#ifdef HAS_BLSIC
#else
#endif

#ifdef HAS_T1MSKC
#else
#endif

#ifdef HAS_TZMSK
#else
#endif


// PDep
#ifdef HAS_PDEP
	FORCE_INLINE uint64_t PDep(const uint64_t src, const uint64_t mask) { return _pdep_u64(src, mask); }
#else
	inline uint64_t PDep(uint64_t src, uint64_t mask){
		uint64_t res = 0;
		for (uint64_t bb = 1; mask; bb += bb)
		{
			if (src & bb)
				res |= GetLSB(mask);
			RemoveLSB(mask);
		}
		return res;
	}
#endif


// PExt
#ifdef HAS_PEXT
	FORCE_INLINE uint64_t PExt(const uint64_t src, const uint64_t mask) { return _pext_u64(src, mask); }
#else
	inline uint64_t PExt(uint64_t src, uint64_t mask){
		uint64_t res = 0;
		for (uint64_t bb = 1; mask; bb += bb)
		{
			if (src & GetLSB(mask))
				res |= bb;
			RemoveLSB(mask);
		}
		return res;
	}
#endif

// BSWAP
#if defined(_MSC_VER)
	FORCE_INLINE uint64_t BSwap(const uint64_t b) { return _byteswap_uint64(b); }
#elif defined(__GNUC__)
	FORCE_INLINE uint64_t BSwap(const uint64_t b) { return __builtin_bswap64(b); }
#endif


#if defined(_MSC_VER)
    #ifdef HAS_SSE2
        FORCE_INLINE __m128i operator~(const __m128i& a) { return _mm_andnot_si128(a, _mm_set1_epi64x(0xFFFFFFFFFFFFFFFFULL)); }

        FORCE_INLINE __m128i operator+(const __m128i& a, const __m128i& b) { return _mm_add_epi64(a, b); }
        FORCE_INLINE __m128i operator-(const __m128i& a, const __m128i& b) { return _mm_sub_epi64(a, b); }
        FORCE_INLINE __m128i operator&(const __m128i& a, const __m128i& b) { return _mm_and_si128(a, b); }
        FORCE_INLINE __m128i operator|(const __m128i& a, const __m128i& b) { return _mm_or_si128(a, b); }
        FORCE_INLINE __m128i operator^(const __m128i& a, const __m128i& b) { return _mm_xor_si128(a, b); }
        FORCE_INLINE __m128i operator<<(const __m128i& a, const int b) { return _mm_slli_epi64(a, b); }
        FORCE_INLINE __m128i operator>>(const __m128i& a, const int b) { return _mm_srli_epi64(a, b); }

        FORCE_INLINE __m128i operator+=(__m128i& a, const __m128i& b) { return a = a + b; }
        FORCE_INLINE __m128i operator-=(__m128i& a, const __m128i& b) { return a = a - b; }
        FORCE_INLINE __m128i operator&=(__m128i& a, const __m128i& b) { return a = a & b; }
        FORCE_INLINE __m128i operator|=(__m128i& a, const __m128i& b) { return a = a | b; }
        FORCE_INLINE __m128i operator^=(__m128i& a, const __m128i& b) { return a = a ^ b; }
        FORCE_INLINE __m128i operator<<=(__m128i& a, const int b) { return a = a << b; }
        FORCE_INLINE __m128i operator>>=(__m128i& a, const int b) { return a = a >> b; }
    #endif
    #ifdef HAS_SSE4_1
        FORCE_INLINE __m128i operator==(const __m128i& a, const __m128i& b) { return _mm_cmpeq_epi64(a, b); }
    #endif
    #ifdef HAS_SSE4_2
        FORCE_INLINE __m128i operator>(const __m128i& a, const __m128i& b) { return _mm_cmpgt_epi64(a, b); }
        FORCE_INLINE __m128i operator<(const __m128i& a, const __m128i& b) { return b > a; }
        FORCE_INLINE __m128i operator>=(const __m128i& a, const __m128i& b) { return ~(a < b); }
        FORCE_INLINE __m128i operator<=(const __m128i& a, const __m128i& b) { return ~(a > b); }
    #endif
    #ifdef HAS_AVX2
		FORCE_INLINE __m256i operator~(const __m256i& a) { return _mm256_xor_si256(a, _mm256_set1_epi64x(0xFFFFFFFFFFFFFFFFULL)); }

        FORCE_INLINE __m256i operator+(const __m256i& a, const __m256i& b) { return _mm256_add_epi64(a, b); }
        FORCE_INLINE __m256i operator-(const __m256i& a, const __m256i& b) { return _mm256_sub_epi64(a, b); }
        FORCE_INLINE __m256i operator&(const __m256i& a, const __m256i& b) { return _mm256_and_si256(a, b); }
        FORCE_INLINE __m256i operator|(const __m256i& a, const __m256i& b) { return _mm256_or_si256(a, b); }
        FORCE_INLINE __m256i operator^(const __m256i& a, const __m256i& b) { return _mm256_xor_si256(a, b); }
        FORCE_INLINE __m256i operator<<(const __m256i& a, const int b) { return _mm256_slli_epi64(a, b); }
        FORCE_INLINE __m256i operator>>(const __m256i& a, const int b) { return _mm256_srli_epi64(a, b); }

        FORCE_INLINE __m256i operator+=(__m256i& a, const __m256i& b) { return a = a + b; }
        FORCE_INLINE __m256i operator-=(__m256i& a, const __m256i& b) { return a = a - b; }
        FORCE_INLINE __m256i operator&=(__m256i& a, const __m256i& b) { return a = a & b; }
        FORCE_INLINE __m256i operator|=(__m256i& a, const __m256i& b) { return a = a | b; }
        FORCE_INLINE __m256i operator^=(__m256i& a, const __m256i& b) { return a = a ^ b; }
        FORCE_INLINE __m256i operator<<=(__m256i& a, const int b) { return a = a << b; }
        FORCE_INLINE __m256i operator>>=(__m256i& a, const int b) { return a = a >> b; }
    #endif
#endif

// ### Fix for Visual Studio bug ###
#if defined(HAS_AVX2) && defined(_MSC_VER)
inline uint64_t _mm256_extract_epi64(__m256i X, const unsigned int N)
{
	const __m128i Y = _mm256_extracti128_si256(X, N >> 1);
	if (N & 1)
		return _mm_extract_epi64(Y, 1);
	else
		return _mm_extract_epi64(Y, 0);
}
#endif
