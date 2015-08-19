#include <algorithm>
#include <cstdint>
#include <iostream>
#include <map>
#include "datamanipulation.h"
#include "generatepositions.h"
#include "selectivity.h"
#include "utility.h"

void ScoreHistogram(const std::string & filename)
{
	std::string ending = filename.substr(filename.rfind(".") + 1, filename.length());
	if (!validEnding(ending)) { std::cerr << "File ending not supported." << std::endl; throw "File ending not supported."; }

	printf("score|   frequency  \n");
	printf("-----+--------------\n");

	if (ending.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
	{
		std::map<int, std::size_t> score_map;
		std::vector<CDataset_Position_Score_PV> vec = read_vector<CDataset_Position_Score_PV>(filename);
		for (const auto& it : vec) score_map[it.score]++;
		for (const auto& it : score_map) printf(" %+2.2d |%14s\n", it.first, ThousandsSeparator(it.second).c_str());
	}
	else if (ending.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0)
	{
		std::map<int, std::size_t> score_map;
		std::vector<CDataset_Position_FullScore> vec = read_vector<CDataset_Position_FullScore>(filename);
		for (const auto& it : vec) score_map[it.DeepestScore()]++;
		for (const auto& it : score_map) printf(" %+2.2d |%14s\n", it.first, ThousandsSeparator(it.second).c_str());
	}
	else if (ending.compare(DATASET_ENDING_POSITION_HR) == 0)
	{
		std::map<int, std::size_t> score_map;
		std::vector<CDataset_Position_Score_PV> vec = read_hr_vector(filename);
		for (const auto& it : vec) score_map[it.score]++;
		for (const auto& it : score_map) printf(" %+2.2d |%14s\n", it.first, ThousandsSeparator(it.second).c_str());
	}
}

void PVstats(const std::string & filename)
{
	std::string ending = filename.substr(filename.rfind(".") + 1, filename.length());
	if (!validEnding(ending)) { std::cerr << "File ending not supported." << std::endl; throw "File ending not supported."; }

	if (ending.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec = read_vector<CDataset_Position_Score_PV>(filename);
		uint64_t PV[5][65];
		uint64_t FieldSum[5][11];
		int field_type[65] = {
			0, 1, 2, 3, 3, 2, 1, 0,
			1, 4, 5, 6, 6, 5, 4, 1,
			2, 5, 7, 8, 8, 7, 5, 3,
			3, 6, 8, 9, 9, 8, 6, 3,
			3, 6, 8, 9, 9, 8, 6, 3,
			2, 5, 7, 8, 8, 7, 5, 3,
			1, 4, 5, 6, 6, 5, 4, 1,
			0, 1, 2, 3, 3, 2, 1, 0, 10
		};
		memset(PV, 0, 5 * 65 * sizeof(uint64_t));
		memset(FieldSum, 0, 5 * 11 * sizeof(uint64_t));
		for (const auto& it : vec)
			for (unsigned int i = 0; i < 5; i++)
			{
				PV[i][it.PV[i]]++;
				FieldSum[i][field_type[it.PV[i]]]++;
			}

		printf("move|   PV[0]  |   PV[1]  |   PV[2]  |   PV[3]  |   PV[4]  |    sum   \n");
		printf("----+----------+----------+----------+----------+----------+----------\n");
		for (unsigned int j = 0; j < 65; j++)
			printf(" %2s |%10s|%10s|%10s|%10s|%10s|%10s\n", field_name[j].c_str(),
				ThousandsSeparator(PV[0][j]).c_str(),
				ThousandsSeparator(PV[1][j]).c_str(),
				ThousandsSeparator(PV[2][j]).c_str(),
				ThousandsSeparator(PV[3][j]).c_str(),
				ThousandsSeparator(PV[4][j]).c_str(),
				ThousandsSeparator(PV[0][j]+PV[1][j]+PV[2][j]+PV[3][j]+PV[4][j]).c_str());
		printf("\n");
		printf("field type| fields \n");
		printf("----------+--------\n");
		for (unsigned int j = 0; j < 11; j++)
		{
			printf("       %2d |", j);
			for (unsigned int i = 0; i < 65; i++) if (field_type[i] == j) printf(" %2s", field_name[i]);
			printf("\n");
		}
		printf("\n");
		printf("field type|   PV[0]  |   PV[1]  |   PV[2]  |   PV[3]  |   PV[4]  |    sum   \n");
		printf("----------+----------+----------+----------+----------+----------+----------\n");
		for (unsigned int j = 0; j < 11; j++)
			printf("       %2d |%10s|%10s|%10s|%10s|%10s|%10s\n", j,
				ThousandsSeparator(FieldSum[0][j]).c_str(),
				ThousandsSeparator(FieldSum[1][j]).c_str(),
				ThousandsSeparator(FieldSum[2][j]).c_str(),
				ThousandsSeparator(FieldSum[3][j]).c_str(),
				ThousandsSeparator(FieldSum[4][j]).c_str(),
				ThousandsSeparator(FieldSum[0][j]+FieldSum[1][j]+FieldSum[2][j]+FieldSum[3][j]+FieldSum[4][j]).c_str());
	}
	else
		{ std::cerr << "File ending not supported." << std::endl; throw "File ending not supported."; }
}

void FieldValues(const std::string & filename)
{
	std::string ending = filename.substr(filename.rfind(".") + 1, filename.length());
	if (!validEnding(ending)) { std::cerr << "File ending not supported." << std::endl; throw "File ending not supported."; }
	
	if (ending.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec = read_vector<CDataset_Position_Score_PV>(filename);
		uint64_t FieldSum[11];
		int field_type[65] = {
			0, 1, 2, 3, 3, 2, 1, 0,
			1, 4, 5, 6, 6, 5, 4, 1,
			2, 5, 7, 8, 8, 7, 5, 3,
			3, 6, 8, 9, 9, 8, 6, 3,
			3, 6, 8, 9, 9, 8, 6, 3,
			2, 5, 7, 8, 8, 7, 5, 3,
			1, 4, 5, 6, 6, 5, 4, 1,
			0, 1, 2, 3, 3, 2, 1, 0, 10
		};
		memset(FieldSum, 0, 11);
		for (const auto& it : vec)
			for (unsigned int i = 3; i < 5; i++)
				FieldSum[field_type[it.PV[i]]]++;
		FieldSum[0] *= 2;
		FieldSum[4] *= 2;
		FieldSum[7] *= 2;
		FieldSum[9] *= 2;
		for (unsigned int i = 0; i < 10; i++) printf("%2d\t", RoundInt(FieldSum[i] / static_cast<float>(vec.size() / 50)));
		printf("\n");
	}
	else
		{ std::cerr << "File ending not supported." << std::endl; throw "File ending not supported."; }
}

void Peek(const std::string & filename, const uint64_t start, const uint64_t num)
{
	const unsigned int width_num = ThousandsSeparator(start + num - 1).length();
	std::string L1, L2;
	std::string ending = filename.substr(filename.rfind(".") + 1, filename.length());
	if (!validEnding(ending)) { std::cerr << "File ending not supported." << std::endl; throw "File ending not supported."; }
	

	L1.append(std::string(width_num - 2, ' ')).append("# |");
	L2.append(std::string(width_num, '-')).append("+");

	L1.append("                            Position                            | depth|score|       PV       \n");
	L2.append("----------------------------------------------------------------+------+-----+----------------\n");
	printf("%s%s", L1.c_str(), L2.c_str());

	if (ending.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec = read_vector<CDataset_Position_Score_PV>(filename);
		for (uint64_t i = start; i < start + num; i++)
			printf("%*s|%64s|%6s| %+2.2d | %2s %2s %2s %2s %2s \n", width_num, ThousandsSeparator(i).c_str(), board1D(vec[i].P, vec[i].O).c_str(), 
				DepthSelectivity(vec[i].depth, vec[i].selectivity, vec[i].P, vec[i].O).c_str(), vec[i].score,
				field_name[vec[i].PV[0]].c_str(),
				field_name[vec[i].PV[1]].c_str(),
				field_name[vec[i].PV[2]].c_str(),
				field_name[vec[i].PV[3]].c_str(),
				field_name[vec[i].PV[4]].c_str());
	}
	else if (ending.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0)
	{
		std::vector<CDataset_Position_FullScore> vec = read_vector<CDataset_Position_FullScore>(filename);
		for (uint64_t i = start; i < start + num; i++)
			printf("%*s|%64s|%6s| %+2.2d | %2s %2s %2s %2s %2s \n", width_num, ThousandsSeparator(i).c_str(), board1D(vec[i].P, vec[i].O).c_str(), 
				DepthSelectivity(vec[i].DeepestDepth(), DATASET_DEFAULT_selectivity, vec[i].P, vec[i].O).c_str(), vec[i].DeepestScore(),
				field_name[vec[i].PV[0]].c_str(),
				field_name[vec[i].PV[1]].c_str(),
				field_name[vec[i].PV[2]].c_str(),
				field_name[vec[i].PV[3]].c_str(),
				field_name[vec[i].PV[4]].c_str());
	}
	else if (ending.compare(DATASET_ENDING_POSITION_HR) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec = read_hr_vector(filename);
		for (uint64_t i = start; i < start + num; i++)
			printf("%*s|%64s|%6s| %+2.2d | %2s %2s %2s %2s %2s \n", width_num, ThousandsSeparator(i).c_str(), board1D(vec[i].P, vec[i].O).c_str(), 
				DepthSelectivity(vec[i].depth, vec[i].selectivity, vec[i].P, vec[i].O).c_str(), vec[i].score,
				field_name[vec[i].PV[0]].c_str(),
				field_name[vec[i].PV[1]].c_str(),
				field_name[vec[i].PV[2]].c_str(),
				field_name[vec[i].PV[3]].c_str(),
				field_name[vec[i].PV[4]].c_str());
	}
}

void Progress(const std::string & filename)
{
	std::string ending = filename.substr(filename.rfind(".") + 1, filename.length());
	if (!validEnding(ending)) { std::cerr << "File ending not supported." << std::endl; throw "File ending not supported."; }

	printf(" depth| number of pos\n");
	printf("------+--------------\n");

	if (ending.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
	{
		std::map<std::pair<unsigned int, unsigned int>, std::size_t> depth_selectivity_map;
		std::vector<CDataset_Position_Score_PV> vec = read_vector<CDataset_Position_Score_PV>(filename);
		for (const auto& it : vec) depth_selectivity_map[std::pair<unsigned int, unsigned int>(it.depth, it.selectivity)]++;
		for (const auto& it : depth_selectivity_map) printf("%6s|%14s\n", DepthSelectivity(it.first.first, it.first.second).c_str(), ThousandsSeparator(it.second).c_str());
	}
	else if (ending.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0)
	{
		std::map<std::pair<unsigned int, unsigned int>, std::size_t> depth_selectivity_map;
		std::vector<CDataset_Position_FullScore> vec = read_vector<CDataset_Position_FullScore>(filename);
		for (const auto& it : vec)
			for (int i = 0; i < 61; i++)
				if (it.score[i] != DATASET_DEFAULT_score)
					depth_selectivity_map[std::pair<unsigned int, unsigned int>(i, DATASET_DEFAULT_selectivity)]++;
		for (const auto& it : depth_selectivity_map) printf("%6s|%14s\n", DepthSelectivity(it.first.first, it.first.second).c_str(), ThousandsSeparator(it.second).c_str());
	}
	else if (ending.compare(DATASET_ENDING_POSITION_HR) == 0)
	{
		std::map<std::pair<unsigned int, unsigned int>, std::size_t> depth_selectivity_map;
		std::vector<CDataset_Position_Score_PV> vec = read_hr_vector(filename);
		for (const auto& it : vec) depth_selectivity_map[std::pair<unsigned int, unsigned int>(it.depth, it.selectivity)]++;
		for (const auto& it : depth_selectivity_map) printf("%6s|%14s\n", DepthSelectivity(it.first.first, it.first.second).c_str(), ThousandsSeparator(it.second).c_str());
	}
}

void Test(const std::string & filename, const int empties)
{
	uint64_t counter = 0;
	std::string ending = filename.substr(filename.rfind(".") + 1, filename.length());
	if (!validEnding(ending)) { std::cerr << "File ending not supported." << std::endl; throw "File ending not supported."; }
	
	if (ending.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec = read_vector<CDataset_Position_Score_PV>(filename);
		counter = std::count_if(vec.begin(), vec.end(), [=](const CDataset_Position_Score_PV& it){ return ((it.P & it.O) != 0) || (Empties(it.P, it.O) != empties); });
	}
	else if (ending.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0)
	{
		std::vector<CDataset_Position_FullScore> vec = read_vector<CDataset_Position_FullScore>(filename);
		counter = std::count_if(vec.begin(), vec.end(), [=](const CDataset_Position_FullScore& it){ return ((it.P & it.O) != 0) || (Empties(it.P, it.O) != empties); });
	}
	else if (ending.compare(DATASET_ENDING_POSITION_HR) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec = read_hr_vector(filename);
		counter = std::count_if(vec.begin(), vec.end(), [=](const CDataset_Position_Score_PV& it){ return ((it.P & it.O) != 0) || (Empties(it.P, it.O) != empties); });
	}
	std::cout << "Numer of erroneous positions: " << counter << "." << std::endl;
}

void Reset(const std::string & filename)
{
	std::string ending = filename.substr(filename.rfind(".") + 1, filename.length());
	if (!validEnding(ending)) { std::cerr << "File ending not supported." << std::endl; throw "File ending not supported."; }

	if (ending.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec = read_vector<CDataset_Position_Score_PV>(filename);
		for (auto& it : vec) it.Reset();
		write_to_file(filename, vec);
	}
	else if (ending.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0)
	{
		std::vector<CDataset_Position_FullScore> vec = read_vector<CDataset_Position_FullScore>(filename);
		for (auto& it : vec) it.Reset();
		write_to_file(filename, vec);
	}
	else if (ending.compare(DATASET_ENDING_POSITION_HR) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec = read_hr_vector(filename);
		for (auto& it : vec) it.Reset();
		write_to_hr_file(filename, vec);
	}
	std::cout << "Positions of " << filename << " resetted." << std::endl;
}

void Convert(const std::string & s_input, const std::string & s_output)
{
	std::string ending_input = s_input.substr(s_input.rfind(".") + 1, s_input.length());
	std::string ending_output = s_output.substr(s_output.rfind(".") + 1, s_output.length());
	if (!validEnding(ending_input)) throw "File ending not supported.";
	if (!validEnding(ending_output)) throw "File ending not supported.";

	if (ending_input.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec_in = read_vector<CDataset_Position_Score_PV>(s_input);
		if (ending_output.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0)
		{
			std::vector<CDataset_Position_FullScore> vec_out;
			for (const auto& elem : vec_in)
				vec_out.push_back(static_cast<CDataset_Position_FullScore>(elem));
			write_to_file(s_output, vec_out);
		}
		else if (ending_output.compare(DATASET_ENDING_POSITION_HR) == 0)
			write_to_hr_file(s_output, vec_in);
		else
			std::cerr << "ERROR: conversion not available." << std::endl;
	}
	else if (ending_input.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0)
	{
		std::vector<CDataset_Position_FullScore> vec_in = read_vector<CDataset_Position_FullScore>(s_input);
		std::vector<CDataset_Position_Score_PV> vec_out;
		for (const auto& elem : vec_in)
			vec_out.push_back(static_cast<CDataset_Position_Score_PV>(elem));
		if (ending_output.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
			write_to_file(s_output, vec_out);
		else if (ending_output.compare(DATASET_ENDING_POSITION_HR) == 0)
			write_to_hr_file(s_output, vec_out);
		else
			std::cerr << "ERROR: conversion not available." << std::endl;
	}
	else if (ending_input.compare(DATASET_ENDING_POSITION_HR) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec_in = read_hr_vector(s_input);
		if (ending_output.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
			write_to_file(s_output, vec_in);
		else if (ending_output.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0)
		{
			std::vector<CDataset_Position_FullScore> vec_out;
			for (const auto& elem : vec_in)
				vec_out.push_back(static_cast<CDataset_Position_FullScore>(elem));
			write_to_file<CDataset_Position_FullScore>(s_output, vec_out);
		}
		else
			std::cerr << "ERROR: conversion not available." << std::endl;
	}
	else
		std::cerr << "ERROR: conversion not available." << std::endl;

	std::cout << "Positions of " << s_input << " converted to " << s_output << std::endl;
}

void PlayPV(const std::string& in, const std::string& out)
{
	std::string  in_ending =  in.substr( in.rfind(".") + 1,  in.length());
	std::string out_ending = out.substr(out.rfind(".") + 1, out.length());
	if ( in_ending.compare(DATASET_ENDING_POSITION_SCORE_PV) != 0) throw "File ending not supported.";
	if (out_ending.compare(DATASET_ENDING_POSITION_SCORE_PV) != 0) throw "File ending not supported.";
	
	std::vector<CDataset_Position_Score_PV> vec_in = read_vector<CDataset_Position_Score_PV>(in);
	std::vector<CDataset_Position_Score_PV> vec_out;
	std::cout << ThousandsSeparator(vec_in.size()) << " positions loaded." << std::endl;

	for (const auto& pos : vec_in)
	{
		uint64_t P = pos.P;
		uint64_t O = pos.O;
		assert((P & O) == 0);
		if (pos.PV[0] == 64) continue;
		if (!HasMoves(P, O))
		{
			if (HasMoves(O, P))
				std::swap(P, O);
			else
				continue;
		}
		PlayStone(P, O, pos.PV[0]);
		vec_out.push_back(CDataset_Position_Score_PV(P, O, pos.depth-1, pos.selectivity, -pos.score, pos.PV[1], pos.PV[2], pos.PV[3], pos.PV[4], 64));
	}

	write_to_file(out, vec_out);
	std::cout << " Wrote " << ThousandsSeparator(vec_out.size()) << " positions to " << out << std::endl;
}

void SelectOnScore(const std::string& in, const std::string& out, const int score)
{
	std::string  in_ending =  in.substr( in.rfind(".") + 1,  in.length());
	std::string out_ending = out.substr(out.rfind(".") + 1, out.length());
	if ( in_ending.compare(DATASET_ENDING_POSITION_SCORE_PV) != 0) throw "File ending not supported.";
	if (out_ending.compare(DATASET_ENDING_POSITION_SCORE_PV) != 0) throw "File ending not supported.";
	
	std::vector<CDataset_Position_Score_PV> vec = read_vector<CDataset_Position_Score_PV>(in);
	std::cout << ThousandsSeparator(vec.size()) << " positions loaded." << std::endl;
	
	vec.erase(std::remove_if(vec.begin(), vec.end(), [=](const CDataset_Position_Score_PV& pos){ return pos.score < -score || pos.score > +score; }), vec.end());
	std::sort(vec.begin(), vec.end(), [](const CDataset_Position_Score_PV& lhs, const CDataset_Position_Score_PV& rhs){ return (lhs.P <  rhs.P) || ((lhs.P == rhs.P) && (lhs.O <  rhs.O)); });
	vec.erase(std::unique(vec.begin(), vec.end(), [](const CDataset_Position_Score_PV& lhs, const CDataset_Position_Score_PV& rhs){ return (lhs.P ==  rhs.P) && (lhs.O == rhs.O); }), vec.end());
	
	write_to_file(out, vec);
	std::cout << " Wrote " << ThousandsSeparator(vec.size()) << " positions to " << out << std::endl;
}

void Distribute(const std::string& in, const std::string& out)
{
	std::string  in_ending =  in.substr( in.rfind(".") + 1,  in.length());
	if ( in_ending.compare(DATASET_ENDING_POSITION_SCORE_PV) != 0) throw "File ending not supported.";
	
	std::vector<CDataset_Position_Score_PV> vec = read_vector<CDataset_Position_Score_PV>(in);
	std::cout << ThousandsSeparator(vec.size()) << " positions loaded." << std::endl;
	
	// Shuffle
	std::random_shuffle(vec.begin(), vec.end());

	{
		std::vector<CDataset_Position_Score_PV> vec_tmp(vec.begin(), vec.begin() + 1000000);
		std::string tmp = out + "1M.psp";
		write_to_file(tmp, vec_tmp);
		std::cout << " Wrote " << ThousandsSeparator(vec_tmp.size()) << " positions to " << tmp << std::endl;
		vec.erase(vec.begin(), vec.begin() + 1000000);
	}
	{
		std::vector<CDataset_Position_FullScore> vec_tmp(vec.begin(), vec.begin() + 10000);
		std::string tmp = out + "10k.pfs";
		write_to_file(tmp, vec_tmp);
		std::cout << " Wrote " << ThousandsSeparator(vec_tmp.size()) << " positions to " << tmp << std::endl;
		vec.erase(vec.begin(), vec.begin() + 10000);
	}
	{
		std::vector<CDataset_Position_FullScore> vec_tmp(vec.begin(), vec.begin() + 1000);
		std::string tmp = out + "1k.pfs";
		write_to_file(tmp, vec_tmp);
		std::cout << " Wrote " << ThousandsSeparator(vec_tmp.size()) << " positions to " << tmp << std::endl;
		vec.erase(vec.begin(), vec.begin() + 1000);
	}
	{
		std::vector<CDataset_Position_FullScore> vec_tmp(vec.begin(), vec.begin() + 200);
		std::string tmp = out + "200.pfs";
		write_to_file(tmp, vec_tmp);
		std::cout << " Wrote " << ThousandsSeparator(vec_tmp.size()) << " positions to " << tmp << std::endl;
		vec.erase(vec.begin(), vec.begin() + 200);
	}
}

void print_help()
{
	std::cout << "Manages files." << std::endl <<
	"Modes:" << std::endl <<
	"         -rnd -n 123 -e 123 -o file.xxx (random positions)" << std::endl <<
	"         -playPV -i file.xxx -o filex.yyy (play PV)" << std::endl <<
	"         -ss -n 123 -i file.xxx -o filex.yyy (select on score and make unique)" << std::endl <<
	"         -distribute -i file.xxx -o file (distributes)" << std::endl <<
	"         -perft -d 123 -o file.xxx (perft positions)" << std::endl <<
	"         -i file.xxx -o file.yyy (convert file)" << std::endl <<
	//"         -char -i file.xxx (score characteristics)" << std::endl <<
	"         -hist -i file.xxx (score histogram)" << std::endl <<
	"         -PV -i file.xxx (PV stats)" << std::endl <<
	"         -FieldValues -i file.xxx (Field Values)" << std::endl <<
	"         -peek 123 123 -i file.xxx (peek at positions)" << std::endl <<
	"         -prog -i file.xxx (progress)" << std::endl <<
	"         -test -e 123 -i file.xxx (test positions)" << std::endl <<
	"         -reset -i file.xxx (reset positions)" << std::endl <<
	"Arguments:" << std::endl <<
	"-rnd     Writes (-n) random positions with (-e) empty fields to file (-o)." << std::endl <<
	"-perft   Writes perft positions of depth (-d) to file (-o)." << std::endl <<
	"-ETH     ETH startposition." << std::endl <<
	"-n 123   Number of positions." << std::endl <<
	"-e 123   Number of empty fields." << std::endl <<
	"-d 123   Depth." << std::endl <<
	"-i abc   Input filename." << std::endl <<
	"-o abc   Output filename." << std::endl <<
	"-h       Displays this help." << std::endl;
}

int main(int argc, char* argv[])
{
	bool b_rnd = false;
	bool b_playPV = false;
	bool b_perft = false;
	bool b_ss = false;
	bool b_distribute = false;
	//bool b_char = false;
	bool b_hist = false;
	bool b_PV = false;
	bool b_FieldValues = false;
	bool b_peek = false;
	bool b_prog = false;
	bool b_test = false;
	bool b_reset = false;
	bool b_ETH = false;
	int n, e, d, peek_start, peek_num;
	std::string s_input;
	std::string s_output;

	for (int i = 0; i < argc; ++i)
	{
		     if (std::string(argv[i]) == "-rnd") b_rnd = true;
		else if (std::string(argv[i]) == "-playPV") b_playPV = true;
		else if (std::string(argv[i]) == "-ss") b_ss = true;
		else if (std::string(argv[i]) == "-distribute") b_distribute = true;
		else if (std::string(argv[i]) == "-perft") b_perft = true;
		//else if (std::string(argv[i]) == "-char") b_char = true;
		else if (std::string(argv[i]) == "-hist") b_hist = true;
		else if (std::string(argv[i]) == "-PV") b_PV = true;
		else if (std::string(argv[i]) == "-FieldValues") b_FieldValues = true;
		else if (std::string(argv[i]) == "-peek") { b_peek = true; peek_start = atoi(argv[++i]); peek_num = atoi(argv[++i]); }
		else if (std::string(argv[i]) == "-prog") b_prog = true;
		else if (std::string(argv[i]) == "-test") b_test = true;
		else if (std::string(argv[i]) == "-reset") b_reset = true;
		else if (std::string(argv[i]) == "-ETH") b_ETH = true;
		else if (std::string(argv[i]) == "-n") n = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-e") e = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-d") d = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-i") s_input = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-o") s_output = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-h") { print_help(); return 0; }
	}

	     if (b_rnd) GenerateRandomPositions(s_output, n, e, b_ETH);
	else if (b_playPV) PlayPV(s_input, s_output);
	else if (b_ss) SelectOnScore(s_input, s_output, n);
	else if (b_distribute) Distribute(s_input, s_output);
	else if (b_perft) GeneratePerftPositions(s_output, d, b_ETH);
	//else if (b_char) ;
	else if (b_hist) ScoreHistogram(s_input);
	else if (b_PV)   PVstats(s_input);
	else if (b_FieldValues) FieldValues(s_input);
	else if (b_peek) Peek(s_input, peek_start, peek_num);
	else if (b_prog) Progress(s_input);
	else if (b_test) Test(s_input, e);
	else if (b_reset) Reset(s_input);
	else if (!s_input.empty() && !s_output.empty()) Convert(s_input, s_output);
	else print_help();

	return 0;
}
