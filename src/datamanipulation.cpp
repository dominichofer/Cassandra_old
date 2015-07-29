#include "datamanipulation.h"

CDataset_Position_Score_PV::CDataset_Position_Score_PV(const CDataset_Position_FullScore& o)	
	: P(o.P), O(o.O), depth(o.DeepestDepth()), selectivity(DATASET_DEFAULT_selectivity), score(o.DeepestScore())
{ for (int i = 0; i < 5; i++) PV[i] = o.PV[i]; }


CDataset_Position_Score_PV::CDataset_Position_Score_PV(std::string s)
{
	std::string delimiter = ";";
	std::string token;
	size_t pos = 0;

	Reset();
	P = 0;
	O = 0;

	for (unsigned int i = 0; i < 64; ++i)
	{
		     if (s.substr(i, 1) == "X") P |= 0x8000000000000000ULL >> i;
		else if (s.substr(i, 1) == "O") O |= 0x8000000000000000ULL >> i;
	}
	if (s.substr(65, 1) == "O") std::swap(P, O);

	while ((pos = s.find(delimiter)) != std::string::npos)
	{
		token = s.substr(0, pos);
		s.erase(0, pos + delimiter.length());

		for (int i = 0; i < 64; ++i)
			if (token.substr(1, 2) == field_name[i])
				score = MAX(score, static_cast<int8_t>(std::stoi(token.substr(4, 3))));
	}
	depth = DATASET_depth_END;
	selectivity = DATASET_DEFAULT_selectivity;

}


CDataset_Position_FullScore::CDataset_Position_FullScore(const CDataset_Position_Score_PV& o)
	: P(o.P), O(o.O)
{ 
	Reset();
	if ((o.selectivity == DATASET_DEFAULT_selectivity) && (o.depth != DATASET_DEFAULT_depth)) score[o.depth == DATASET_depth_END ? Empties(P, O) : o.depth] = o.score;
	for (int i = 0; i < 5; i++) PV[i] = o.PV[i];
}



std::vector<CDataset_Position_Score_PV> read_hr_vector(const std::string & filename, std::size_t size)
{
	FILE* file = fopen(filename.c_str(), "r");
	if (!file){
		std::cerr << "ERROR: File '" << filename << "' could not be opened!" << std::endl;
		throw "File could not be opened.";
		return std::vector<CDataset_Position_Score_PV>();
	}

	const std::size_t N = 4 * 1024;
	std::vector<CDataset_Position_Score_PV> vector;
	char DataArray[N];
	while (size-- && fgets(DataArray, N, file))
		vector.push_back(CDataset_Position_Score_PV(std::string(DataArray)));

	fclose(file);
	return vector;
}

void write_to_hr_file(const std::string & filename, const std::vector<CDataset_Position_Score_PV>& vector)
{
	FILE* file = fopen(filename.c_str(), "w");
	if (!file){
		std::cerr << "ERROR: File '" << filename << "' could not be opened!" << std::endl;
		throw "File could not be opened.";
		return;
	}

	for (const CDataset_Position_Score_PV& it : vector) fputs((it.to_string() + "\n").c_str(), file);

	fclose(file);
}

template <> std::vector<CPositionScore> read_vector(const std::string & filename, std::size_t size)
{
	std::vector<CPositionScore> vec_out;
	std::string ending = filename.substr(filename.rfind(".") + 1, filename.length());
	if (!validEnding(ending)) throw "File ending not supported.";

	if (ending.compare(DATASET_ENDING_POSITION_SCORE_PV))
	{
		std::vector<CDataset_Position_Score_PV> vec_in = read_vector<CDataset_Position_Score_PV>(filename, size);
		for (const auto& pos : vec_in)
			vec_out.push_back(static_cast<CPositionScore>(pos));
	}
	else if (ending.compare(DATASET_ENDING_POSITION_FULLSCORE))
	{
		std::vector<CDataset_Position_FullScore> vec_in = read_vector<CDataset_Position_FullScore>(filename, size);
		for (const auto& pos : vec_in)
			vec_out.push_back(static_cast<CPositionScore>(pos));
	}
	else if (ending.compare(DATASET_ENDING_POSITION_HR))
	{
		std::vector<CDataset_Position_Score_PV> vec_in = read_hr_vector(filename, size);
		for (const auto& pos : vec_in)
			vec_out.push_back(static_cast<CPositionScore>(pos));
	}
	return vec_out;
}
