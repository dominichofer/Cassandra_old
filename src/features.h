#pragma once
#include <cstdint>
#include "datamanipulation.h"
#include "position.h"
#include "utility.h"
#include "configfile.h"
#include <functional>
#include <iostream>
#include <vector>
#include <map>

static const bool PATTERN_L02X  = true;
static const bool PATTERN_L1    = true;
static const bool PATTERN_L2    = true;
static const bool PATTERN_L3    = true;
static const bool PATTERN_D4    = true;
static const bool PATTERN_D5    = true;
static const bool PATTERN_D6    = true;
static const bool PATTERN_D7    = true;
static const bool PATTERN_Comet = true;
static const bool PATTERN_Eplus = true;
static const bool PATTERN_C3p1  = false;
static const bool PATTERN_C3p2  = true;
static const bool PATTERN_Q0    = true;
static const bool PATTERN_B5    = true;

namespace Features
{

	const int NumberOfPatternWithSymmetrie = 0
		+ (PATTERN_L02X ? 4 : 0)
		+ (PATTERN_L1   ? 4 : 0)
		+ (PATTERN_L2   ? 4 : 0)
		+ (PATTERN_L3   ? 4 : 0)
		+ (PATTERN_D4   ? 4 : 0)
		+ (PATTERN_D5   ? 4 : 0)
		+ (PATTERN_D6   ? 4 : 0)
		+ (PATTERN_D7   ? 4 : 0)
		+ (PATTERN_Comet? 4 : 0)
		+ (PATTERN_Eplus? 4 : 0)
		+ (PATTERN_C3p1 ? 4 : 0)
		+ (PATTERN_C3p2 ? 4 : 0)
		+ (PATTERN_Q0   ? 4 : 0)
		+ (PATTERN_B5   ? 8 : 0);

	class CIndexArray
	{
	public:
		uint64_t BitBoard;
		std::vector<int> Indices;
		CIndexArray(const uint64_t BitBoard) : BitBoard(BitBoard), Indices(std::vector<int>(NumberOfPatternWithSymmetrie)) {}
		CIndexArray(const CIndexArray& o) : BitBoard(BitBoard), Indices(o.Indices) {}
	};

	// Template metaprogramming hack
	template <uint64_t b>
	class Flip
	{
		private: static const uint64_t          c = ((b >> 1) & 0x5555555555555555ULL) | ((b << 1) & 0xAAAAAAAAAAAAAAAAULL);
		private: static const uint64_t          d = ((c >> 2) & 0x3333333333333333ULL) | ((c << 2) & 0xCCCCCCCCCCCCCCCCULL);
		public:  static const uint64_t horizontal = ((d >> 4) & 0x0F0F0F0F0F0F0F0FULL) | ((d << 4) & 0xF0F0F0F0F0F0F0F0ULL);

		private: static const uint64_t          e = ((b >>  8) & 0x00FF00FF00FF00FFULL) | ((b <<  8) & 0xFF00FF00FF00FF00ULL);
		private: static const uint64_t          f = ((e >> 16) & 0x0000FFFF0000FFFFULL) | ((e << 16) & 0xFFFF0000FFFF0000ULL);
		public:	 static const uint64_t vertical   = ((f >> 32) & 0x00000000FFFFFFFFULL) | ((f << 32) & 0xFFFFFFFF00000000ULL);

		private: static const uint64_t          g =    (b ^ (b >>  7)) & 0x00AA00AA00AA00AAULL;
		private: static const uint64_t          h = b ^ g ^ (g <<  7);
		private: static const uint64_t          i =    (h ^ (h >> 14)) & 0x0000CCCC0000CCCCULL;
		private: static const uint64_t          j = h ^ i ^ (i << 14);
		private: static const uint64_t          k =    (j ^ (j >> 28)) & 0x00000000F0F0F0F0ULL;
		public:  static const uint64_t diagonal   = j ^ k ^ (k << 28);

		private: static const uint64_t          l =      b ^ (b << 36);
		private: static const uint64_t          m = b ^ (l ^ (b >> 36)) & 0xF0F0F0F00F0F0F0FULL;
		private: static const uint64_t          n =     (m ^ (m << 18)) & 0xCCCC0000CCCC0000ULL;
		private: static const uint64_t          o = m ^  n ^ (n >> 18);
		private: static const uint64_t          p =     (o ^ (o <<  9)) & 0xAA00AA00AA00AA00ULL;
		public:  static const uint64_t codiagonal = o ^  p ^ (p >>  9);
	};

	const int POW_2_0 = 1;
	const int POW_2_1 = 2 * POW_2_0;
	const int POW_2_2 = 2 * POW_2_1;
	const int POW_2_3 = 2 * POW_2_2;
	const int POW_2_4 = 2 * POW_2_3;
	const int POW_2_5 = 2 * POW_2_4;
	const int POW_2_6 = 2 * POW_2_5;
	const int POW_2_7 = 2 * POW_2_6;
	const int POW_2_8 = 2 * POW_2_7;
	const int POW_2_9 = 2 * POW_2_8;
	const int POW_2_10 = 2 * POW_2_9;

	const int POW_3_0 = 1;
	const int POW_3_1 = 3 * POW_3_0;
	const int POW_3_2 = 3 * POW_3_1;
	const int POW_3_3 = 3 * POW_3_2;
	const int POW_3_4 = 3 * POW_3_3;
	const int POW_3_5 = 3 * POW_3_4;
	const int POW_3_6 = 3 * POW_3_5;
	const int POW_3_7 = 3 * POW_3_6;
	const int POW_3_8 = 3 * POW_3_7;
	const int POW_3_9 = 3 * POW_3_8;
	const int POW_3_10 = 3 * POW_3_9;

	extern unsigned int SumPow3[1024];

	inline unsigned int FullPatternIndex(const uint64_t P, const uint64_t O, const uint64_t mask) { return SumPow3[PExt(P, mask)] + (SumPow3[PExt(O, mask)] << 1); }

	void For_each_configuration_in_pattern_do_fkt(const uint64_t Pattern, std::function<void(const uint64_t, const uint64_t)> fkt);
	void Initialize();
	void Finalize();


	class CPattern_L02X
	{
		static const int halfSize = POW_3_5;
		static const uint64_t Pattern = 0x00000000000042FFULL;
		static const uint64_t PatternH = Flip<Pattern>::horizontal;
		static const uint64_t PatternV = Flip<Pattern>::vertical;
		static const uint64_t PatternD = Flip<Pattern>::diagonal;
		static const uint64_t PatternC = Flip<Pattern>::codiagonal;
		static const uint64_t PatternHV = Flip<Flip<Pattern>::horizontal>::vertical;
		static const uint64_t PatternHD = Flip<Flip<Pattern>::horizontal>::diagonal;
		static const uint64_t PatternHC = Flip<Flip<Pattern>::horizontal>::codiagonal;

		std::vector<std::vector<float>> m_weights; //m_weights[Index][FullIndex]

		static int ReducedPatternIndex0(const uint64_t P, const uint64_t O)
		{
			const int indexA = FullPatternIndex(                P ,                 O , 0x000000000000020FULL);
			const int indexB = FullPatternIndex(FlipHorizontal(P), FlipHorizontal(O), 0x000000000000020FULL);
			return (indexA > indexB) ? (halfSize*indexB + indexA - (((indexB + 1)*indexB) >> 1)) : (halfSize*indexA + indexB - (((indexA + 1)*indexA) >> 1));
		}
		static inline int ReducedPatternIndex1(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		static inline int ReducedPatternIndex2(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }
		static inline int ReducedPatternIndex3(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipDiagonal(P), FlipDiagonal(O)); }

	public:
		static const int FullSize = POW_3_10;
		static const int ReducedSize = halfSize * (halfSize + 1) / 2;

		CPattern_L02X()
		{
			//Initialize weights
			m_weights.resize(4);
			for (int i = 0; i < 4; ++i)
				m_weights[i].resize(FullSize);
		}
		void set_weights(const std::vector<float>& compressed_weights)
		{
			int ReducedIndex;
			int FullIndex;

			For_each_configuration_in_pattern_do_fkt(Pattern,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex0(P, O);
					FullIndex = FullPatternIndex(P, O, Pattern);
					m_weights[0][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternC,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex1(P, O);
					FullIndex = FullPatternIndex(P, O, PatternC);
					m_weights[1][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternV,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex2(P, O);
					FullIndex = FullPatternIndex(P, O, PatternV);
					m_weights[2][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternD,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex3(P, O);
					FullIndex = FullPatternIndex(P, O, PatternD);
					m_weights[3][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[0][FullPatternIndex(P, O, Pattern )]
				 + m_weights[1][FullPatternIndex(P, O, PatternC)]
				 + m_weights[2][FullPatternIndex(P, O, PatternV)]
				 + m_weights[3][FullPatternIndex(P, O, PatternD)];
		}
		inline float score(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			uint64_t diff = (new_P ^ old_P) | (new_O ^ old_O);

			return m_weights[0][(diff & Pattern ) ? FullPatternIndex(new_P, new_O, Pattern ) : *(Indices_P+0) + (*(Indices_P+0) << 1)]
				 + m_weights[1][(diff & PatternC) ? FullPatternIndex(new_P, new_O, PatternC) : *(Indices_P+1) + (*(Indices_P+1) << 1)]
				 + m_weights[2][(diff & PatternV) ? FullPatternIndex(new_P, new_O, PatternV) : *(Indices_P+2) + (*(Indices_P+2) << 1)]
				 + m_weights[3][(diff & PatternD) ? FullPatternIndex(new_P, new_O, PatternD) : *(Indices_P+3) + (*(Indices_P+3) << 1)];
		}
		inline float score(const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			return m_weights[0][*(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[1][*(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[2][*(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[3][*(Indices_P+3) + (*(Indices_O+3) << 1)];
		}

		static void UpdateIndexVec(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, std::vector<int>::iterator Indices_P, std::vector<int>::iterator Indices_O)
		{
			uint64_t diff_P = new_P ^ old_P;
			uint64_t diff_O = new_O ^ old_O;
			if (diff_P & Pattern ) *(Indices_P+0) = SumPow3[PExt(new_P, Pattern )];
			if (diff_P & PatternC) *(Indices_P+1) = SumPow3[PExt(new_P, PatternC)];
			if (diff_P & PatternV) *(Indices_P+2) = SumPow3[PExt(new_P, PatternV)];
			if (diff_P & PatternD) *(Indices_P+3) = SumPow3[PExt(new_P, PatternD)];
			if (diff_O & Pattern ) *(Indices_O+0) = SumPow3[PExt(new_O, Pattern )];
			if (diff_O & PatternC) *(Indices_O+1) = SumPow3[PExt(new_O, PatternC)];
			if (diff_O & PatternV) *(Indices_O+2) = SumPow3[PExt(new_O, PatternV)];
			if (diff_O & PatternD) *(Indices_O+3) = SumPow3[PExt(new_O, PatternD)];
		}

		static void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec, int Offset = 0)
		{
			vec.push_back(ReducedPatternIndex0(P, O) + Offset);
			vec.push_back(ReducedPatternIndex1(P, O) + Offset);
			vec.push_back(ReducedPatternIndex2(P, O) + Offset);
			vec.push_back(ReducedPatternIndex3(P, O) + Offset);
		}
	};

	class CPattern_L1
	{
		static const int halfSize = POW_3_4;
		static const uint64_t Pattern = 0x000000000000FF00ULL;
		static const uint64_t PatternH = Flip<Pattern>::horizontal;
		static const uint64_t PatternV = Flip<Pattern>::vertical;
		static const uint64_t PatternD = Flip<Pattern>::diagonal;
		static const uint64_t PatternC = Flip<Pattern>::codiagonal;
		static const uint64_t PatternHV = Flip<Flip<Pattern>::horizontal>::vertical;
		static const uint64_t PatternHD = Flip<Flip<Pattern>::horizontal>::diagonal;
		static const uint64_t PatternHC = Flip<Flip<Pattern>::horizontal>::codiagonal;

		std::vector<float> m_weights; //m_weights[FullIndex]

		static int ReducedPatternIndex0(const uint64_t P, const uint64_t O)
		{
			const int indexA = FullPatternIndex(                P ,                 O , 0x0000000000000F00ULL);
			const int indexB = FullPatternIndex(FlipHorizontal(P), FlipHorizontal(O), 0x0000000000000F00ULL);
			return (indexA > indexB) ? (halfSize*indexB + indexA - (((indexB + 1)*indexB) >> 1)) : (halfSize*indexA + indexB - (((indexA + 1)*indexA) >> 1));
		}
		static inline int ReducedPatternIndex1(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		static inline int ReducedPatternIndex2(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }
		static inline int ReducedPatternIndex3(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipDiagonal(P), FlipDiagonal(O)); }

	public:
		static const int FullSize = POW_3_8;
		static const int ReducedSize = halfSize * (halfSize + 1) / 2;

		CPattern_L1() { m_weights.resize(FullSize); }
		void set_weights(const std::vector<float>& compressed_weights)
		{
			int ReducedIndex;
			int FullIndex;

			For_each_configuration_in_pattern_do_fkt(Pattern,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex0(P, O);
					FullIndex = FullPatternIndex(P, O, Pattern);
					m_weights[FullIndex] = compressed_weights[ReducedIndex];
				}
			);
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[FullPatternIndex(P, O, Pattern )]
				 + m_weights[FullPatternIndex(P, O, PatternC)]
				 + m_weights[FullPatternIndex(P, O, PatternV)]
				 + m_weights[FullPatternIndex(P, O, PatternD)];
		}
		inline float score(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			uint64_t diff = (new_P ^ old_P) | (new_O ^ old_O);

			return m_weights[(diff & Pattern ) ? FullPatternIndex(new_P, new_O, Pattern ) : *(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[(diff & PatternC) ? FullPatternIndex(new_P, new_O, PatternC) : *(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[(diff & PatternV) ? FullPatternIndex(new_P, new_O, PatternV) : *(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[(diff & PatternD) ? FullPatternIndex(new_P, new_O, PatternD) : *(Indices_P+3) + (*(Indices_O+3) << 1)];
		}
		inline float score(const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			return m_weights[*(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[*(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[*(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[*(Indices_P+3) + (*(Indices_O+3) << 1)];
		}

		static void UpdateIndexVec(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, std::vector<int>::iterator Indices_P, std::vector<int>::iterator Indices_O)
		{
			uint64_t diff_P = new_P ^ old_P;
			uint64_t diff_O = new_O ^ old_O;
			if (diff_P & Pattern ) *(Indices_P+0) = SumPow3[PExt(new_P, Pattern )];
			if (diff_P & PatternC) *(Indices_P+1) = SumPow3[PExt(new_P, PatternC)];
			if (diff_P & PatternV) *(Indices_P+2) = SumPow3[PExt(new_P, PatternV)];
			if (diff_P & PatternD) *(Indices_P+3) = SumPow3[PExt(new_P, PatternD)];
			if (diff_O & Pattern ) *(Indices_O+0) = SumPow3[PExt(new_O, Pattern )];
			if (diff_O & PatternC) *(Indices_O+1) = SumPow3[PExt(new_O, PatternC)];
			if (diff_O & PatternV) *(Indices_O+2) = SumPow3[PExt(new_O, PatternV)];
			if (diff_O & PatternD) *(Indices_O+3) = SumPow3[PExt(new_O, PatternD)];
		}


		static void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec, int Offset = 0)
		{
			vec.push_back(ReducedPatternIndex0(P, O) + Offset);
			vec.push_back(ReducedPatternIndex1(P, O) + Offset);
			vec.push_back(ReducedPatternIndex2(P, O) + Offset);
			vec.push_back(ReducedPatternIndex3(P, O) + Offset);
		}
	};

	class CPattern_L2
	{
		static const int halfSize = POW_3_4;
		static const uint64_t Pattern = 0x0000000000FF0000ULL;
		static const uint64_t PatternH = Flip<Pattern>::horizontal;
		static const uint64_t PatternV = Flip<Pattern>::vertical;
		static const uint64_t PatternD = Flip<Pattern>::diagonal;
		static const uint64_t PatternC = Flip<Pattern>::codiagonal;
		static const uint64_t PatternHV = Flip<Flip<Pattern>::horizontal>::vertical;
		static const uint64_t PatternHD = Flip<Flip<Pattern>::horizontal>::diagonal;
		static const uint64_t PatternHC = Flip<Flip<Pattern>::horizontal>::codiagonal;

		std::vector<float> m_weights; //m_weights[FullIndex]

		static int ReducedPatternIndex0(const uint64_t P, const uint64_t O)
		{
			const int indexA = FullPatternIndex(                P ,                 O , 0x00000000000F0000ULL);
			const int indexB = FullPatternIndex(FlipHorizontal(P), FlipHorizontal(O), 0x00000000000F0000ULL);
			return (indexA > indexB) ? (halfSize*indexB + indexA - (((indexB + 1)*indexB) >> 1)) : (halfSize*indexA + indexB - (((indexA + 1)*indexA) >> 1));
		}
		static inline int ReducedPatternIndex1(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		static inline int ReducedPatternIndex2(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }
		static inline int ReducedPatternIndex3(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipDiagonal(P), FlipDiagonal(O)); }

	public:
		static const int FullSize = POW_3_8;
		static const int ReducedSize = halfSize * (halfSize + 1) / 2;

		CPattern_L2() { m_weights.resize(FullSize); }
		void set_weights(const std::vector<float>& compressed_weights)
		{
			int ReducedIndex;
			int FullIndex;

			For_each_configuration_in_pattern_do_fkt(Pattern,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex0(P, O);
					FullIndex = FullPatternIndex(P, O, Pattern);
					m_weights[FullIndex] = compressed_weights[ReducedIndex];
				}
			);
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[FullPatternIndex(P, O, Pattern )]
				 + m_weights[FullPatternIndex(P, O, PatternC)]
				 + m_weights[FullPatternIndex(P, O, PatternV)]
				 + m_weights[FullPatternIndex(P, O, PatternD)];
		}
		inline float score(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			uint64_t diff = (new_P ^ old_P) | (new_O ^ old_O);

			return m_weights[(diff & Pattern ) ? FullPatternIndex(new_P, new_O, Pattern ) : *(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[(diff & PatternC) ? FullPatternIndex(new_P, new_O, PatternC) : *(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[(diff & PatternV) ? FullPatternIndex(new_P, new_O, PatternV) : *(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[(diff & PatternD) ? FullPatternIndex(new_P, new_O, PatternD) : *(Indices_P+3) + (*(Indices_O+3) << 1)];
		}
		inline float score(const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			return m_weights[*(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[*(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[*(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[*(Indices_P+3) + (*(Indices_O+3) << 1)];
		}

		static void UpdateIndexVec(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, std::vector<int>::iterator Indices_P, std::vector<int>::iterator Indices_O)
		{
			uint64_t diff_P = new_P ^ old_P;
			uint64_t diff_O = new_O ^ old_O;
			if (diff_P & Pattern ) *(Indices_P+0) = SumPow3[PExt(new_P, Pattern )];
			if (diff_P & PatternC) *(Indices_P+1) = SumPow3[PExt(new_P, PatternC)];
			if (diff_P & PatternV) *(Indices_P+2) = SumPow3[PExt(new_P, PatternV)];
			if (diff_P & PatternD) *(Indices_P+3) = SumPow3[PExt(new_P, PatternD)];
			if (diff_O & Pattern ) *(Indices_O+0) = SumPow3[PExt(new_O, Pattern )];
			if (diff_O & PatternC) *(Indices_O+1) = SumPow3[PExt(new_O, PatternC)];
			if (diff_O & PatternV) *(Indices_O+2) = SumPow3[PExt(new_O, PatternV)];
			if (diff_O & PatternD) *(Indices_O+3) = SumPow3[PExt(new_O, PatternD)];
		}


		static void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec, int Offset = 0)
		{
			vec.push_back(ReducedPatternIndex0(P, O) + Offset);
			vec.push_back(ReducedPatternIndex1(P, O) + Offset);
			vec.push_back(ReducedPatternIndex2(P, O) + Offset);
			vec.push_back(ReducedPatternIndex3(P, O) + Offset);
		}
	};

	class CPattern_L3
	{
		static const int halfSize = POW_3_3 * POW_2_1;
		static const uint64_t Pattern = 0x00000000FF000000ULL;
		static const uint64_t PatternH = Flip<Pattern>::horizontal;
		static const uint64_t PatternV = Flip<Pattern>::vertical;
		static const uint64_t PatternD = Flip<Pattern>::diagonal;
		static const uint64_t PatternC = Flip<Pattern>::codiagonal;
		static const uint64_t PatternHV = Flip<Flip<Pattern>::horizontal>::vertical;
		static const uint64_t PatternHD = Flip<Flip<Pattern>::horizontal>::diagonal;
		static const uint64_t PatternHC = Flip<Flip<Pattern>::horizontal>::codiagonal;

		std::vector<float> m_weights; //m_weights[FullIndex]

		static int ReducedPatternIndex0(const uint64_t P, const uint64_t O)
		{
			const int indexA = FullPatternIndex(                P ,                 O , 0x0000000007000000ULL) + ((                O  & 0x0000000008000000ULL) ? POW_3_3 : 0);
			const int indexB = FullPatternIndex(FlipHorizontal(P), FlipHorizontal(O), 0x0000000007000000ULL) + ((FlipHorizontal(O) & 0x0000000008000000ULL) ? POW_3_3 : 0);
			return (indexA > indexB) ? (halfSize*indexB + indexA - (((indexB + 1)*indexB) >> 1)) : (halfSize*indexA + indexB - (((indexA + 1)*indexA) >> 1));
		}
		static inline int ReducedPatternIndex1(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		static inline int ReducedPatternIndex2(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }
		static inline int ReducedPatternIndex3(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipDiagonal(P), FlipDiagonal(O)); }

	public:
		static const int FullSize = POW_3_8;
		static const int ReducedSize = halfSize * (halfSize + 1) / 2;

		CPattern_L3() { m_weights.resize(FullSize); }
		void set_weights(const std::vector<float>& compressed_weights)
		{
			int ReducedIndex;
			int FullIndex;

			For_each_configuration_in_pattern_do_fkt(Pattern,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex0(P, O);
					FullIndex = FullPatternIndex(P, O, Pattern);
					m_weights[FullIndex] = compressed_weights[ReducedIndex];
				}
			);
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[FullPatternIndex(P, O, Pattern )]
				 + m_weights[FullPatternIndex(P, O, PatternC)]
				 + m_weights[FullPatternIndex(P, O, PatternV)]
				 + m_weights[FullPatternIndex(P, O, PatternD)];
		}
		inline float score(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			uint64_t diff = (new_P ^ old_P) | (new_O ^ old_O);

			return m_weights[(diff & Pattern ) ? FullPatternIndex(new_P, new_O, Pattern ) : *(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[(diff & PatternC) ? FullPatternIndex(new_P, new_O, PatternC) : *(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[(diff & PatternV) ? FullPatternIndex(new_P, new_O, PatternV) : *(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[(diff & PatternD) ? FullPatternIndex(new_P, new_O, PatternD) : *(Indices_P+3) + (*(Indices_O+3) << 1)];
		}
		inline float score(const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			return m_weights[*(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[*(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[*(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[*(Indices_P+3) + (*(Indices_O+3) << 1)];
		}

		static void UpdateIndexVec(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, std::vector<int>::iterator Indices_P, std::vector<int>::iterator Indices_O)
		{
			uint64_t diff_P = new_P ^ old_P;
			uint64_t diff_O = new_O ^ old_O;
			if (diff_P & Pattern ) *(Indices_P+0) = SumPow3[PExt(new_P, Pattern )];
			if (diff_P & PatternC) *(Indices_P+1) = SumPow3[PExt(new_P, PatternC)];
			if (diff_P & PatternV) *(Indices_P+2) = SumPow3[PExt(new_P, PatternV)];
			if (diff_P & PatternD) *(Indices_P+3) = SumPow3[PExt(new_P, PatternD)];
			if (diff_O & Pattern ) *(Indices_O+0) = SumPow3[PExt(new_O, Pattern )];
			if (diff_O & PatternC) *(Indices_O+1) = SumPow3[PExt(new_O, PatternC)];
			if (diff_O & PatternV) *(Indices_O+2) = SumPow3[PExt(new_O, PatternV)];
			if (diff_O & PatternD) *(Indices_O+3) = SumPow3[PExt(new_O, PatternD)];
		}


		static void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec, int Offset = 0)
		{
			vec.push_back(ReducedPatternIndex0(P, O) + Offset);
			vec.push_back(ReducedPatternIndex1(P, O) + Offset);
			vec.push_back(ReducedPatternIndex2(P, O) + Offset);
			vec.push_back(ReducedPatternIndex3(P, O) + Offset);
		}
	};

	class CPattern_D4
	{
		static const int halfSize = POW_3_2;
		static const uint64_t Pattern = 0x0000000001020408ULL;
		static const uint64_t PatternH = Flip<Pattern>::horizontal;
		static const uint64_t PatternV = Flip<Pattern>::vertical;
		static const uint64_t PatternD = Flip<Pattern>::diagonal;
		static const uint64_t PatternC = Flip<Pattern>::codiagonal;
		static const uint64_t PatternHV = Flip<Flip<Pattern>::horizontal>::vertical;
		static const uint64_t PatternHD = Flip<Flip<Pattern>::horizontal>::diagonal;
		static const uint64_t PatternHC = Flip<Flip<Pattern>::horizontal>::codiagonal;

		std::vector<float> m_weights; //m_weights[FullIndex]

		static int ReducedPatternIndex0(const uint64_t P, const uint64_t O)
		{
			const int indexA = FullPatternIndex(              P ,               O , 0x0000000000000408ULL);
			const int indexB = FullPatternIndex(FlipDiagonal(P), FlipDiagonal(O), 0x0000000000000408ULL);
			return (indexA > indexB) ? (halfSize*indexB + indexA - (((indexB + 1)*indexB) >> 1)) : (halfSize*indexA + indexB - (((indexA + 1)*indexA) >> 1));
		}
		static inline int ReducedPatternIndex1(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipHorizontal(P), FlipHorizontal(O)); }
		static inline int ReducedPatternIndex2(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		static inline int ReducedPatternIndex3(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }
	public:
		static const int FullSize = POW_3_4;
		static const int ReducedSize = halfSize * (halfSize + 1) / 2;

		CPattern_D4() { m_weights.resize(FullSize); }
		void set_weights(const std::vector<float>& compressed_weights)
		{
			int ReducedIndex;
			int FullIndex;

			For_each_configuration_in_pattern_do_fkt(Pattern,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex0(P, O);
					FullIndex = FullPatternIndex(P, O, Pattern);
					m_weights[FullIndex] = compressed_weights[ReducedIndex];
				}
			);
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[FullPatternIndex(P, O, Pattern )]
				 + m_weights[FullPatternIndex(P, O, PatternH)]
				 + m_weights[FullPatternIndex(P, O, PatternC)]
				 + m_weights[FullPatternIndex(P, O, PatternV)];
		}
		inline float score(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			uint64_t diff = (new_P ^ old_P) | (new_O ^ old_O);

			return m_weights[(diff & Pattern ) ? FullPatternIndex(new_P, new_O, Pattern ) : *(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[(diff & PatternH) ? FullPatternIndex(new_P, new_O, PatternH) : *(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[(diff & PatternC) ? FullPatternIndex(new_P, new_O, PatternC) : *(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[(diff & PatternV) ? FullPatternIndex(new_P, new_O, PatternV) : *(Indices_P+3) + (*(Indices_O+3) << 1)];
		}
		inline float score(const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			return m_weights[*(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[*(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[*(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[*(Indices_P+3) + (*(Indices_O+3) << 1)];
		}

		static void UpdateIndexVec(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, std::vector<int>::iterator Indices_P, std::vector<int>::iterator Indices_O)
		{
			uint64_t diff_P = new_P ^ old_P;
			uint64_t diff_O = new_O ^ old_O;
			if (diff_P & Pattern ) *(Indices_P+0) = SumPow3[PExt(new_P, Pattern )];
			if (diff_P & PatternH) *(Indices_P+1) = SumPow3[PExt(new_P, PatternH)];
			if (diff_P & PatternC) *(Indices_P+2) = SumPow3[PExt(new_P, PatternC)];
			if (diff_P & PatternV) *(Indices_P+3) = SumPow3[PExt(new_P, PatternV)];
			if (diff_O & Pattern ) *(Indices_O+0) = SumPow3[PExt(new_O, Pattern )];
			if (diff_O & PatternH) *(Indices_O+1) = SumPow3[PExt(new_O, PatternH)];
			if (diff_O & PatternC) *(Indices_O+2) = SumPow3[PExt(new_O, PatternC)];
			if (diff_O & PatternV) *(Indices_O+3) = SumPow3[PExt(new_O, PatternV)];
		}

		static void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec, int Offset = 0)
		{
			vec.push_back(ReducedPatternIndex0(P, O) + Offset);
			vec.push_back(ReducedPatternIndex1(P, O) + Offset);
			vec.push_back(ReducedPatternIndex2(P, O) + Offset);
			vec.push_back(ReducedPatternIndex3(P, O) + Offset);
		}
	};

	class CPattern_D5
	{
		static const int halfSize = POW_3_2;
		static const int diagSize = POW_3_1;
		static const uint64_t Pattern = 0x0000000102040810ULL;
		static const uint64_t PatternH = Flip<Pattern>::horizontal;
		static const uint64_t PatternV = Flip<Pattern>::vertical;
		static const uint64_t PatternD = Flip<Pattern>::diagonal;
		static const uint64_t PatternC = Flip<Pattern>::codiagonal;
		static const uint64_t PatternHV = Flip<Flip<Pattern>::horizontal>::vertical;
		static const uint64_t PatternHD = Flip<Flip<Pattern>::horizontal>::diagonal;
		static const uint64_t PatternHC = Flip<Flip<Pattern>::horizontal>::codiagonal;

		std::vector<float> m_weights; //m_weights[FullIndex]

		static int ReducedPatternIndex0(const uint64_t P, const uint64_t O)
		{
			const int indexA = FullPatternIndex(              P ,               O , 0x0000000000000810ULL);
			const int indexB = FullPatternIndex(FlipDiagonal(P), FlipDiagonal(O), 0x0000000000000810ULL);
			const int indexD = FullPatternIndex(              P ,               O , 0x0000000000040000ULL);
			return (indexA > indexB) ? (diagSize*(halfSize*indexB+indexA - (((indexB+1)*indexB) >> 1)) + indexD) : (diagSize*(halfSize*indexA+indexB - (((indexA+1)*indexA) >> 1)) + indexD);
		}
		static inline int ReducedPatternIndex1(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipHorizontal(P), FlipHorizontal(O)); }
		static inline int ReducedPatternIndex2(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		static inline int ReducedPatternIndex3(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }

	public:
		static const int FullSize = POW_3_5;
		static const int ReducedSize = diagSize * halfSize * (halfSize + 1) / 2;

		CPattern_D5() { m_weights.resize(FullSize); }
		void set_weights(const std::vector<float>& compressed_weights)
		{
			int ReducedIndex;
			int FullIndex;

			For_each_configuration_in_pattern_do_fkt(Pattern,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex0(P, O);
					FullIndex = FullPatternIndex(P, O, Pattern);
					m_weights[FullIndex] = compressed_weights[ReducedIndex];
				}
			);
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[FullPatternIndex(P, O, Pattern )]
				 + m_weights[FullPatternIndex(P, O, PatternH)]
				 + m_weights[FullPatternIndex(P, O, PatternC)]
				 + m_weights[FullPatternIndex(P, O, PatternV)];
		}
		inline float score(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			uint64_t diff = (new_P ^ old_P) | (new_O ^ old_O);

			return m_weights[(diff & Pattern ) ? FullPatternIndex(new_P, new_O, Pattern ) : *(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[(diff & PatternH) ? FullPatternIndex(new_P, new_O, PatternH) : *(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[(diff & PatternC) ? FullPatternIndex(new_P, new_O, PatternC) : *(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[(diff & PatternV) ? FullPatternIndex(new_P, new_O, PatternV) : *(Indices_P+3) + (*(Indices_O+3) << 1)];
		}
		inline float score(const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			return m_weights[*(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[*(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[*(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[*(Indices_P+3) + (*(Indices_O+3) << 1)];
		}

		static void UpdateIndexVec(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, std::vector<int>::iterator Indices_P, std::vector<int>::iterator Indices_O)
		{
			uint64_t diff_P = new_P ^ old_P;
			uint64_t diff_O = new_O ^ old_O;
			if (diff_P & Pattern ) *(Indices_P+0) = SumPow3[PExt(new_P, Pattern )];
			if (diff_P & PatternH) *(Indices_P+1) = SumPow3[PExt(new_P, PatternH)];
			if (diff_P & PatternC) *(Indices_P+2) = SumPow3[PExt(new_P, PatternC)];
			if (diff_P & PatternV) *(Indices_P+3) = SumPow3[PExt(new_P, PatternV)];
			if (diff_O & Pattern ) *(Indices_O+0) = SumPow3[PExt(new_O, Pattern )];
			if (diff_O & PatternH) *(Indices_O+1) = SumPow3[PExt(new_O, PatternH)];
			if (diff_O & PatternC) *(Indices_O+2) = SumPow3[PExt(new_O, PatternC)];
			if (diff_O & PatternV) *(Indices_O+3) = SumPow3[PExt(new_O, PatternV)];
		}

		static void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec, int Offset = 0)
		{
			vec.push_back(ReducedPatternIndex0(P, O) + Offset);
			vec.push_back(ReducedPatternIndex1(P, O) + Offset);
			vec.push_back(ReducedPatternIndex2(P, O) + Offset);
			vec.push_back(ReducedPatternIndex3(P, O) + Offset);
		}
	};

	class CPattern_D6
	{
		static const int halfSize = POW_3_3;
		static const uint64_t Pattern = 0x0000010204081020ULL;
		static const uint64_t PatternH = Flip<Pattern>::horizontal;
		static const uint64_t PatternV = Flip<Pattern>::vertical;
		static const uint64_t PatternD = Flip<Pattern>::diagonal;
		static const uint64_t PatternC = Flip<Pattern>::codiagonal;
		static const uint64_t PatternHV = Flip<Flip<Pattern>::horizontal>::vertical;
		static const uint64_t PatternHD = Flip<Flip<Pattern>::horizontal>::diagonal;
		static const uint64_t PatternHC = Flip<Flip<Pattern>::horizontal>::codiagonal;

		std::vector<float> m_weights; //m_weights[FullIndex]

		static int ReducedPatternIndex0(const uint64_t P, const uint64_t O)
		{
			const int indexA = FullPatternIndex(              P ,               O , 0x0000000000081020ULL);
			const int indexB = FullPatternIndex(FlipDiagonal(P), FlipDiagonal(O), 0x0000000000081020ULL);
			return (indexA > indexB) ? (halfSize*indexB + indexA - (((indexB + 1)*indexB) >> 1)) : (halfSize*indexA + indexB - (((indexA + 1)*indexA) >> 1));
		}
		static inline int ReducedPatternIndex1(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipHorizontal(P), FlipHorizontal(O)); }
		static inline int ReducedPatternIndex2(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		static inline int ReducedPatternIndex3(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }

	public:
		static const int FullSize = POW_3_6;
		static const int ReducedSize = halfSize * (halfSize + 1) / 2;

		CPattern_D6() { m_weights.resize(FullSize); }
		void set_weights(const std::vector<float>& compressed_weights)
		{
			int ReducedIndex;
			int FullIndex;

			For_each_configuration_in_pattern_do_fkt(Pattern,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex0(P, O);
					FullIndex = FullPatternIndex(P, O, Pattern);
					m_weights[FullIndex] = compressed_weights[ReducedIndex];
				}
			);
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[FullPatternIndex(P, O, Pattern )]
				 + m_weights[FullPatternIndex(P, O, PatternH)]
				 + m_weights[FullPatternIndex(P, O, PatternC)]
				 + m_weights[FullPatternIndex(P, O, PatternV)];
		}
		inline float score(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			uint64_t diff = (new_P ^ old_P) | (new_O ^ old_O);

			return m_weights[(diff & Pattern ) ? FullPatternIndex(new_P, new_O, Pattern ) : *(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[(diff & PatternH) ? FullPatternIndex(new_P, new_O, PatternH) : *(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[(diff & PatternC) ? FullPatternIndex(new_P, new_O, PatternC) : *(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[(diff & PatternV) ? FullPatternIndex(new_P, new_O, PatternV) : *(Indices_P+3) + (*(Indices_O+3) << 1)];
		}
		inline float score(const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			return m_weights[*(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[*(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[*(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[*(Indices_P+3) + (*(Indices_O+3) << 1)];
		}

		static void UpdateIndexVec(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, std::vector<int>::iterator Indices_P, std::vector<int>::iterator Indices_O)
		{
			uint64_t diff_P = new_P ^ old_P;
			uint64_t diff_O = new_O ^ old_O;
			if (diff_P & Pattern ) *(Indices_P+0) = SumPow3[PExt(new_P, Pattern )];
			if (diff_P & PatternH) *(Indices_P+1) = SumPow3[PExt(new_P, PatternH)];
			if (diff_P & PatternC) *(Indices_P+2) = SumPow3[PExt(new_P, PatternC)];
			if (diff_P & PatternV) *(Indices_P+3) = SumPow3[PExt(new_P, PatternV)];
			if (diff_O & Pattern ) *(Indices_O+0) = SumPow3[PExt(new_O, Pattern )];
			if (diff_O & PatternH) *(Indices_O+1) = SumPow3[PExt(new_O, PatternH)];
			if (diff_O & PatternC) *(Indices_O+2) = SumPow3[PExt(new_O, PatternC)];
			if (diff_O & PatternV) *(Indices_O+3) = SumPow3[PExt(new_O, PatternV)];
		}

		static void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec, int Offset = 0)
		{
			vec.push_back(ReducedPatternIndex0(P, O) + Offset);
			vec.push_back(ReducedPatternIndex1(P, O) + Offset);
			vec.push_back(ReducedPatternIndex2(P, O) + Offset);
			vec.push_back(ReducedPatternIndex3(P, O) + Offset);
		}
	};

	class CPattern_D7
	{
		static const int halfSize = POW_3_3;
		static const int diagSize = POW_2_1;
		static const uint64_t Pattern = 0x0001020408102040ULL;
		static const uint64_t PatternH = Flip<Pattern>::horizontal;
		static const uint64_t PatternV = Flip<Pattern>::vertical;
		static const uint64_t PatternD = Flip<Pattern>::diagonal;
		static const uint64_t PatternC = Flip<Pattern>::codiagonal;
		static const uint64_t PatternHV = Flip<Flip<Pattern>::horizontal>::vertical;
		static const uint64_t PatternHD = Flip<Flip<Pattern>::horizontal>::diagonal;
		static const uint64_t PatternHC = Flip<Flip<Pattern>::horizontal>::codiagonal;

		std::vector<float> m_weights; //m_weights[FullIndex]

		static int ReducedPatternIndex0(const uint64_t P, const uint64_t O)
		{
			const int indexA = FullPatternIndex(              P ,               O , 0x0000000000102040ULL);
			const int indexB = FullPatternIndex(FlipDiagonal(P), FlipDiagonal(O), 0x0000000000102040ULL);
			const int indexD = (O & 0x0000000008000000ULL) ? 1 : 0;
			return (indexA > indexB) ? (diagSize*(halfSize*indexB+indexA - (((indexB+1)*indexB) >> 1)) + indexD) : (diagSize*(halfSize*indexA+indexB - (((indexA+1)*indexA) >> 1)) + indexD);
		}
		static inline int ReducedPatternIndex1(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipHorizontal(P), FlipHorizontal(O)); }
		static inline int ReducedPatternIndex2(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		static inline int ReducedPatternIndex3(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }

	public:
		static const int FullSize = POW_3_7;
		static const int ReducedSize = diagSize * halfSize * (halfSize + 1) / 2;

		CPattern_D7() { m_weights.resize(FullSize); }
		void set_weights(const std::vector<float>& compressed_weights)
		{
			int ReducedIndex;
			int FullIndex;

			For_each_configuration_in_pattern_do_fkt(Pattern,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex0(P, O);
					FullIndex = FullPatternIndex(P, O, Pattern);
					m_weights[FullIndex] = compressed_weights[ReducedIndex];
				}
			);
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[FullPatternIndex(P, O, Pattern )]
				 + m_weights[FullPatternIndex(P, O, PatternH)]
				 + m_weights[FullPatternIndex(P, O, PatternC)]
				 + m_weights[FullPatternIndex(P, O, PatternV)];
		}
		inline float score(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			uint64_t diff = (new_P ^ old_P) | (new_O ^ old_O);

			return m_weights[(diff & Pattern ) ? FullPatternIndex(new_P, new_O, Pattern ) : *(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[(diff & PatternH) ? FullPatternIndex(new_P, new_O, PatternH) : *(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[(diff & PatternC) ? FullPatternIndex(new_P, new_O, PatternC) : *(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[(diff & PatternV) ? FullPatternIndex(new_P, new_O, PatternV) : *(Indices_P+3) + (*(Indices_O+3) << 1)];
		}
		inline float score(const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			return m_weights[*(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[*(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[*(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[*(Indices_P+3) + (*(Indices_O+3) << 1)];
		}

		static void UpdateIndexVec(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, std::vector<int>::iterator Indices_P, std::vector<int>::iterator Indices_O)
		{
			uint64_t diff_P = new_P ^ old_P;
			uint64_t diff_O = new_O ^ old_O;
			if (diff_P & Pattern ) *(Indices_P+0) = SumPow3[PExt(new_P, Pattern )];
			if (diff_P & PatternH) *(Indices_P+1) = SumPow3[PExt(new_P, PatternH)];
			if (diff_P & PatternC) *(Indices_P+2) = SumPow3[PExt(new_P, PatternC)];
			if (diff_P & PatternV) *(Indices_P+3) = SumPow3[PExt(new_P, PatternV)];
			if (diff_O & Pattern ) *(Indices_O+0) = SumPow3[PExt(new_O, Pattern )];
			if (diff_O & PatternH) *(Indices_O+1) = SumPow3[PExt(new_O, PatternH)];
			if (diff_O & PatternC) *(Indices_O+2) = SumPow3[PExt(new_O, PatternC)];
			if (diff_O & PatternV) *(Indices_O+3) = SumPow3[PExt(new_O, PatternV)];
		}

		static void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec, int Offset = 0)
		{
			vec.push_back(ReducedPatternIndex0(P, O) + Offset);
			vec.push_back(ReducedPatternIndex1(P, O) + Offset);
			vec.push_back(ReducedPatternIndex2(P, O) + Offset);
			vec.push_back(ReducedPatternIndex3(P, O) + Offset);
		}
	};

	class CPattern_Comet
	{
		static const int halfSize = POW_3_1;
		static const int diagSize = POW_3_6 * POW_2_2;
		static const uint64_t Pattern = 0x8040201008040303ULL;
		static const uint64_t PatternH = Flip<Pattern>::horizontal;
		static const uint64_t PatternV = Flip<Pattern>::vertical;
		static const uint64_t PatternD = Flip<Pattern>::diagonal;
		static const uint64_t PatternC = Flip<Pattern>::codiagonal;
		static const uint64_t PatternHV = Flip<Flip<Pattern>::horizontal>::vertical;
		static const uint64_t PatternHD = Flip<Flip<Pattern>::horizontal>::diagonal;
		static const uint64_t PatternHC = Flip<Flip<Pattern>::horizontal>::codiagonal;

		std::vector<std::vector<float>> m_weights; //m_weights[FullIndex]

		static int ReducedPatternIndex0(const uint64_t P, const uint64_t O)
		{
			const int indexA = FullPatternIndex(              P ,               O , 0x0000000000000002ULL);
			const int indexB = FullPatternIndex(FlipDiagonal(P), FlipDiagonal(O), 0x0000000000000002ULL);
			const int indexD = FullPatternIndex(              P ,               O , 0x8040200000040201ULL) * 4 + PExt(O, 0x0000001008000000ULL);
			return (indexA > indexB) ? (diagSize*(halfSize*indexB+indexA - (((indexB+1)*indexB) >> 1)) + indexD) : (diagSize*(halfSize*indexA+indexB - (((indexA+1)*indexA) >> 1)) + indexD);
		}
		static inline int ReducedPatternIndex1(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipHorizontal(P), FlipHorizontal(O)); }
		static inline int ReducedPatternIndex2(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		static inline int ReducedPatternIndex3(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }

	public:
		static const int FullSize = POW_3_10;
		static const int ReducedSize = diagSize * halfSize * (halfSize + 1) / 2;

		CPattern_Comet()
		{
			//Initialize weights
			m_weights.resize(4);
			for (int i = 0; i < 4; ++i)
				m_weights[i].resize(FullSize);
		}
		void set_weights(const std::vector<float>& compressed_weights)
		{
			int ReducedIndex;
			int FullIndex;

			For_each_configuration_in_pattern_do_fkt(Pattern,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex0(P, O);
					FullIndex = FullPatternIndex(P, O, Pattern);
					m_weights[0][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternH,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex1(P, O);
					FullIndex = FullPatternIndex(P, O, PatternH);
					m_weights[1][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternC,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex2(P, O);
					FullIndex = FullPatternIndex(P, O, PatternC);
					m_weights[2][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternV,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex3(P, O);
					FullIndex = FullPatternIndex(P, O, PatternV);
					m_weights[3][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[0][FullPatternIndex(P, O, Pattern )]
				 + m_weights[1][FullPatternIndex(P, O, PatternH)]
				 + m_weights[2][FullPatternIndex(P, O, PatternC)]
				 + m_weights[3][FullPatternIndex(P, O, PatternV)];
		}
		inline float score(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			uint64_t diff = (new_P ^ old_P) | (new_O ^ old_O);

			return m_weights[0][(diff & Pattern ) ? FullPatternIndex(new_P, new_O, Pattern ) : *(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[1][(diff & PatternH) ? FullPatternIndex(new_P, new_O, PatternH) : *(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[2][(diff & PatternC) ? FullPatternIndex(new_P, new_O, PatternC) : *(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[3][(diff & PatternV) ? FullPatternIndex(new_P, new_O, PatternV) : *(Indices_P+3) + (*(Indices_O+3) << 1)];
		}
		inline float score(const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			return m_weights[0][*(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[1][*(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[2][*(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[3][*(Indices_P+3) + (*(Indices_O+3) << 1)];
		}

		static void UpdateIndexVec(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, std::vector<int>::iterator Indices_P, std::vector<int>::iterator Indices_O)
		{
			uint64_t diff_P = new_P ^ old_P;
			uint64_t diff_O = new_O ^ old_O;
			if (diff_P & Pattern ) *(Indices_P+0) = SumPow3[PExt(new_P, Pattern )];
			if (diff_P & PatternH) *(Indices_P+1) = SumPow3[PExt(new_P, PatternH)];
			if (diff_P & PatternC) *(Indices_P+2) = SumPow3[PExt(new_P, PatternC)];
			if (diff_P & PatternV) *(Indices_P+3) = SumPow3[PExt(new_P, PatternV)];
			if (diff_O & Pattern ) *(Indices_O+0) = SumPow3[PExt(new_O, Pattern )];
			if (diff_O & PatternH) *(Indices_O+1) = SumPow3[PExt(new_O, PatternH)];
			if (diff_O & PatternC) *(Indices_O+2) = SumPow3[PExt(new_O, PatternC)];
			if (diff_O & PatternV) *(Indices_O+3) = SumPow3[PExt(new_O, PatternV)];
		}

		static void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec, int Offset = 0)
		{
			vec.push_back(ReducedPatternIndex0(P, O) + Offset);
			vec.push_back(ReducedPatternIndex1(P, O) + Offset);
			vec.push_back(ReducedPatternIndex2(P, O) + Offset);
			vec.push_back(ReducedPatternIndex3(P, O) + Offset);
		}
	};

	class CPattern_Eplus
	{
		static const int halfSize = POW_3_5;
		static const uint64_t Pattern = 0x0000000000003CBDULL;
		static const uint64_t PatternH = Flip<Pattern>::horizontal;
		static const uint64_t PatternV = Flip<Pattern>::vertical;
		static const uint64_t PatternD = Flip<Pattern>::diagonal;
		static const uint64_t PatternC = Flip<Pattern>::codiagonal;
		static const uint64_t PatternHV = Flip<Flip<Pattern>::horizontal>::vertical;
		static const uint64_t PatternHD = Flip<Flip<Pattern>::horizontal>::diagonal;
		static const uint64_t PatternHC = Flip<Flip<Pattern>::horizontal>::codiagonal;

		std::vector<std::vector<float>> m_weights; //m_weights[Index][FullIndex]

		static int ReducedPatternIndex0(const uint64_t P, const uint64_t O)
		{
			const int indexA = FullPatternIndex(                P ,                 O , 0x0000000000000C0DULL);
			const int indexB = FullPatternIndex(FlipHorizontal(P), FlipHorizontal(O), 0x0000000000000C0DULL);
			return (indexA > indexB) ? (halfSize*indexB + indexA - (((indexB + 1)*indexB) >> 1)) : (halfSize*indexA + indexB - (((indexA + 1)*indexA) >> 1));
		}
		static inline int ReducedPatternIndex1(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		static inline int ReducedPatternIndex2(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }
		static inline int ReducedPatternIndex3(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipDiagonal(P), FlipDiagonal(O)); }

	public:
		static const int FullSize = POW_3_10;
		static const int ReducedSize = halfSize * (halfSize + 1) / 2;

		CPattern_Eplus()
		{
			//Initialize weights
			m_weights.resize(4);
			for (int i = 0; i < 4; ++i)
				m_weights[i].resize(FullSize);
		}
		void set_weights(const std::vector<float>& compressed_weights)
		{
			int ReducedIndex;
			int FullIndex;

			For_each_configuration_in_pattern_do_fkt(Pattern,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex0(P, O);
					FullIndex = FullPatternIndex(P, O, Pattern);
					m_weights[0][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternC,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex1(P, O);
					FullIndex = FullPatternIndex(P, O, PatternC);
					m_weights[1][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternV,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex2(P, O);
					FullIndex = FullPatternIndex(P, O, PatternV);
					m_weights[2][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternD,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex3(P, O);
					FullIndex = FullPatternIndex(P, O, PatternD);
					m_weights[3][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[0][FullPatternIndex(P, O, Pattern )]
				 + m_weights[1][FullPatternIndex(P, O, PatternC)]
				 + m_weights[2][FullPatternIndex(P, O, PatternV)]
				 + m_weights[3][FullPatternIndex(P, O, PatternD)];
		}
		inline float score(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			uint64_t diff = (new_P ^ old_P) | (new_O ^ old_O);

			return m_weights[0][(diff & Pattern ) ? FullPatternIndex(new_P, new_O, Pattern ) : *(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[1][(diff & PatternC) ? FullPatternIndex(new_P, new_O, PatternC) : *(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[2][(diff & PatternV) ? FullPatternIndex(new_P, new_O, PatternV) : *(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[3][(diff & PatternD) ? FullPatternIndex(new_P, new_O, PatternD) : *(Indices_P+3) + (*(Indices_O+3) << 1)];
		}
		inline float score(const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			return m_weights[0][*(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[1][*(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[2][*(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[3][*(Indices_P+3) + (*(Indices_O+3) << 1)];
		}

		static void UpdateIndexVec(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, std::vector<int>::iterator Indices_P, std::vector<int>::iterator Indices_O)
		{
			uint64_t diff_P = new_P ^ old_P;
			uint64_t diff_O = new_O ^ old_O;
			if (diff_P & Pattern ) *(Indices_P+0) = SumPow3[PExt(new_P, Pattern )];
			if (diff_P & PatternC) *(Indices_P+1) = SumPow3[PExt(new_P, PatternC)];
			if (diff_P & PatternV) *(Indices_P+2) = SumPow3[PExt(new_P, PatternV)];
			if (diff_P & PatternD) *(Indices_P+3) = SumPow3[PExt(new_P, PatternD)];
			if (diff_O & Pattern ) *(Indices_O+0) = SumPow3[PExt(new_O, Pattern )];
			if (diff_O & PatternC) *(Indices_O+1) = SumPow3[PExt(new_O, PatternC)];
			if (diff_O & PatternV) *(Indices_O+2) = SumPow3[PExt(new_O, PatternV)];
			if (diff_O & PatternD) *(Indices_O+3) = SumPow3[PExt(new_O, PatternD)];
		}


		static void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec, int Offset = 0)
		{
			vec.push_back(ReducedPatternIndex0(P, O) + Offset);
			vec.push_back(ReducedPatternIndex1(P, O) + Offset);
			vec.push_back(ReducedPatternIndex2(P, O) + Offset);
			vec.push_back(ReducedPatternIndex3(P, O) + Offset);
		}
	};

	class CPattern_C3p1
	{
		static const int halfSize = POW_3_3;
		static const int diagSize = POW_3_2;
		static const uint64_t Pattern = 0x000000000101030FULL;
		static const uint64_t PatternH = Flip<Pattern>::horizontal;
		static const uint64_t PatternV = Flip<Pattern>::vertical;
		static const uint64_t PatternD = Flip<Pattern>::diagonal;
		static const uint64_t PatternC = Flip<Pattern>::codiagonal;
		static const uint64_t PatternHV = Flip<Flip<Pattern>::horizontal>::vertical;
		static const uint64_t PatternHD = Flip<Flip<Pattern>::horizontal>::diagonal;
		static const uint64_t PatternHC = Flip<Flip<Pattern>::horizontal>::codiagonal;

		std::vector<std::vector<float>> m_weights; //m_weights[Index][FullIndex]

		static int ReducedPatternIndex0(const uint64_t P, const uint64_t O)
		{
			const int indexA = FullPatternIndex(              P ,               O , 0x0000000001010100ULL);
			const int indexB = FullPatternIndex(FlipDiagonal(P), FlipDiagonal(O), 0x0000000001010100ULL);
			const int indexD = FullPatternIndex(              P ,               O , 0x0000000000000201ULL);
			return (indexA > indexB) ? (diagSize*(halfSize*indexB+indexA - (((indexB+1)*indexB) >> 1)) + indexD) : (diagSize*(halfSize*indexA+indexB - (((indexA+1)*indexA) >> 1)) + indexD);
		}
		static inline int ReducedPatternIndex1(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipHorizontal(P), FlipHorizontal(O)); }
		static inline int ReducedPatternIndex2(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		static inline int ReducedPatternIndex3(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }

	public:
		static const int FullSize = POW_3_8;
		static const int ReducedSize = diagSize * halfSize * (halfSize + 1) / 2;

		CPattern_C3p1()
		{
			//Initialize weights
			m_weights.resize(4);
			for (int i = 0; i < 4; ++i)
				m_weights[i].resize(FullSize);
		}
		void set_weights(const std::vector<float>& compressed_weights)
		{
			int ReducedIndex;
			int FullIndex;

			For_each_configuration_in_pattern_do_fkt(Pattern,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex0(P, O);
					FullIndex = FullPatternIndex(P, O, Pattern);
					m_weights[0][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternH,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex1(P, O);
					FullIndex = FullPatternIndex(P, O, PatternH);
					m_weights[1][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternC,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex2(P, O);
					FullIndex = FullPatternIndex(P, O, PatternC);
					m_weights[2][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternV,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex3(P, O);
					FullIndex = FullPatternIndex(P, O, PatternV);
					m_weights[3][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[0][FullPatternIndex(P, O, Pattern )]
				 + m_weights[1][FullPatternIndex(P, O, PatternH)]
				 + m_weights[2][FullPatternIndex(P, O, PatternC)]
				 + m_weights[3][FullPatternIndex(P, O, PatternV)];
		}
		inline float score(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			uint64_t diff = (new_P ^ old_P) | (new_O ^ old_O);

			return m_weights[0][(diff & Pattern ) ? FullPatternIndex(new_P, new_O, Pattern ) : *(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[1][(diff & PatternH) ? FullPatternIndex(new_P, new_O, PatternH) : *(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[2][(diff & PatternC) ? FullPatternIndex(new_P, new_O, PatternC) : *(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[3][(diff & PatternV) ? FullPatternIndex(new_P, new_O, PatternV) : *(Indices_P+3) + (*(Indices_O+3) << 1)];
		}
		inline float score(const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			return m_weights[0][*(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[1][*(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[2][*(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[3][*(Indices_P+3) + (*(Indices_O+3) << 1)];
		}

		static void UpdateIndexVec(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, std::vector<int>::iterator Indices_P, std::vector<int>::iterator Indices_O)
		{
			uint64_t diff_P = new_P ^ old_P;
			uint64_t diff_O = new_O ^ old_O;
			if (diff_P & Pattern ) *(Indices_P+0) = SumPow3[PExt(new_P, Pattern )];
			if (diff_P & PatternH) *(Indices_P+1) = SumPow3[PExt(new_P, PatternH)];
			if (diff_P & PatternC) *(Indices_P+2) = SumPow3[PExt(new_P, PatternC)];
			if (diff_P & PatternV) *(Indices_P+3) = SumPow3[PExt(new_P, PatternV)];
			if (diff_O & Pattern ) *(Indices_O+0) = SumPow3[PExt(new_O, Pattern )];
			if (diff_O & PatternH) *(Indices_O+1) = SumPow3[PExt(new_O, PatternH)];
			if (diff_O & PatternC) *(Indices_O+2) = SumPow3[PExt(new_O, PatternC)];
			if (diff_O & PatternV) *(Indices_O+3) = SumPow3[PExt(new_O, PatternV)];
		}

		static void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec, int Offset = 0)
		{
			vec.push_back(ReducedPatternIndex0(P, O) + Offset);
			vec.push_back(ReducedPatternIndex1(P, O) + Offset);
			vec.push_back(ReducedPatternIndex2(P, O) + Offset);
			vec.push_back(ReducedPatternIndex3(P, O) + Offset);
		}
	};

	class CPattern_C3p2
	{
		static const int halfSize = POW_3_4;
		static const int diagSize = POW_3_2;
		static const uint64_t Pattern = 0x000000010101031FULL;
		static const uint64_t PatternH = Flip<Pattern>::horizontal;
		static const uint64_t PatternV = Flip<Pattern>::vertical;
		static const uint64_t PatternD = Flip<Pattern>::diagonal;
		static const uint64_t PatternC = Flip<Pattern>::codiagonal;
		static const uint64_t PatternHV = Flip<Flip<Pattern>::horizontal>::vertical;
		static const uint64_t PatternHD = Flip<Flip<Pattern>::horizontal>::diagonal;
		static const uint64_t PatternHC = Flip<Flip<Pattern>::horizontal>::codiagonal;

		std::vector<std::vector<float>> m_weights; //m_weights[Index][FullIndex]

		static int ReducedPatternIndex0(const uint64_t P, const uint64_t O)
		{
			const int indexA = FullPatternIndex(              P ,               O , 0x0000000101010100ULL);
			const int indexB = FullPatternIndex(FlipDiagonal(P), FlipDiagonal(O), 0x0000000101010100ULL);
			const int indexD = FullPatternIndex(              P ,               O , 0x0000000000000201ULL);
			return (indexA > indexB) ? (diagSize*(halfSize*indexB+indexA - (((indexB+1)*indexB) >> 1)) + indexD) : (diagSize*(halfSize*indexA+indexB - (((indexA+1)*indexA) >> 1)) + indexD);
		}
		static inline int ReducedPatternIndex1(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipHorizontal(P), FlipHorizontal(O)); }
		static inline int ReducedPatternIndex2(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		static inline int ReducedPatternIndex3(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }

	public:
		static const int FullSize = POW_3_10;
		static const int ReducedSize = diagSize * halfSize * (halfSize + 1) / 2;

		CPattern_C3p2()
		{
			//Initialize weights
			m_weights.resize(4);
			for (int i = 0; i < 4; ++i)
				m_weights[i].resize(FullSize);
		}
		void set_weights(const std::vector<float>& compressed_weights)
		{
			int ReducedIndex;
			int FullIndex;

			For_each_configuration_in_pattern_do_fkt(Pattern,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex0(P, O);
					FullIndex = FullPatternIndex(P, O, Pattern);
					m_weights[0][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternH,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex1(P, O);
					FullIndex = FullPatternIndex(P, O, PatternH);
					m_weights[1][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternC,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex2(P, O);
					FullIndex = FullPatternIndex(P, O, PatternC);
					m_weights[2][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternV,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex3(P, O);
					FullIndex = FullPatternIndex(P, O, PatternV);
					m_weights[3][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[0][FullPatternIndex(P, O, Pattern )]
				 + m_weights[1][FullPatternIndex(P, O, PatternH)]
				 + m_weights[2][FullPatternIndex(P, O, PatternC)]
				 + m_weights[3][FullPatternIndex(P, O, PatternV)];
		}
		inline float score(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			uint64_t diff = (new_P ^ old_P) | (new_O ^ old_O);

			return m_weights[0][(diff & Pattern ) ? FullPatternIndex(new_P, new_O, Pattern ) : *(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[1][(diff & PatternH) ? FullPatternIndex(new_P, new_O, PatternH) : *(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[2][(diff & PatternC) ? FullPatternIndex(new_P, new_O, PatternC) : *(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[3][(diff & PatternV) ? FullPatternIndex(new_P, new_O, PatternV) : *(Indices_P+3) + (*(Indices_O+3) << 1)];
		}
		inline float score(const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			return m_weights[0][*(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[1][*(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[2][*(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[3][*(Indices_P+3) + (*(Indices_O+3) << 1)];
		}

		static void UpdateIndexVec(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, std::vector<int>::iterator Indices_P, std::vector<int>::iterator Indices_O)
		{
			uint64_t diff_P = new_P ^ old_P;
			uint64_t diff_O = new_O ^ old_O;
			if (diff_P & Pattern ) *(Indices_P+0) = SumPow3[PExt(new_P, Pattern )];
			if (diff_P & PatternH) *(Indices_P+1) = SumPow3[PExt(new_P, PatternH)];
			if (diff_P & PatternC) *(Indices_P+2) = SumPow3[PExt(new_P, PatternC)];
			if (diff_P & PatternV) *(Indices_P+3) = SumPow3[PExt(new_P, PatternV)];
			if (diff_O & Pattern ) *(Indices_O+0) = SumPow3[PExt(new_O, Pattern )];
			if (diff_O & PatternH) *(Indices_O+1) = SumPow3[PExt(new_O, PatternH)];
			if (diff_O & PatternC) *(Indices_O+2) = SumPow3[PExt(new_O, PatternC)];
			if (diff_O & PatternV) *(Indices_O+3) = SumPow3[PExt(new_O, PatternV)];
		}

		static void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec, int Offset = 0)
		{
			vec.push_back(ReducedPatternIndex0(P, O) + Offset);
			vec.push_back(ReducedPatternIndex1(P, O) + Offset);
			vec.push_back(ReducedPatternIndex2(P, O) + Offset);
			vec.push_back(ReducedPatternIndex3(P, O) + Offset);
		}
	};

	class CPattern_Q0
	{
		static const int halfSize = POW_3_3;
		static const int diagSize = POW_3_3;
		static const uint64_t Pattern = 0x0000000000070707ULL;
		static const uint64_t PatternH = Flip<Pattern>::horizontal;
		static const uint64_t PatternV = Flip<Pattern>::vertical;
		static const uint64_t PatternD = Flip<Pattern>::diagonal;
		static const uint64_t PatternC = Flip<Pattern>::codiagonal;
		static const uint64_t PatternHV = Flip<Flip<Pattern>::horizontal>::vertical;
		static const uint64_t PatternHD = Flip<Flip<Pattern>::horizontal>::diagonal;
		static const uint64_t PatternHC = Flip<Flip<Pattern>::horizontal>::codiagonal;

		std::vector<std::vector<float>> m_weights; //m_weights[FullIndex]

		static int ReducedPatternIndex0(const uint64_t P, const uint64_t O)
		{
			const int indexA = FullPatternIndex(              P ,               O , 0x0000000000030100ULL);
			const int indexB = FullPatternIndex(FlipDiagonal(P), FlipDiagonal(O), 0x0000000000030100ULL);
			const int indexD = FullPatternIndex(              P ,               O , 0x0000000000040201ULL);
			return (indexA > indexB) ? (diagSize*(halfSize*indexB+indexA - (((indexB+1)*indexB) >> 1)) + indexD) : (diagSize*(halfSize*indexA+indexB - (((indexA+1)*indexA) >> 1)) + indexD);
		}
		static inline int ReducedPatternIndex1(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipHorizontal(P), FlipHorizontal(O)); }
		static inline int ReducedPatternIndex2(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		static inline int ReducedPatternIndex3(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }

	public:
		static const int FullSize = POW_3_9;
		static const int ReducedSize = diagSize * halfSize * (halfSize + 1) / 2;

		CPattern_Q0()
		{
			//Initialize weights
			m_weights.resize(4);
			for (int i = 0; i < 4; ++i)
				m_weights[i].resize(FullSize);
		}
		void set_weights(const std::vector<float>& compressed_weights)
		{
			int ReducedIndex;
			int FullIndex;

			For_each_configuration_in_pattern_do_fkt(Pattern,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex0(P, O);
					FullIndex = FullPatternIndex(P, O, Pattern);
					m_weights[0][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternH,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex1(P, O);
					FullIndex = FullPatternIndex(P, O, PatternH);
					m_weights[1][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternC,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex2(P, O);
					FullIndex = FullPatternIndex(P, O, PatternC);
					m_weights[2][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternV,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex3(P, O);
					FullIndex = FullPatternIndex(P, O, PatternV);
					m_weights[3][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[0][FullPatternIndex(P, O, Pattern )]
				 + m_weights[1][FullPatternIndex(P, O, PatternH)]
				 + m_weights[2][FullPatternIndex(P, O, PatternC)]
				 + m_weights[3][FullPatternIndex(P, O, PatternV)];
		}
		inline float score(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			uint64_t diff = (new_P ^ old_P) | (new_O ^ old_O);

			return m_weights[0][(diff & Pattern ) ? FullPatternIndex(new_P, new_O, Pattern ) : *(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[1][(diff & PatternH) ? FullPatternIndex(new_P, new_O, PatternH) : *(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[2][(diff & PatternC) ? FullPatternIndex(new_P, new_O, PatternC) : *(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[3][(diff & PatternV) ? FullPatternIndex(new_P, new_O, PatternV) : *(Indices_P+3) + (*(Indices_O+3) << 1)];
		}
		inline float score(const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			return m_weights[0][*(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[1][*(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[2][*(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[3][*(Indices_P+3) + (*(Indices_O+3) << 1)];
		}

		static void UpdateIndexVec(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, std::vector<int>::iterator Indices_P, std::vector<int>::iterator Indices_O)
		{
			uint64_t diff_P = new_P ^ old_P;
			uint64_t diff_O = new_O ^ old_O;
			if (diff_P & Pattern ) *(Indices_P+0) = SumPow3[PExt(new_P, Pattern )];
			if (diff_P & PatternH) *(Indices_P+1) = SumPow3[PExt(new_P, PatternH)];
			if (diff_P & PatternC) *(Indices_P+2) = SumPow3[PExt(new_P, PatternC)];
			if (diff_P & PatternV) *(Indices_P+3) = SumPow3[PExt(new_P, PatternV)];
			if (diff_O & Pattern ) *(Indices_O+0) = SumPow3[PExt(new_O, Pattern )];
			if (diff_O & PatternH) *(Indices_O+1) = SumPow3[PExt(new_O, PatternH)];
			if (diff_O & PatternC) *(Indices_O+2) = SumPow3[PExt(new_O, PatternC)];
			if (diff_O & PatternV) *(Indices_O+3) = SumPow3[PExt(new_O, PatternV)];
		}

		static void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec, int Offset = 0)
		{
			vec.push_back(ReducedPatternIndex0(P, O) + Offset);
			vec.push_back(ReducedPatternIndex1(P, O) + Offset);
			vec.push_back(ReducedPatternIndex2(P, O) + Offset);
			vec.push_back(ReducedPatternIndex3(P, O) + Offset);
		}
	};

	class CPattern_B5
	{
		static const uint64_t Pattern = 0x0000000000001F1FULL;
		static const uint64_t PatternH = Flip<Pattern>::horizontal;
		static const uint64_t PatternV = Flip<Pattern>::vertical;
		static const uint64_t PatternD = Flip<Pattern>::diagonal;
		static const uint64_t PatternC = Flip<Pattern>::codiagonal;
		static const uint64_t PatternHV = Flip<Flip<Pattern>::horizontal>::vertical;
		static const uint64_t PatternHD = Flip<Flip<Pattern>::horizontal>::diagonal;
		static const uint64_t PatternHC = Flip<Flip<Pattern>::horizontal>::codiagonal;

		std::vector<std::vector<float>> m_weights; //m_weights[Index][FullIndex]

		static inline int ReducedPatternIndex0(const uint64_t P, const uint64_t O) { return FullPatternIndex(P, O, 0x0000000000001F1FULL); }
		static inline int ReducedPatternIndex1(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipHorizontal(P), FlipHorizontal(O)); }
		static inline int ReducedPatternIndex2(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }
		static inline int ReducedPatternIndex3(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipDiagonal(P), FlipDiagonal(O)); }
		static inline int ReducedPatternIndex4(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		static inline int ReducedPatternIndex5(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipHorizontal(FlipVertical(P)), FlipHorizontal(FlipVertical(O))); }
		static inline int ReducedPatternIndex6(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipHorizontal(FlipDiagonal(P)), FlipHorizontal(FlipDiagonal(O))); }
		static inline int ReducedPatternIndex7(const uint64_t P, const uint64_t O) { return ReducedPatternIndex0(FlipHorizontal(FlipCodiagonal(P)), FlipHorizontal(FlipCodiagonal(O))); }

	public:
		static const int FullSize = POW_3_10;
		static const int ReducedSize = POW_3_10;

		CPattern_B5()
		{
			//Initialize weights
			m_weights.resize(8);
			for (int i = 0; i < 8; ++i)
				m_weights[i].resize(FullSize);
		}
		void set_weights(const std::vector<float>& compressed_weights)
		{
			int ReducedIndex;
			int FullIndex;

			For_each_configuration_in_pattern_do_fkt(Pattern,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex0(P, O);
					FullIndex = FullPatternIndex(P, O, Pattern);
					m_weights[0][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternH,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex1(P, O);
					FullIndex = FullPatternIndex(P, O, PatternH);
					m_weights[1][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternV,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex2(P, O);
					FullIndex = FullPatternIndex(P, O, PatternV);
					m_weights[2][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternD,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex3(P, O);
					FullIndex = FullPatternIndex(P, O, PatternD);
					m_weights[3][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternC,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex4(P, O);
					FullIndex = FullPatternIndex(P, O, PatternC);
					m_weights[4][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternHV,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex5(P, O);
					FullIndex = FullPatternIndex(P, O, PatternHV);
					m_weights[5][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternHD,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex6(P, O);
					FullIndex = FullPatternIndex(P, O, PatternHD);
					m_weights[6][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
			For_each_configuration_in_pattern_do_fkt(PatternHC,
				[&](uint64_t P, uint64_t O)
				{
					ReducedIndex = ReducedPatternIndex7(P, O);
					FullIndex = FullPatternIndex(P, O, PatternHC);
					m_weights[7][FullIndex] = compressed_weights[ReducedIndex];
				}
			);
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[0][FullPatternIndex(P, O, Pattern  )]
				 + m_weights[1][FullPatternIndex(P, O, PatternH )]
				 + m_weights[2][FullPatternIndex(P, O, PatternV )]
				 + m_weights[3][FullPatternIndex(P, O, PatternD )]
				 + m_weights[4][FullPatternIndex(P, O, PatternC )]
				 + m_weights[5][FullPatternIndex(P, O, PatternHV)]
				 + m_weights[6][FullPatternIndex(P, O, PatternHD)]
				 + m_weights[7][FullPatternIndex(P, O, PatternHC)];
		}
		inline float score(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			uint64_t diff = (new_P ^ old_P) | (new_O ^ old_O);

			return m_weights[0][(diff & Pattern  ) ? FullPatternIndex(new_P, new_O, Pattern  ) : *(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[1][(diff & PatternH ) ? FullPatternIndex(new_P, new_O, PatternH ) : *(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[2][(diff & PatternV ) ? FullPatternIndex(new_P, new_O, PatternV ) : *(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[3][(diff & PatternD ) ? FullPatternIndex(new_P, new_O, PatternD ) : *(Indices_P+3) + (*(Indices_O+3) << 1)]
				 + m_weights[4][(diff & PatternC ) ? FullPatternIndex(new_P, new_O, PatternC ) : *(Indices_P+4) + (*(Indices_O+4) << 1)]
				 + m_weights[5][(diff & PatternHV) ? FullPatternIndex(new_P, new_O, PatternHV) : *(Indices_P+5) + (*(Indices_O+5) << 1)]
				 + m_weights[6][(diff & PatternHD) ? FullPatternIndex(new_P, new_O, PatternHD) : *(Indices_P+6) + (*(Indices_O+6) << 1)]
				 + m_weights[7][(diff & PatternHC) ? FullPatternIndex(new_P, new_O, PatternHC) : *(Indices_P+7) + (*(Indices_O+7) << 1)];
		}
		inline float score(const std::vector<int>::const_iterator& Indices_P, const std::vector<int>::const_iterator& Indices_O) const
		{
			return m_weights[0][*(Indices_P+0) + (*(Indices_O+0) << 1)]
				 + m_weights[1][*(Indices_P+1) + (*(Indices_O+1) << 1)]
				 + m_weights[2][*(Indices_P+2) + (*(Indices_O+2) << 1)]
				 + m_weights[3][*(Indices_P+3) + (*(Indices_O+3) << 1)]
				 + m_weights[4][*(Indices_P+4) + (*(Indices_O+4) << 1)]
				 + m_weights[5][*(Indices_P+5) + (*(Indices_O+5) << 1)]
				 + m_weights[6][*(Indices_P+6) + (*(Indices_O+6) << 1)]
				 + m_weights[7][*(Indices_P+7) + (*(Indices_O+7) << 1)];
		}

		static void UpdateIndexVec(const uint64_t new_P, const uint64_t new_O, const uint64_t old_P, const uint64_t old_O, std::vector<int>::iterator Indices_P, std::vector<int>::iterator Indices_O)
		{
			uint64_t diff_P = new_P ^ old_P;
			uint64_t diff_O = new_O ^ old_O;
			if (diff_P & Pattern  ) *(Indices_P+0) = SumPow3[PExt(new_P, Pattern  )];
			if (diff_P & PatternH ) *(Indices_P+1) = SumPow3[PExt(new_P, PatternH )];
			if (diff_P & PatternV ) *(Indices_P+2) = SumPow3[PExt(new_P, PatternV )];
			if (diff_P & PatternD ) *(Indices_P+3) = SumPow3[PExt(new_P, PatternD )];
			if (diff_P & PatternC ) *(Indices_P+4) = SumPow3[PExt(new_P, PatternC )];
			if (diff_P & PatternHV) *(Indices_P+5) = SumPow3[PExt(new_P, PatternHV)];
			if (diff_P & PatternHD) *(Indices_P+6) = SumPow3[PExt(new_P, PatternHD)];
			if (diff_P & PatternHC) *(Indices_P+7) = SumPow3[PExt(new_P, PatternHC)];
			if (diff_O & Pattern  ) *(Indices_O+0) = SumPow3[PExt(new_O, Pattern  )];
			if (diff_O & PatternH ) *(Indices_O+1) = SumPow3[PExt(new_O, PatternH )];
			if (diff_O & PatternV ) *(Indices_O+2) = SumPow3[PExt(new_O, PatternV )];
			if (diff_O & PatternD ) *(Indices_O+3) = SumPow3[PExt(new_O, PatternD )];
			if (diff_O & PatternC ) *(Indices_O+4) = SumPow3[PExt(new_O, PatternC )];
			if (diff_O & PatternHV) *(Indices_O+5) = SumPow3[PExt(new_O, PatternHV)];
			if (diff_O & PatternHD) *(Indices_P+6) = SumPow3[PExt(new_O, PatternHD)];
			if (diff_O & PatternHC) *(Indices_P+7) = SumPow3[PExt(new_O, PatternHC)];
		}

		static void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec, int Offset = 0)
		{
			vec.push_back(ReducedPatternIndex0(P, O) + Offset);
			vec.push_back(ReducedPatternIndex1(P, O) + Offset);
			vec.push_back(ReducedPatternIndex2(P, O) + Offset);
			vec.push_back(ReducedPatternIndex3(P, O) + Offset);
			vec.push_back(ReducedPatternIndex4(P, O) + Offset);
			vec.push_back(ReducedPatternIndex5(P, O) + Offset);
			vec.push_back(ReducedPatternIndex6(P, O) + Offset);
			vec.push_back(ReducedPatternIndex7(P, O) + Offset);
		}
	};


	const int ReducedSize = 0
		+ (PATTERN_L02X  ? CPattern_L02X ::ReducedSize : 0)
		+ (PATTERN_L1    ? CPattern_L1   ::ReducedSize : 0)
		+ (PATTERN_L2    ? CPattern_L2   ::ReducedSize : 0)
		+ (PATTERN_L3    ? CPattern_L3   ::ReducedSize : 0)
		+ (PATTERN_D4    ? CPattern_D4   ::ReducedSize : 0)
		+ (PATTERN_D5    ? CPattern_D5   ::ReducedSize : 0)
		+ (PATTERN_D6    ? CPattern_D6   ::ReducedSize : 0)
		+ (PATTERN_D7    ? CPattern_D7   ::ReducedSize : 0)
		+ (PATTERN_Comet ? CPattern_Comet::ReducedSize : 0)
		+ (PATTERN_Eplus ? CPattern_Eplus::ReducedSize : 0)
		+ (PATTERN_C3p1  ? CPattern_C3p1 ::ReducedSize : 0)
		+ (PATTERN_C3p2  ? CPattern_C3p2 ::ReducedSize : 0)
		+ (PATTERN_Q0    ? CPattern_Q0   ::ReducedSize : 0)
		+ (PATTERN_B5    ? CPattern_B5   ::ReducedSize : 0);
	const int NumberOfPattern = 0
		+ (PATTERN_L02X ? 1 : 0)
		+ (PATTERN_L1   ? 1 : 0)
		+ (PATTERN_L2   ? 1 : 0)
		+ (PATTERN_L3   ? 1 : 0)
		+ (PATTERN_D4   ? 1 : 0)
		+ (PATTERN_D5   ? 1 : 0)
		+ (PATTERN_D6   ? 1 : 0)
		+ (PATTERN_D7   ? 1 : 0)
		+ (PATTERN_Comet? 1 : 0)
		+ (PATTERN_Eplus? 1 : 0)
		+ (PATTERN_C3p1 ? 1 : 0)
		+ (PATTERN_C3p2 ? 1 : 0)
		+ (PATTERN_Q0   ? 1 : 0)
		+ (PATTERN_B5   ? 1 : 0);
}


int EvaluateFeatures(const uint64_t P, const uint64_t O);
int EvaluateFeatures(const uint64_t P, const uint64_t O, std::vector<float>& scores);
int EvaluateFeatures(const uint64_t P, const uint64_t O, const Features::CIndexArray& Indices_P, const Features::CIndexArray& Indices_O);
int EvaluateFeatures(const uint64_t empties, const Features::CIndexArray& Indices_P, const Features::CIndexArray& Indices_O);
void UpdateIndexVec(const uint64_t P, const uint64_t O, Features::CIndexArray& Indices_P, Features::CIndexArray& Indices_O);

void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec);
void FillConfigurationVecOffsetted(const uint64_t P, const uint64_t O, std::vector<int>& vec);

//bool Test_All_Features();
