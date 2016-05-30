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

namespace Features
{
	extern unsigned int NumberOfPatternWithSymmetrie;
	extern unsigned int ReducedSize;
	extern unsigned int NumberOfPattern;
		
	extern unsigned int SumPow3[32768];

	inline unsigned int FullPatternIndex(const uint64_t P, const uint64_t O, const uint64_t mask)
	{ 
		assert(PopCount(mask) <= 15);
		return SumPow3[PExt(P, mask)] + (SumPow3[PExt(O, mask)] << 1); 
	}

	void For_each_configuration_in_pattern_do_fkt(const uint64_t Pattern, std::function<void(const uint64_t, const uint64_t)> fkt);
	void Initialize();

	class CPattern
	{
	public:
		virtual int occurrences() = 0;
		virtual int FullSize() = 0;
		virtual int ReducedSize() = 0;
		
		virtual void set_weights(const std::vector<float>& compressed_weights) = 0;
		virtual inline float score(const uint64_t P, const uint64_t O) const = 0;
		virtual inline void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>::iterator& it, int Offset = 0) const = 0;
	};

	class CPatternH : public CPattern
	{
		static const uint64_t HALF = 0x0F0F0F0F0F0F0F0FULL;
		static const uint64_t MID  = 0x0000001818000000ULL;
		const uint64_t Pattern, PatternC, PatternV, PatternD;
		const int halfSize, m_FullSize, m_ReducedSize;
		std::vector<std::vector<float>> m_weights; //m_weights[Index][FullIndex]

		int ReducedPatternIndex0(const uint64_t P, const uint64_t O) const
		{
			const uint64_t P_h = FlipHorizontal(P);
			const uint64_t O_h = FlipHorizontal(O);
			const int indexA = (FullPatternIndex(P  , O  , Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + PExt(O  , Pattern & HALF & MID);
			const int indexB = (FullPatternIndex(P_h, O_h, Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + PExt(O_h, Pattern & HALF & MID);
			return (indexA > indexB) ? 
				(indexB * halfSize + indexA - (indexB * indexB + indexB >> 1)) : 
				(indexA * halfSize + indexB - (indexA * indexA + indexA >> 1));
		}
		int ReducedPatternIndex1(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		int ReducedPatternIndex2(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }
		int ReducedPatternIndex3(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipDiagonal(P), FlipDiagonal(O)); }

	public:
		int occurrences() { return 4; }
		int FullSize() { return m_FullSize; }
		int ReducedSize() { return m_ReducedSize; }
		
		CPatternH(const uint64_t Pattern = 0)
			: Pattern(Pattern),
			PatternC(FlipCodiagonal(Pattern)),
			PatternV(FlipVertical(Pattern)),
			PatternD(FlipDiagonal(Pattern)),
			m_FullSize(_Pow_int(3, PopCount(Pattern))),
			halfSize(_Pow_int(3, PopCount(Pattern & HALF & ~MID)) * _Pow_int(2, PopCount(Pattern & HALF & MID))),
			m_ReducedSize(halfSize * (halfSize + 1) / 2)
		{
			assert(Pattern == FlipHorizontal(Pattern));
			m_weights.resize(4);
			for (int i = 0; i < 4; ++i) m_weights[i].resize(m_FullSize);
		}
		void set_weights(const std::vector<float>& compressed_weights)
		{
			For_each_configuration_in_pattern_do_fkt(Pattern , [&](uint64_t P, uint64_t O) { m_weights[0][FullPatternIndex(P, O, Pattern )] = compressed_weights[ReducedPatternIndex0(P, O)]; });
			For_each_configuration_in_pattern_do_fkt(PatternC, [&](uint64_t P, uint64_t O) { m_weights[1][FullPatternIndex(P, O, PatternC)] = compressed_weights[ReducedPatternIndex1(P, O)]; });
			For_each_configuration_in_pattern_do_fkt(PatternV, [&](uint64_t P, uint64_t O) { m_weights[2][FullPatternIndex(P, O, PatternV)] = compressed_weights[ReducedPatternIndex2(P, O)]; });
			For_each_configuration_in_pattern_do_fkt(PatternD, [&](uint64_t P, uint64_t O) { m_weights[3][FullPatternIndex(P, O, PatternD)] = compressed_weights[ReducedPatternIndex3(P, O)]; });
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[0][FullPatternIndex(P, O, Pattern )]
				 + m_weights[1][FullPatternIndex(P, O, PatternC)]
				 + m_weights[2][FullPatternIndex(P, O, PatternV)]
				 + m_weights[3][FullPatternIndex(P, O, PatternD)];
		}

		inline void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>::iterator& it, int Offset = 0) const
		{
			*(it++) = ReducedPatternIndex0(P, O) + Offset;
			*(it++) = ReducedPatternIndex1(P, O) + Offset;
			*(it++) = ReducedPatternIndex2(P, O) + Offset;
			*(it++) = ReducedPatternIndex3(P, O) + Offset;
		}
	};
	
	class CPatternD : public CPattern
	{
		static const uint64_t HALF = 0x0080C0E0F0F8FCFEULL;
		static const uint64_t DIAG = 0x8040201008040201ULL;
		static const uint64_t MID  = 0x0000001818000000ULL;
		const uint64_t Pattern, PatternH, PatternC, PatternV;
		const int halfSize, diagSize, m_FullSize, m_ReducedSize;
		std::vector<std::vector<float>> m_weights; //m_weights[Index][FullIndex]

		int ReducedPatternIndex0(const uint64_t P, const uint64_t O) const
		{
			const uint64_t P_d = FlipDiagonal(P);
			const uint64_t O_d = FlipDiagonal(O);
			const int indexA = (FullPatternIndex(P  , O  , Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + PExt(O  , Pattern & HALF & MID);
			const int indexB = (FullPatternIndex(P_d, O_d, Pattern & HALF & ~MID) << PopCount(Pattern & HALF & MID)) + PExt(O_d, Pattern & HALF & MID);
			const int indexD = (FullPatternIndex(P  , O  , Pattern & DIAG & ~MID) << PopCount(Pattern & DIAG & MID)) + PExt(O  , Pattern & DIAG & MID);
			return (indexA > indexB) ? 
				(diagSize * (indexB * halfSize + indexA - (indexB * indexB + indexB >> 1)) + indexD) : 
				(diagSize * (indexA * halfSize + indexB - (indexA * indexA + indexA >> 1)) + indexD);
		}
		int ReducedPatternIndex1(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipHorizontal(P), FlipHorizontal(O)); }
		int ReducedPatternIndex2(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		int ReducedPatternIndex3(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }

	public:
		int occurrences() { return 4; }
		int FullSize() { return m_FullSize; }
		int ReducedSize() { return m_ReducedSize; }

		CPatternD(const uint64_t Pattern = 0)
			: Pattern(Pattern),
			PatternH(FlipHorizontal(Pattern)),
			PatternC(FlipCodiagonal(Pattern)),
			PatternV(FlipVertical(Pattern)),
			m_FullSize(_Pow_int(3, PopCount(Pattern))),
			halfSize(_Pow_int(3, PopCount(Pattern & HALF & ~MID)) * _Pow_int(2, PopCount(Pattern & HALF & MID))),
			diagSize(_Pow_int(3, PopCount(Pattern & DIAG & ~MID)) * _Pow_int(2, PopCount(Pattern & DIAG & MID))),
			m_ReducedSize(diagSize * halfSize * (halfSize + 1) / 2)
		{
			assert(Pattern == FlipDiagonal(Pattern));
			m_weights.resize(4);
			for (int i = 0; i < 4; ++i) m_weights[i].resize(m_FullSize);
		}
		void set_weights(const std::vector<float>& compressed_weights)
		{
			For_each_configuration_in_pattern_do_fkt(Pattern , [&](uint64_t P, uint64_t O) { m_weights[0][FullPatternIndex(P, O, Pattern )] = compressed_weights[ReducedPatternIndex0(P, O)]; });
			For_each_configuration_in_pattern_do_fkt(PatternH, [&](uint64_t P, uint64_t O) { m_weights[1][FullPatternIndex(P, O, PatternH)] = compressed_weights[ReducedPatternIndex1(P, O)]; });
			For_each_configuration_in_pattern_do_fkt(PatternC, [&](uint64_t P, uint64_t O) { m_weights[2][FullPatternIndex(P, O, PatternC)] = compressed_weights[ReducedPatternIndex2(P, O)]; });
			For_each_configuration_in_pattern_do_fkt(PatternV, [&](uint64_t P, uint64_t O) { m_weights[3][FullPatternIndex(P, O, PatternV)] = compressed_weights[ReducedPatternIndex3(P, O)]; });
		}

		inline float score(const uint64_t P, const uint64_t O) const
		{
			return m_weights[0][FullPatternIndex(P, O, Pattern )]
				 + m_weights[1][FullPatternIndex(P, O, PatternH)]
				 + m_weights[2][FullPatternIndex(P, O, PatternC)]
				 + m_weights[3][FullPatternIndex(P, O, PatternV)];
		}

		inline void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>::iterator& it, int Offset = 0) const
		{
			*(it++) = ReducedPatternIndex0(P, O) + Offset;
			*(it++) = ReducedPatternIndex1(P, O) + Offset;
			*(it++) = ReducedPatternIndex2(P, O) + Offset;
			*(it++) = ReducedPatternIndex3(P, O) + Offset;
		}
	};
	
	class CPattern0 : public CPattern
	{
		static const uint64_t MID = 0x0000001818000000ULL;
		const uint64_t Pattern, PatternH, PatternV, PatternD, PatternC, PatternHV, PatternHD, PatternHC;
		const int m_FullSize, m_ReducedSize;
		std::vector<std::vector<float>> m_weights; //m_weights[Index][FullIndex]

		int ReducedPatternIndex0(const uint64_t P, const uint64_t O) const { return FullPatternIndex(P, O, Pattern); }
		int ReducedPatternIndex1(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipHorizontal(P), FlipHorizontal(O)); }
		int ReducedPatternIndex2(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipVertical(P), FlipVertical(O)); }
		int ReducedPatternIndex3(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipDiagonal(P), FlipDiagonal(O)); }
		int ReducedPatternIndex4(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipCodiagonal(P), FlipCodiagonal(O)); }
		int ReducedPatternIndex5(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipHorizontal(FlipVertical(P)), FlipHorizontal(FlipVertical(O))); }
		int ReducedPatternIndex6(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipHorizontal(FlipDiagonal(P)), FlipHorizontal(FlipDiagonal(O))); }
		int ReducedPatternIndex7(const uint64_t P, const uint64_t O) const { return ReducedPatternIndex0(FlipHorizontal(FlipCodiagonal(P)), FlipHorizontal(FlipCodiagonal(O))); }

	public:
		int occurrences() { return 8; }
		int FullSize() { return m_FullSize; }
		int ReducedSize() { return m_ReducedSize; }

		CPattern0(const uint64_t Pattern = 0)
			: Pattern(Pattern),
			PatternH(FlipHorizontal(Pattern)),
			PatternV(FlipVertical(Pattern)),
			PatternD(FlipDiagonal(Pattern)),
			PatternC(FlipCodiagonal(Pattern)),
			PatternHV(FlipVertical(FlipHorizontal(Pattern))),
			PatternHD(FlipDiagonal(FlipHorizontal(Pattern))),
			PatternHC(FlipCodiagonal(FlipHorizontal(Pattern))),
			m_FullSize(_Pow_int(3, PopCount(Pattern))),
			m_ReducedSize(_Pow_int(3, PopCount(Pattern & ~MID)) * _Pow_int(2, PopCount(Pattern & MID)))
		{
			//Initialize weights
			m_weights.resize(8);
			for (int i = 0; i < 8; ++i) m_weights[i].resize(m_FullSize);
		}
		void set_weights(const std::vector<float>& compressed_weights)
		{
			For_each_configuration_in_pattern_do_fkt(Pattern  , [&](uint64_t P, uint64_t O) { m_weights[0][FullPatternIndex(P, O, Pattern  )] = compressed_weights[ReducedPatternIndex0(P, O)]; });
			For_each_configuration_in_pattern_do_fkt(PatternH , [&](uint64_t P, uint64_t O) { m_weights[1][FullPatternIndex(P, O, PatternH )] = compressed_weights[ReducedPatternIndex1(P, O)]; });
			For_each_configuration_in_pattern_do_fkt(PatternV , [&](uint64_t P, uint64_t O) { m_weights[2][FullPatternIndex(P, O, PatternV )] = compressed_weights[ReducedPatternIndex2(P, O)]; });
			For_each_configuration_in_pattern_do_fkt(PatternD , [&](uint64_t P, uint64_t O) { m_weights[3][FullPatternIndex(P, O, PatternD )] = compressed_weights[ReducedPatternIndex3(P, O)]; });
			For_each_configuration_in_pattern_do_fkt(PatternC , [&](uint64_t P, uint64_t O) { m_weights[4][FullPatternIndex(P, O, PatternC )] = compressed_weights[ReducedPatternIndex4(P, O)]; });
			For_each_configuration_in_pattern_do_fkt(PatternHV, [&](uint64_t P, uint64_t O) { m_weights[5][FullPatternIndex(P, O, PatternHV)] = compressed_weights[ReducedPatternIndex5(P, O)]; });
			For_each_configuration_in_pattern_do_fkt(PatternHD, [&](uint64_t P, uint64_t O) { m_weights[6][FullPatternIndex(P, O, PatternHD)] = compressed_weights[ReducedPatternIndex6(P, O)]; });
			For_each_configuration_in_pattern_do_fkt(PatternHC, [&](uint64_t P, uint64_t O) { m_weights[7][FullPatternIndex(P, O, PatternHC)] = compressed_weights[ReducedPatternIndex7(P, O)]; });
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
		
		inline void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>::iterator& it, int Offset = 0) const
		{
			*(it++) = ReducedPatternIndex0(P, O) + Offset;
			*(it++) = ReducedPatternIndex1(P, O) + Offset;
			*(it++) = ReducedPatternIndex2(P, O) + Offset;
			*(it++) = ReducedPatternIndex3(P, O) + Offset;
			*(it++) = ReducedPatternIndex4(P, O) + Offset;
			*(it++) = ReducedPatternIndex5(P, O) + Offset;
			*(it++) = ReducedPatternIndex6(P, O) + Offset;
			*(it++) = ReducedPatternIndex7(P, O) + Offset;
		}
	};
}


int EvaluateFeatures(const uint64_t P, const uint64_t O);
int EvaluateFeatures(const uint64_t P, const uint64_t O, std::vector<float>& scores);

void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>::iterator& it);
void FillConfigurationVecOffsetted(const uint64_t P, const uint64_t O, std::vector<int>::iterator& it);

//bool Test_All_Features();
