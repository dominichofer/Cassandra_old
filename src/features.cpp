#include "features.h"

namespace Features
{
	const int BOXES = 16;

	unsigned int NumberOfPatternWithSymmetrie;
	unsigned int ReducedSize;
	unsigned int NumberOfPattern;
	unsigned int SumPow3[32768]; // OPTIMIZATION: Replace look-up by small calculation if possible
	
	std::vector<std::vector<CPattern*>> PatternVec; //PatternVec[box_index][pattern_index]

	uint64_t sumpow3(uint64_t exp)
	{
		int64_t sum = 0;
		while (exp)	{
			sum += _Pow_int(3ULL, BitScanLSB(exp));
			RemoveLSB(exp);
		}
		return sum;
	}

	void For_each_configuration_in_pattern_do_fkt(const uint64_t Pattern, std::function<void(const uint64_t, const uint64_t)> fkt)
	{
		uint64_t P, O;
		const uint64_t PatternSize = 1ULL << PopCount(Pattern);
		const uint64_t ExtractedCenter = PExt(0x0000001818000000ULL, Pattern);
		for (uint64_t i = 0; i < PatternSize; i++)
		{
			P = PDep(i, Pattern);
			for (uint64_t j = 0; j < PatternSize; j++)
			{
				if (i & j) continue;
				if (((i | j) & ExtractedCenter) != ExtractedCenter) continue;
				O = PDep(j, Pattern);

				fkt(P, O);
			}
		}
	}

	CPattern* NewPattern(const uint64_t Pattern)
	{
		if (Pattern == FlipHorizontal(Pattern)) return new CPatternH(Pattern);
		if (Pattern == FlipDiagonal  (Pattern)) return new CPatternD(Pattern);
		return new CPattern0(Pattern);
	}

	std::vector<uint64_t> GetActivePattern()
	{
		std::vector<uint64_t> ActivePattern;
		std::vector<std::string> ActivePatternNames;
		std::string PatternString = ConfigFile::Configurations["active pattern"];
		std::string delimiter = " ";
		std::string token;
		size_t pos = 0;
		while ((pos = PatternString.find(delimiter)) != std::string::npos)
		{
			token = PatternString.substr(0, pos);
			ActivePatternNames.push_back(PatternString.substr(0, pos));
			PatternString.erase(0, pos + delimiter.length());
		}

		for (auto& it : ActivePatternNames)
		{
			std::string s = "pattern " + it;
			if (ConfigFile::Configurations.count(s) > 0)
				ActivePattern.push_back(std::stoull(ConfigFile::Configurations[s], 0, 16));
			else
				std::cerr << "ERROR: Pattern '" << it << "' not defined." << std::endl;
		}

		return ActivePattern;
	}

	void Initialize()
	{
		for (int i = 0; i < 32768; i++) SumPow3[i] = sumpow3(i);
		
		std::vector<uint64_t> ActivePattern = GetActivePattern();

		std::cout << std::hex;
		for (auto& it : ActivePattern)
			std::cout << "Using Pattern " << it << std::endl;
		std::cout << std::dec;

		PatternVec.resize(BOXES);
		for (int i = 0; i < BOXES; ++i)
			for (auto& it : ActivePattern)
					PatternVec[i].push_back(NewPattern(it));

		NumberOfPatternWithSymmetrie = 0;
		for (auto& pat : Features::PatternVec[0]) NumberOfPatternWithSymmetrie += pat->occurrences();

		ReducedSize = 0;
		for (auto& pat : Features::PatternVec[0]) ReducedSize += pat->ReducedSize();

		NumberOfPattern = Features::PatternVec[0].size();

		// Extract filenames form config file
		std::string s;
		std::vector<std::string> Filenames;
		for (int i = 0; i < BOXES; ++i)
		{
			s = "weights range " + std::to_string(i);
			if (ConfigFile::Configurations.count(s) > 0)
				Filenames.push_back(ConfigFile::Configurations["filepath"] + ConfigFile::Configurations[s]);
		}

		// Read in weight files
		std::vector<double> v_double;
		for (std::size_t j = 0; j < Filenames.size(); ++j)
		{
			v_double = read_vector<double>(Filenames[j]);
			std::vector<float> v_float(v_double.begin(), v_double.end());
			if (v_float.size() != ReducedSize){
				std::cerr << "ERROR: Weight's file " << Filenames[j] << " size (" << v_float.size() << ") does not match expected size (" << ReducedSize << ")." << std::endl;
				continue;
			}
			for (auto& pat : PatternVec[j])
			{
				pat->set_weights(v_float);
				v_float.erase(v_float.begin(), v_float.begin() + pat->ReducedSize());
			}
			if (v_float.size() != 0) std::cerr << "ERROR: filesizes are wrong!" << std::endl;
		}
	}
}

int EvaluateFeatures(const uint64_t P, const uint64_t O) // OPTIMIZATION: create a function taking "empties" as a variable
{
	const int empties = Empties(P, O);
	if (empties == 0) return (PopCount(P) << 1) - 64;
	const int BoxIndex = MIN((empties - 1) / 3, Features::BOXES - 1);
	float sum = 0;
	for (auto& pat : Features::PatternVec[BoxIndex])
		sum += pat->score(P, O);
	return BIND(RoundInt(sum), -64, 64);
}
int EvaluateFeatures(const uint64_t P, const uint64_t O, std::vector<float>& scores)
{
	const int BoxIndex = MIN(static_cast<int>(Empties(P, O) - 1) / 3, Features::BOXES - 1);
	scores.clear();
	for (auto& pat : Features::PatternVec[BoxIndex])
		scores.push_back(pat->score(P, O));
	float sum = 0;
	for (const auto& it : scores) sum += it;
	return RoundInt(sum);
}

void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>::iterator& it)
{
	for (auto& pat : Features::PatternVec[0])
		pat->FillConfigurationVec(P, O, it);
}
void FillConfigurationVecOffsetted(const uint64_t P, const uint64_t O, std::vector<int>::iterator& it)
{
	int Offset = 0;
	for (auto& pat : Features::PatternVec[0])
	{
		pat->FillConfigurationVec(P, O, it, Offset); 
		Offset += pat->ReducedSize();
	}
}

//bool Congruent(const uint64_t P1, const uint64_t P2)
//{
//	if (P1 == P2) return true;
//	if (P1 == horizontal_flip(P2)) return true;
//	if (P1 == vertical_flip(P2)) return true;
//	if (P1 == diagonal_flip(P2)) return true;
//	if (P1 == codiagonal_flip(P2)) return true;
//	if (P1 == vertical_flip(horizontal_flip(P2))) return true;
//	if (P1 == diagonal_flip(horizontal_flip(P2))) return true;
//	if (P1 == codiagonal_flip(horizontal_flip(P2))) return true;
//	return false;
//}
//
//bool Test_Patterns_PopCount(const int symmetries, const uint64_t * const Pattern)
//{
//	bool Success = true;
//	uint64_t * POPCOUNT = new uint64_t[symmetries];
//
//	for (int i = 0; i < symmetries; i++)
//		POPCOUNT[i] = PopCount(Pattern[i]);
//	for (int i = 0; i < symmetries; i++)
//	{
//		if (POPCOUNT[0] != POPCOUNT[i]){
//			std::cerr << "ERROR: Pattern are inconsistent!" << std::endl;
//			Success = false;
//		}
//	}
//
//	delete[] POPCOUNT;
//	return Success;
//}
//
//bool Test_Pattern_Congruence(const int symmetries, const uint64_t * const Pattern)
//{
//	bool Success = true;
//
//	for (int i = 0; i < symmetries; i++)
//	{
//		if (!Congruent(Pattern[0], Pattern[i])){
//			std::cerr << "ERROR: Pattern are not congruent!" << std::endl;
//			Success = false;
//		}
//	}
//
//	return Success;
//}
//
//void For_each_configuration_in_pattern_do_fkt(const uint64_t Pattern, std::function<void(const uint64_t, const uint64_t)> fkt)
//{
//	uint64_t P, O;
//	const uint64_t PatternSize = 1ULL << PopCount(Pattern);
//	const uint64_t ExtractedCenter = PExt(0x0000001818000000ULL, Pattern);
//	for (uint64_t i = 0; i < PatternSize; i++)
//	{
//		P = PDep(i, Pattern);
//		for (uint64_t j = 0; j < PatternSize; j++)
//		{
//			if (i & j) continue;
//			if (((i | j) & ExtractedCenter) != ExtractedCenter) continue;
//			O = PDep(j, Pattern);
//
//			fkt(P, O);
//		}
//	}
//
//}
//
//bool Test_Index_Range(const int symmetries, const int size, uint64_t * Pattern, int(*Feature[])(const uint64_t P, const uint64_t O))
//{
//	bool Success = true;
//
//	for (int k = 0; k < symmetries; k++)
//	{
//		For_each_configuration_in_pattern_do_fkt(Pattern[k],
//			[Feature, k, size, &Success](const uint64_t P, const uint64_t O)
//				{
//					int index = Feature[k](P, O);
//					if (index < 0){
//						std::cerr << "ERROR: Index is too small!" << std::endl;
//						Success = false;
//					}
//					if (index > size){
//						std::cerr << "ERROR: Index is too big!" << std::endl;
//						Success = false;
//					}
//					return ;
//				}
//		);
//	}
//
//	return Success;
//}
//
//bool Test_Feature_symmetrie(const int symmetries, uint64_t * Pattern, int(*Feature[])(const uint64_t P, const uint64_t O))
//{
//	bool Success = true;
//
//	For_each_configuration_in_pattern_do_fkt(Pattern[0],
//		[Feature, symmetries, Pattern, &Success](const uint64_t P, const uint64_t O)
//			{
//				int indexA = Feature[0](P, O);
//				int indexB;
//				for (int k = 0; k < symmetries; ++k)
//				{
//					if (Pattern[k] == horizontal_flip(Pattern[0])){
//						indexB = Feature[k](horizontal_flip(P), horizontal_flip(O));
//						if (indexA != indexB){
//							std::cerr << "ERROR: Indices don't match!" << std::endl;
//							Success = false;
//						}
//					}
//					if (Pattern[k] == vertical_flip(Pattern[0])){
//						indexB = Feature[k](vertical_flip(P), vertical_flip(O));
//						if (indexA != indexB){
//							std::cerr << "ERROR: Indices don't match!" << std::endl;
//							Success = false;
//						}
//					}
//					if (Pattern[k] == diagonal_flip(Pattern[0])){
//						indexB = Feature[k](diagonal_flip(P), diagonal_flip(O));
//						if (indexA != indexB){
//							std::cerr << "ERROR: Indices don't match!" << std::endl;
//							Success = false;
//						}
//					}
//					if (Pattern[k] == codiagonal_flip(Pattern[0])){
//						indexB = Feature[k](codiagonal_flip(P), codiagonal_flip(O));
//						if (indexA != indexB){
//							std::cerr << "ERROR: Indices don't match!" << std::endl;
//							Success = false;
//						}
//					}
//					if (Pattern[k] == vertical_flip(horizontal_flip(Pattern[0]))){
//						indexB = Feature[k](vertical_flip(horizontal_flip(P)), vertical_flip(horizontal_flip(O)));
//						if (indexA != indexB){
//							std::cerr << "ERROR: Indices don't match!" << std::endl;
//							Success = false;
//						}
//					}
//					if (Pattern[k] == diagonal_flip(horizontal_flip(Pattern[0]))){
//						indexB = Feature[k](diagonal_flip(horizontal_flip(P)), diagonal_flip(horizontal_flip(O)));
//						if (indexA != indexB){
//							std::cerr << "ERROR: Indices don't match!" << std::endl;
//							Success = false;
//						}
//					}
//					if (Pattern[k] == codiagonal_flip(horizontal_flip(Pattern[0]))){
//						indexB = Feature[k](codiagonal_flip(horizontal_flip(P)), codiagonal_flip(horizontal_flip(O)));
//						if (indexA != indexB){
//							std::cerr << "ERROR: Indices don't match!" << std::endl;
//							Success = false;
//						}
//					}
//				}
//			}
//	);
//
//	return Success;
//}
//
//bool Test_Feature_symmetrie_FullIndex(int * tmpWeights, const int symmetries, uint64_t * Pattern, int(*Feature[])(const uint64_t P, const uint64_t O), int Offset)
//{
//	bool Success = true;
//
//	For_each_configuration_in_pattern_do_fkt(Pattern[0],
//		[&](const uint64_t P, const uint64_t O)
//			{
//				auto err = [&]{
//					std::cerr << "ERROR: Indices don't match!" << std::endl;
//					std::cerr << board2D(P, O) << "\n";
//					Success = false;
//				};
//				int indexA, indexB;
//				int weightA = tmpWeights[indexA = Offset + Feature[0](P, O)];
//				int weightB;
//				for (int k = 0; k < symmetries; ++k)
//				{
//					if (Pattern[k] == horizontal_flip(Pattern[0])){
//						weightB = tmpWeights[indexB = Offset + Feature[k](horizontal_flip(P), horizontal_flip(O))];
//						if (weightA != weightB) err();
//					}
//					if (Pattern[k] == vertical_flip(Pattern[0])){
//						weightB = tmpWeights[indexB = Offset + Feature[k](vertical_flip(P), vertical_flip(O))];
//						if (weightA != weightB) err();
//					}
//					if (Pattern[k] == diagonal_flip(Pattern[0])){
//						weightB = tmpWeights[indexB = Offset + Feature[k](diagonal_flip(P), diagonal_flip(O))];
//						if (weightA != weightB) err();
//					}
//					if (Pattern[k] == codiagonal_flip(Pattern[0])){
//						weightB = tmpWeights[indexB = Offset + Feature[k](codiagonal_flip(P), codiagonal_flip(O))];
//						if (weightA != weightB) err();
//					}
//					if (Pattern[k] == vertical_flip(horizontal_flip(Pattern[0]))){
//						weightB = tmpWeights[indexB = Offset + Feature[k](vertical_flip(horizontal_flip(P)), vertical_flip(horizontal_flip(O)))];
//						if (weightA != weightB) err();
//					}
//					if (Pattern[k] == diagonal_flip(horizontal_flip(Pattern[0]))){
//						weightB = tmpWeights[indexB = Offset + Feature[k](diagonal_flip(horizontal_flip(P)), diagonal_flip(horizontal_flip(O)))];
//						if (weightA != weightB) err();
//					}
//					if (Pattern[k] == codiagonal_flip(horizontal_flip(Pattern[0]))){
//						weightB = tmpWeights[indexB = Offset + Feature[k](codiagonal_flip(horizontal_flip(P)), codiagonal_flip(horizontal_flip(O)))];
//						if (weightA != weightB) err();
//					}
//				}
//			}
//	);
//
//	return Success;
//}
//
//bool Test_Index_Covering(const int symmetries, const int size, uint64_t * Pattern, int(*Feature[])(const uint64_t P, const uint64_t O))
//{
//	bool Success = true;
//	int * taken = new int[size];
//
//	for (int k = 0; k < symmetries; k++)
//	{
//		for (int i = 0; i < size; i++)
//			taken[i] = 0;
//
//		For_each_configuration_in_pattern_do_fkt(Pattern[k],
//			[Feature, k, &taken](const uint64_t P, const uint64_t O){ taken[Feature[k](P, O)]++; }
//		);
//
//		for (int i = 0; i < size; i++)
//		{
//			if (taken[i] == 0){
//				std::cerr << "ERROR: Not every index is covered! Index " << i << " is never taken." << std::endl;
//				Success = false;
//			}
//		}
//	}
//	return Success;
//}
//
//bool Test(const int symmetries, const int size, uint64_t * Pattern, int(*Feature[])(const uint64_t P, const uint64_t O), bool IsPattern)
//{
//	using namespace Features;
//	bool Success = true;
//	const uint64_t POPCOUNT = PopCount(Pattern[0]);
//	uint64_t * BitConfig = new uint64_t[POPCOUNT];
//
//	// Test patern's population count to be equal
//	Success &= Test_Patterns_PopCount(symmetries, Pattern);
//
//	// Test congruence of patterns
//	Success &= Test_Pattern_Congruence(symmetries, Pattern);
//
//	// Test the range of all the features
//	Success &= Test_Index_Range(symmetries, size, Pattern, Feature);
//
//	// Test if all the symmetries of all configurations of all the features result in the same index
//	Success &= Test_Feature_symmetrie(symmetries, Pattern, Feature);
//
//	// Test if all possible indices are taken at least once
//	if (IsPattern)
//		Success &= Test_Index_Covering(symmetries, size, Pattern, Feature);
//
//	return Success;
//}
//
//bool Test_All_Features()
//{
//	using namespace Features;
//	bool Success = true;
//	int * tmpWeights = new int[FullSize];
//	int ReducedOffset = 0;
//	int FullOffset = 0;
//	For_each_configuration_in_pattern_do_fkt(Pattern_LowerC      , [&](uint64_t P, uint64_t O){	tmpWeights[FullIndex_LowerC      (P, O) + FullOffset] = ReducedIndex_LowerC      (P, O) + ReducedOffset; }); ReducedOffset += ReducedSize_C ; FullOffset += FullSize_C ;
//	For_each_configuration_in_pattern_do_fkt(Pattern_LowerL1     , [&](uint64_t P, uint64_t O){ tmpWeights[FullIndex_LowerL1     (P, O) + FullOffset] = ReducedIndex_LowerL1     (P, O) + ReducedOffset; }); ReducedOffset += ReducedSize_L1; FullOffset += FullSize_L1;
//	For_each_configuration_in_pattern_do_fkt(Pattern_LowerL2     , [&](uint64_t P, uint64_t O){ tmpWeights[FullIndex_LowerL2     (P, O) + FullOffset] = ReducedIndex_LowerL2     (P, O) + ReducedOffset; }); ReducedOffset += ReducedSize_L2; FullOffset += FullSize_L2;
//	For_each_configuration_in_pattern_do_fkt(Pattern_LowerL3     , [&](uint64_t P, uint64_t O){ tmpWeights[FullIndex_LowerL3     (P, O) + FullOffset] = ReducedIndex_LowerL3     (P, O) + ReducedOffset; }); ReducedOffset += ReducedSize_L3; FullOffset += FullSize_L3;
//	For_each_configuration_in_pattern_do_fkt(Pattern_RightLowerA , [&](uint64_t P, uint64_t O){ tmpWeights[FullIndex_RightLowerA (P, O) + FullOffset] = ReducedIndex_RightLowerA (P, O) + ReducedOffset; }); ReducedOffset += ReducedSize_A ; FullOffset += FullSize_A ;
//	For_each_configuration_in_pattern_do_fkt(Pattern_RightLowerSq, [&](uint64_t P, uint64_t O){ tmpWeights[FullIndex_RightLowerSq(P, O) + FullOffset] = ReducedIndex_RightLowerSq(P, O) + ReducedOffset; }); ReducedOffset += ReducedSize_Sq; FullOffset += FullSize_Sq;
//	For_each_configuration_in_pattern_do_fkt(Pattern_LowerRightB , [&](uint64_t P, uint64_t O){ tmpWeights[FullIndex_LowerRightB (P, O) + FullOffset] = ReducedIndex_LowerRightB (P, O) + ReducedOffset; }); ReducedOffset += ReducedSize_B ; FullOffset += FullSize_B ;
//	For_each_configuration_in_pattern_do_fkt(Pattern_RightLowerD4, [&](uint64_t P, uint64_t O){ tmpWeights[FullIndex_RightLowerD4(P, O) + FullOffset] = ReducedIndex_RightLowerD4(P, O) + ReducedOffset; }); ReducedOffset += ReducedSize_D4; FullOffset += FullSize_D4;
//	For_each_configuration_in_pattern_do_fkt(Pattern_RightLowerD5, [&](uint64_t P, uint64_t O){ tmpWeights[FullIndex_RightLowerD5(P, O) + FullOffset] = ReducedIndex_RightLowerD5(P, O) + ReducedOffset; }); ReducedOffset += ReducedSize_D5; FullOffset += FullSize_D5;
//	For_each_configuration_in_pattern_do_fkt(Pattern_RightLowerD6, [&](uint64_t P, uint64_t O){ tmpWeights[FullIndex_RightLowerD6(P, O) + FullOffset] = ReducedIndex_RightLowerD6(P, O) + ReducedOffset; }); ReducedOffset += ReducedSize_D6; FullOffset += FullSize_D6;
//	For_each_configuration_in_pattern_do_fkt(Pattern_RightLowerD7, [&](uint64_t P, uint64_t O){ tmpWeights[FullIndex_RightLowerD7(P, O) + FullOffset] = ReducedIndex_RightLowerD7(P, O) + ReducedOffset; }); ReducedOffset += ReducedSize_D7; FullOffset += FullSize_D7;
//
//	uint64_t Pattern_A[] = {Pattern_RightLowerA, Pattern_LeftUpperA, Pattern_LeftLowerA, Pattern_RightUpperA};
//	uint64_t Pattern_B[] = {Pattern_LowerRightB, Pattern_LowerLeftB, Pattern_LeftLowerB, Pattern_LeftUpperB, Pattern_UpperLeftB, Pattern_UpperRightB, Pattern_RightUpperB, Pattern_RightLowerB};
//	uint64_t Pattern_C[] = {Pattern_LowerC, Pattern_UpperC, Pattern_LeftC, Pattern_RightC};
//	uint64_t Pattern_D3[] = {Pattern_RightLowerD3, Pattern_LeftUpperD3, Pattern_LeftLowerD3, Pattern_RightUpperD3};
//	uint64_t Pattern_D4[] = {Pattern_RightLowerD4, Pattern_LeftUpperD4, Pattern_LeftLowerD4, Pattern_RightUpperD4};
//	uint64_t Pattern_D5[] = {Pattern_RightLowerD5, Pattern_LeftUpperD5, Pattern_LeftLowerD5, Pattern_RightUpperD5};
//	uint64_t Pattern_D6[] = {Pattern_RightLowerD6, Pattern_LeftUpperD6, Pattern_LeftLowerD6, Pattern_RightUpperD6};
//	uint64_t Pattern_D7[] = {Pattern_RightLowerD7, Pattern_LeftUpperD7, Pattern_LeftLowerD7, Pattern_RightUpperD7};
//	uint64_t Pattern_L1[] = {Pattern_LowerL1, Pattern_UpperL1, Pattern_LeftL1, Pattern_RightL1};
//	uint64_t Pattern_L2[] = {Pattern_LowerL2, Pattern_UpperL2, Pattern_LeftL2, Pattern_RightL2};
//	uint64_t Pattern_L3[] = {Pattern_LowerL3, Pattern_UpperL3, Pattern_LeftL3, Pattern_RightL3};
//	uint64_t Pattern_Sq[] = {Pattern_RightLowerSq, Pattern_LeftUpperSq, Pattern_LeftLowerSq, Pattern_RightUpperSq};
//	int(*ReducedIndex_A[])(const uint64_t P, const uint64_t O) = {ReducedIndex_RightLowerA, ReducedIndex_LeftUpperA, ReducedIndex_LeftLowerA, ReducedIndex_RightUpperA};
//	int(*ReducedIndex_B[])(const uint64_t P, const uint64_t O) = {ReducedIndex_LowerRightB, ReducedIndex_LowerLeftB, ReducedIndex_LeftLowerB, ReducedIndex_LeftUpperB, ReducedIndex_UpperLeftB, ReducedIndex_UpperRightB, ReducedIndex_RightUpperB, ReducedIndex_RightLowerB};
//	int(*ReducedIndex_C[])(const uint64_t P, const uint64_t O) = {ReducedIndex_LowerC, ReducedIndex_UpperC, ReducedIndex_LeftC, ReducedIndex_RightC};
//	int(*ReducedIndex_D3[])(const uint64_t P, const uint64_t O) = {ReducedIndex_RightLowerD3, ReducedIndex_LeftUpperD3, ReducedIndex_LeftLowerD3, ReducedIndex_RightUpperD3};
//	int(*ReducedIndex_D4[])(const uint64_t P, const uint64_t O) = {ReducedIndex_RightLowerD4, ReducedIndex_LeftUpperD4, ReducedIndex_LeftLowerD4, ReducedIndex_RightUpperD4};
//	int(*ReducedIndex_D5[])(const uint64_t P, const uint64_t O) = {ReducedIndex_RightLowerD5, ReducedIndex_LeftUpperD5, ReducedIndex_LeftLowerD5, ReducedIndex_RightUpperD5};
//	int(*ReducedIndex_D6[])(const uint64_t P, const uint64_t O) = {ReducedIndex_RightLowerD6, ReducedIndex_LeftUpperD6, ReducedIndex_LeftLowerD6, ReducedIndex_RightUpperD6};
//	int(*ReducedIndex_D7[])(const uint64_t P, const uint64_t O) = {ReducedIndex_RightLowerD7, ReducedIndex_LeftUpperD7, ReducedIndex_LeftLowerD7, ReducedIndex_RightUpperD7};
//	int(*ReducedIndex_L1[])(const uint64_t P, const uint64_t O) = {ReducedIndex_LowerL1, ReducedIndex_UpperL1, ReducedIndex_LeftL1, ReducedIndex_RightL1};
//	int(*ReducedIndex_L2[])(const uint64_t P, const uint64_t O) = {ReducedIndex_LowerL2, ReducedIndex_UpperL2, ReducedIndex_LeftL2, ReducedIndex_RightL2};
//	int(*ReducedIndex_L3[])(const uint64_t P, const uint64_t O) = {ReducedIndex_LowerL3, ReducedIndex_UpperL3, ReducedIndex_LeftL3, ReducedIndex_RightL3};
//	int(*ReducedIndex_Sq[])(const uint64_t P, const uint64_t O) = {ReducedIndex_RightLowerSq, ReducedIndex_LeftUpperSq, ReducedIndex_LeftLowerSq, ReducedIndex_RightUpperSq};
//	int(*FullIndex_A[])(const uint64_t P, const uint64_t O) = {FullIndex_RightLowerA, FullIndex_LeftUpperA, FullIndex_LeftLowerA, FullIndex_RightUpperA};
//	int(*FullIndex_B[])(const uint64_t P, const uint64_t O) = {FullIndex_LowerRightB, FullIndex_LowerLeftB, FullIndex_LeftLowerB, FullIndex_LeftUpperB, FullIndex_UpperLeftB, FullIndex_UpperRightB, FullIndex_RightUpperB, FullIndex_RightLowerB};
//	int(*FullIndex_C[])(const uint64_t P, const uint64_t O) = {FullIndex_LowerC, FullIndex_UpperC, FullIndex_LeftC, FullIndex_RightC};
//	//int(*FullIndex_D3[])(const uint64_t P, const uint64_t O) = {FullIndex_RightLowerD3, FullIndex_LeftUpperD3, FullIndex_LeftLowerD3, FullIndex_RightUpperD3};
//	int(*FullIndex_D4[])(const uint64_t P, const uint64_t O) = {FullIndex_RightLowerD4, FullIndex_LeftUpperD4, FullIndex_LeftLowerD4, FullIndex_RightUpperD4};
//	int(*FullIndex_D5[])(const uint64_t P, const uint64_t O) = {FullIndex_RightLowerD5, FullIndex_LeftUpperD5, FullIndex_LeftLowerD5, FullIndex_RightUpperD5};
//	int(*FullIndex_D6[])(const uint64_t P, const uint64_t O) = {FullIndex_RightLowerD6, FullIndex_LeftUpperD6, FullIndex_LeftLowerD6, FullIndex_RightUpperD6};
//	int(*FullIndex_D7[])(const uint64_t P, const uint64_t O) = {FullIndex_RightLowerD7, FullIndex_LeftUpperD7, FullIndex_LeftLowerD7, FullIndex_RightUpperD7};
//	int(*FullIndex_L1[])(const uint64_t P, const uint64_t O) = {FullIndex_LowerL1, FullIndex_UpperL1, FullIndex_LeftL1, FullIndex_RightL1};
//	int(*FullIndex_L2[])(const uint64_t P, const uint64_t O) = {FullIndex_LowerL2, FullIndex_UpperL2, FullIndex_LeftL2, FullIndex_RightL2};
//	int(*FullIndex_L3[])(const uint64_t P, const uint64_t O) = {FullIndex_LowerL3, FullIndex_UpperL3, FullIndex_LeftL3, FullIndex_RightL3};
//	int(*FullIndex_Sq[])(const uint64_t P, const uint64_t O) = {FullIndex_RightLowerSq, FullIndex_LeftUpperSq, FullIndex_LeftLowerSq, FullIndex_RightUpperSq};
//
//	// Reduced Index
//	Success &= Test(Symmetry_A,  ReducedSize_A,  Pattern_A,  ReducedIndex_A, true);
//	Success &= Test(Symmetry_B,  ReducedSize_B,  Pattern_B,  ReducedIndex_B, true);
//	Success &= Test(Symmetry_C,  ReducedSize_C,  Pattern_C,  ReducedIndex_C, true);
//	Success &= Test(Symmetry_D3, ReducedSize_D3, Pattern_D3, ReducedIndex_D3, true);
//	Success &= Test(Symmetry_D4, ReducedSize_D4, Pattern_D4, ReducedIndex_D4, true);
//	Success &= Test(Symmetry_D5, ReducedSize_D5, Pattern_D5, ReducedIndex_D5, true);
//	Success &= Test(Symmetry_D6, ReducedSize_D6, Pattern_D6, ReducedIndex_D6, true);
//	Success &= Test(Symmetry_D7, ReducedSize_D7, Pattern_D7, ReducedIndex_D7, false);
//	Success &= Test(Symmetry_L1, ReducedSize_L1, Pattern_L1, ReducedIndex_L1, true);
//	Success &= Test(Symmetry_L2, ReducedSize_L2, Pattern_L2, ReducedIndex_L2, true);
//	Success &= Test(Symmetry_L3, ReducedSize_L3, Pattern_L3, ReducedIndex_L3, false);
//	Success &= Test(Symmetry_Sq, ReducedSize_Sq, Pattern_Sq, ReducedIndex_Sq, true);
//
//	//FullIndex
//	FullOffset = 0;
//	Success &= Test_Feature_symmetrie_FullIndex(tmpWeights, Symmetry_C,  Pattern_C,  FullIndex_C , FullOffset); FullOffset += FullSize_C ;
//	Success &= Test_Feature_symmetrie_FullIndex(tmpWeights, Symmetry_L1, Pattern_L1, FullIndex_L1, FullOffset); FullOffset += FullSize_L1;
//	Success &= Test_Feature_symmetrie_FullIndex(tmpWeights, Symmetry_L2, Pattern_L2, FullIndex_L2, FullOffset); FullOffset += FullSize_L2;
//	Success &= Test_Feature_symmetrie_FullIndex(tmpWeights, Symmetry_L3, Pattern_L3, FullIndex_L3, FullOffset); FullOffset += FullSize_L3;
//	Success &= Test_Feature_symmetrie_FullIndex(tmpWeights, Symmetry_A,  Pattern_A,  FullIndex_A , FullOffset); FullOffset += FullSize_A ;
//	Success &= Test_Feature_symmetrie_FullIndex(tmpWeights, Symmetry_Sq, Pattern_Sq, FullIndex_Sq, FullOffset); FullOffset += FullSize_Sq;
//	Success &= Test_Feature_symmetrie_FullIndex(tmpWeights, Symmetry_B,  Pattern_B,  FullIndex_B , FullOffset); FullOffset += FullSize_B ;
//	//Success &= Test_Feature_symmetrie_FullIndex(tmpWeights, Symmetry_D3, Pattern_D3, FullIndex_D3, FullOffset); FullOffset += FullSize_D3;
//	Success &= Test_Feature_symmetrie_FullIndex(tmpWeights, Symmetry_D4, Pattern_D4, FullIndex_D4, FullOffset); FullOffset += FullSize_D4;
//	Success &= Test_Feature_symmetrie_FullIndex(tmpWeights, Symmetry_D5, Pattern_D5, FullIndex_D5, FullOffset); FullOffset += FullSize_D5;
//	Success &= Test_Feature_symmetrie_FullIndex(tmpWeights, Symmetry_D6, Pattern_D6, FullIndex_D6, FullOffset); FullOffset += FullSize_D6;
//	Success &= Test_Feature_symmetrie_FullIndex(tmpWeights, Symmetry_D7, Pattern_D7, FullIndex_D7, FullOffset); FullOffset += FullSize_D7;
//
//	delete[] tmpWeights;
//	return Success;
//}
