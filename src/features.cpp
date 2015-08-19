#include "features.h"

namespace Features
{
	const uint8_t BOXES = 16;
	const uint8_t BoxIndex[61] = {
		99,         //  0
		 0,  0,  0, //  1,  2,  3
		 1,  1,  1, //  4,  5,  6
		 2,  2,  2, //  7,  8,  9
		 3,  3,  3, // 10, 11, 12
		 4,  4,  4, // 13, 14, 15
		 5,  5,  5, // 16, 17, 18
		 6,  6,  6, // 19, 20, 21
		 7,  7,  7, // 22, 23, 24
		 8,  8,  8, // 25, 26, 27
		 9,  9,  9, // 28, 29, 30
		10, 10, 10, // 31, 32, 33
		11, 11, 11, // 34, 35, 36
		12, 12, 12, // 37, 38, 39
		13, 13, 13, // 40, 41, 42
		14, 14, 14, // 43, 44, 45
		15, 15, 15, // 46, 47, 48
		15, 15, 15, // 49, 50, 51
		15, 15, 15, // 52, 53, 54
		15, 15, 15, // 55, 56, 57
		15, 15, 15, // 58, 59, 60
	};

	unsigned int SumPow3[1024];

	std::vector<CPattern_L02X > PatVecL02X (BOXES);
	std::vector<CPattern_L1   > PatVecL1   (BOXES);
	std::vector<CPattern_L2   > PatVecL2   (BOXES);
	std::vector<CPattern_L3   > PatVecL3   (BOXES);
	std::vector<CPattern_D4   > PatVecD4   (BOXES);
	std::vector<CPattern_D5   > PatVecD5   (BOXES);
	std::vector<CPattern_D6   > PatVecD6   (BOXES);
	std::vector<CPattern_D7   > PatVecD7   (BOXES);
	std::vector<CPattern_Comet> PatVecComet(BOXES);
	std::vector<CPattern_Eplus> PatVecEplus(BOXES);
	std::vector<CPattern_C3p1 > PatVecC3p1 (BOXES);
	std::vector<CPattern_C3p2 > PatVecC3p2 (BOXES);
	std::vector<CPattern_Q0   > PatVecQ0   (BOXES);
	std::vector<CPattern_B5   > PatVecB5   (BOXES);

	uint64_t sumpow3(uint64_t exp)
	{
		int64_t sum = 0;
		while (exp)	{
			sum += pow(3LL, BitScanLSB(exp));
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

	void Initialize()
	{
		for (int i = 0; i < 1024; i++) SumPow3[i] = sumpow3(i);

		// Extract filenames form config file
		std::vector<std::string> Filenames;
		std::string s;
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
			if (PATTERN_L02X ) { PatVecL02X [j].set_weights(v_float); v_float.erase(v_float.begin(), v_float.begin() + PatVecL02X [j].ReducedSize); }
			if (PATTERN_L1   ) { PatVecL1   [j].set_weights(v_float); v_float.erase(v_float.begin(), v_float.begin() + PatVecL1   [j].ReducedSize); }
			if (PATTERN_L2   ) { PatVecL2   [j].set_weights(v_float); v_float.erase(v_float.begin(), v_float.begin() + PatVecL2   [j].ReducedSize); }
			if (PATTERN_L3   ) { PatVecL3   [j].set_weights(v_float); v_float.erase(v_float.begin(), v_float.begin() + PatVecL3   [j].ReducedSize); }
			if (PATTERN_D4   ) { PatVecD4   [j].set_weights(v_float); v_float.erase(v_float.begin(), v_float.begin() + PatVecD4   [j].ReducedSize); }
			if (PATTERN_D5   ) { PatVecD5   [j].set_weights(v_float); v_float.erase(v_float.begin(), v_float.begin() + PatVecD5   [j].ReducedSize); }
			if (PATTERN_D6   ) { PatVecD6   [j].set_weights(v_float); v_float.erase(v_float.begin(), v_float.begin() + PatVecD6   [j].ReducedSize); }
			if (PATTERN_D7   ) { PatVecD7   [j].set_weights(v_float); v_float.erase(v_float.begin(), v_float.begin() + PatVecD7   [j].ReducedSize); }
			if (PATTERN_Comet) { PatVecComet[j].set_weights(v_float); v_float.erase(v_float.begin(), v_float.begin() + PatVecComet[j].ReducedSize); }
			if (PATTERN_Eplus) { PatVecEplus[j].set_weights(v_float); v_float.erase(v_float.begin(), v_float.begin() + PatVecEplus[j].ReducedSize); }
			if (PATTERN_C3p1 ) { PatVecC3p1 [j].set_weights(v_float); v_float.erase(v_float.begin(), v_float.begin() + PatVecC3p1 [j].ReducedSize); }
			if (PATTERN_C3p2 ) { PatVecC3p2 [j].set_weights(v_float); v_float.erase(v_float.begin(), v_float.begin() + PatVecC3p2 [j].ReducedSize); }
			if (PATTERN_Q0   ) { PatVecQ0   [j].set_weights(v_float); v_float.erase(v_float.begin(), v_float.begin() + PatVecQ0   [j].ReducedSize); }
			if (PATTERN_B5   ) { PatVecB5   [j].set_weights(v_float); v_float.erase(v_float.begin(), v_float.begin() + PatVecB5   [j].ReducedSize); }
			if (v_float.size() != 0) std::cerr << "ERROR: filesizes are wrong!" << std::endl;
		}
	}

	void Finalize()
	{

	}
}

int EvaluateFeatures(const uint64_t P, const uint64_t O) // OPTIMIZATION: create a function taking "empties" as a variable
{
	const uint64_t empties = Empties(P, O);
	if (empties == 0) return (PopCount(P) << 1) - 64;
	const uint8_t BoxIndex = Features::BoxIndex[empties];
	float sum = 0;
	if (PATTERN_L02X ) sum += Features::PatVecL02X [BoxIndex].score(P, O);
	if (PATTERN_L1   ) sum += Features::PatVecL1   [BoxIndex].score(P, O);
	if (PATTERN_L2   ) sum += Features::PatVecL2   [BoxIndex].score(P, O);
	if (PATTERN_L3   ) sum += Features::PatVecL3   [BoxIndex].score(P, O);
	if (PATTERN_D4   ) sum += Features::PatVecD4   [BoxIndex].score(P, O);
	if (PATTERN_D5   ) sum += Features::PatVecD5   [BoxIndex].score(P, O);
	if (PATTERN_D6   ) sum += Features::PatVecD6   [BoxIndex].score(P, O);
	if (PATTERN_D7   ) sum += Features::PatVecD7   [BoxIndex].score(P, O);
	if (PATTERN_Comet) sum += Features::PatVecComet[BoxIndex].score(P, O);
	if (PATTERN_Eplus) sum += Features::PatVecEplus[BoxIndex].score(P, O);
	if (PATTERN_C3p1 ) sum += Features::PatVecC3p1 [BoxIndex].score(P, O);
	if (PATTERN_C3p2 ) sum += Features::PatVecC3p2 [BoxIndex].score(P, O);
	if (PATTERN_Q0   ) sum += Features::PatVecQ0   [BoxIndex].score(P, O);
	if (PATTERN_B5   ) sum += Features::PatVecB5   [BoxIndex].score(P, O);
	return BIND(RoundInt(sum), -64, 64);
}
int EvaluateFeatures(const uint64_t P, const uint64_t O, std::vector<float>& scores)
{
	const uint8_t BoxIndex = Features::BoxIndex[Empties(P, O)];
	scores.clear();
	if (PATTERN_L02X ) scores.push_back(Features::PatVecL02X [BoxIndex].score(P, O));
	if (PATTERN_L1   ) scores.push_back(Features::PatVecL1   [BoxIndex].score(P, O));
	if (PATTERN_L2   ) scores.push_back(Features::PatVecL2   [BoxIndex].score(P, O));
	if (PATTERN_L3   ) scores.push_back(Features::PatVecL3   [BoxIndex].score(P, O));
	if (PATTERN_D4   ) scores.push_back(Features::PatVecD4   [BoxIndex].score(P, O));
	if (PATTERN_D5   ) scores.push_back(Features::PatVecD5   [BoxIndex].score(P, O));
	if (PATTERN_D6   ) scores.push_back(Features::PatVecD6   [BoxIndex].score(P, O));
	if (PATTERN_D7   ) scores.push_back(Features::PatVecD7   [BoxIndex].score(P, O));
	if (PATTERN_Comet) scores.push_back(Features::PatVecComet[BoxIndex].score(P, O));
	if (PATTERN_Eplus) scores.push_back(Features::PatVecEplus[BoxIndex].score(P, O));
	if (PATTERN_C3p1 ) scores.push_back(Features::PatVecC3p1 [BoxIndex].score(P, O));
	if (PATTERN_C3p2 ) scores.push_back(Features::PatVecC3p2 [BoxIndex].score(P, O));
	if (PATTERN_Q0   ) scores.push_back(Features::PatVecQ0   [BoxIndex].score(P, O));
	if (PATTERN_B5   ) scores.push_back(Features::PatVecB5   [BoxIndex].score(P, O));
	float sum = 0;
	for (const auto& it : scores) sum += it;
	return RoundInt(sum);
}
int EvaluateFeatures(const uint64_t P, const uint64_t O, const Features::CIndexArray& Indices_P, const Features::CIndexArray& Indices_O)
{
	const uint8_t BoxIndex = Features::BoxIndex[Empties(P, O)];
	float sum = 0;
	int Offset = 0;
	auto P_begin = Indices_P.Indices.begin();
	auto O_begin = Indices_O.Indices.begin();
	if (PATTERN_L02X ) { sum += Features::PatVecL02X [BoxIndex].score(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_L1   ) { sum += Features::PatVecL1   [BoxIndex].score(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_L2   ) { sum += Features::PatVecL2   [BoxIndex].score(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_L3   ) { sum += Features::PatVecL3   [BoxIndex].score(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_D4   ) { sum += Features::PatVecD4   [BoxIndex].score(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_D5   ) { sum += Features::PatVecD5   [BoxIndex].score(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_D6   ) { sum += Features::PatVecD6   [BoxIndex].score(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_D7   ) { sum += Features::PatVecD7   [BoxIndex].score(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_Comet) { sum += Features::PatVecComet[BoxIndex].score(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_Eplus) { sum += Features::PatVecEplus[BoxIndex].score(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_C3p1 ) { sum += Features::PatVecC3p1 [BoxIndex].score(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_C3p2 ) { sum += Features::PatVecC3p2 [BoxIndex].score(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_Q0   ) { sum += Features::PatVecQ0   [BoxIndex].score(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_B5   ) { sum += Features::PatVecB5   [BoxIndex].score(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 8; }
	return RoundInt(sum);
}
int EvaluateFeatures(const uint64_t empties, const Features::CIndexArray& Indices_P, const Features::CIndexArray& Indices_O)
{
	const uint8_t BoxIndex = Features::BoxIndex[empties];
	float sum = 0;
	int Offset = 0;
	auto P_begin = Indices_P.Indices.begin();
	auto O_begin = Indices_O.Indices.begin();
	if (PATTERN_L02X ) { sum += Features::PatVecL02X [BoxIndex].score(P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_L1   ) { sum += Features::PatVecL1   [BoxIndex].score(P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_L2   ) { sum += Features::PatVecL2   [BoxIndex].score(P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_L3   ) { sum += Features::PatVecL3   [BoxIndex].score(P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_D4   ) { sum += Features::PatVecD4   [BoxIndex].score(P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_D5   ) { sum += Features::PatVecD5   [BoxIndex].score(P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_D6   ) { sum += Features::PatVecD6   [BoxIndex].score(P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_D7   ) { sum += Features::PatVecD7   [BoxIndex].score(P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_Comet) { sum += Features::PatVecComet[BoxIndex].score(P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_Eplus) { sum += Features::PatVecEplus[BoxIndex].score(P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_C3p1 ) { sum += Features::PatVecC3p1 [BoxIndex].score(P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_C3p2 ) { sum += Features::PatVecC3p2 [BoxIndex].score(P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_Q0   ) { sum += Features::PatVecQ0   [BoxIndex].score(P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_B5   ) { sum += Features::PatVecB5   [BoxIndex].score(P_begin + Offset, O_begin + Offset); Offset += 8; }
	return RoundInt(sum);
}

void UpdateIndexVec(const uint64_t P, const uint64_t O, Features::CIndexArray& Indices_P, Features::CIndexArray& Indices_O)
{
	int Offset = 0;
	auto P_begin = Indices_P.Indices.begin();
	auto O_begin = Indices_O.Indices.begin();
	if (PATTERN_L02X ) { Features::CPattern_L02X ::UpdateIndexVec(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_L1   ) { Features::CPattern_L1   ::UpdateIndexVec(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_L2   ) { Features::CPattern_L2   ::UpdateIndexVec(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_L3   ) { Features::CPattern_L3   ::UpdateIndexVec(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_D4   ) { Features::CPattern_D4   ::UpdateIndexVec(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_D5   ) { Features::CPattern_D5   ::UpdateIndexVec(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_D6   ) { Features::CPattern_D6   ::UpdateIndexVec(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_D7   ) { Features::CPattern_D7   ::UpdateIndexVec(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_Comet) { Features::CPattern_Comet::UpdateIndexVec(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_Eplus) { Features::CPattern_Eplus::UpdateIndexVec(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_C3p1 ) { Features::CPattern_C3p1 ::UpdateIndexVec(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_C3p2 ) { Features::CPattern_C3p2 ::UpdateIndexVec(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_Q0   ) { Features::CPattern_Q0   ::UpdateIndexVec(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 4; }
	if (PATTERN_B5   ) { Features::CPattern_B5   ::UpdateIndexVec(P, O, Indices_P.BitBoard, Indices_O.BitBoard, P_begin + Offset, O_begin + Offset); Offset += 8; }
}

void FillConfigurationVec(const uint64_t P, const uint64_t O, std::vector<int>& vec)
{
	vec.clear();
	if (PATTERN_L02X ) Features::CPattern_L02X ::FillConfigurationVec(P, O, vec);
	if (PATTERN_L1   ) Features::CPattern_L1   ::FillConfigurationVec(P, O, vec);
	if (PATTERN_L2   ) Features::CPattern_L2   ::FillConfigurationVec(P, O, vec);
	if (PATTERN_L3   ) Features::CPattern_L3   ::FillConfigurationVec(P, O, vec);
	if (PATTERN_D4   ) Features::CPattern_D4   ::FillConfigurationVec(P, O, vec);
	if (PATTERN_D5   ) Features::CPattern_D5   ::FillConfigurationVec(P, O, vec);
	if (PATTERN_D6   ) Features::CPattern_D6   ::FillConfigurationVec(P, O, vec);
	if (PATTERN_D7   ) Features::CPattern_D7   ::FillConfigurationVec(P, O, vec);
	if (PATTERN_Comet) Features::CPattern_Comet::FillConfigurationVec(P, O, vec);
	if (PATTERN_Eplus) Features::CPattern_Eplus::FillConfigurationVec(P, O, vec);
	if (PATTERN_C3p1 ) Features::CPattern_C3p1 ::FillConfigurationVec(P, O, vec);
	if (PATTERN_C3p2 ) Features::CPattern_C3p2 ::FillConfigurationVec(P, O, vec);
	if (PATTERN_Q0   ) Features::CPattern_Q0   ::FillConfigurationVec(P, O, vec);
	if (PATTERN_B5   ) Features::CPattern_B5   ::FillConfigurationVec(P, O, vec);
}
void FillConfigurationVecOffsetted(const uint64_t P, const uint64_t O, std::vector<int>& vec)
{
	int Offset = 0;
	vec.clear();
	if (PATTERN_L02X ) { Features::CPattern_L02X ::FillConfigurationVec(P, O, vec, Offset); Offset += Features::CPattern_L02X ::ReducedSize; }
	if (PATTERN_L1   ) { Features::CPattern_L1   ::FillConfigurationVec(P, O, vec, Offset); Offset += Features::CPattern_L1   ::ReducedSize; }
	if (PATTERN_L2   ) { Features::CPattern_L2   ::FillConfigurationVec(P, O, vec, Offset); Offset += Features::CPattern_L2   ::ReducedSize; }
	if (PATTERN_L3   ) { Features::CPattern_L3   ::FillConfigurationVec(P, O, vec, Offset); Offset += Features::CPattern_L3   ::ReducedSize; }
	if (PATTERN_D4   ) { Features::CPattern_D4   ::FillConfigurationVec(P, O, vec, Offset); Offset += Features::CPattern_D4   ::ReducedSize; }
	if (PATTERN_D5   ) { Features::CPattern_D5   ::FillConfigurationVec(P, O, vec, Offset); Offset += Features::CPattern_D5   ::ReducedSize; }
	if (PATTERN_D6   ) { Features::CPattern_D6   ::FillConfigurationVec(P, O, vec, Offset); Offset += Features::CPattern_D6   ::ReducedSize; }
	if (PATTERN_D7   ) { Features::CPattern_D7   ::FillConfigurationVec(P, O, vec, Offset); Offset += Features::CPattern_D7   ::ReducedSize; }
	if (PATTERN_Comet) { Features::CPattern_Comet::FillConfigurationVec(P, O, vec, Offset); Offset += Features::CPattern_Comet::ReducedSize; }
	if (PATTERN_Eplus) { Features::CPattern_Eplus::FillConfigurationVec(P, O, vec, Offset); Offset += Features::CPattern_Eplus::ReducedSize; }
	if (PATTERN_C3p1 ) { Features::CPattern_C3p1 ::FillConfigurationVec(P, O, vec, Offset); Offset += Features::CPattern_C3p1 ::ReducedSize; }
	if (PATTERN_C3p2 ) { Features::CPattern_C3p2 ::FillConfigurationVec(P, O, vec, Offset); Offset += Features::CPattern_C3p2 ::ReducedSize; }
	if (PATTERN_Q0   ) { Features::CPattern_Q0   ::FillConfigurationVec(P, O, vec, Offset); Offset += Features::CPattern_Q0   ::ReducedSize; }
	if (PATTERN_B5   ) { Features::CPattern_B5   ::FillConfigurationVec(P, O, vec, Offset); Offset += Features::CPattern_B5   ::ReducedSize; }
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
