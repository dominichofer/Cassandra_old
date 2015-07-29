#include "generatepositions.h"

void GenerateRandomPositions(const std::string & filename, const std::size_t numPos, const int numEmpties, const bool ETH)
{
	auto rnd = std::bind(std::uniform_int_distribution<unsigned int>(0,64), std::mt19937_64(std::chrono::system_clock::now().time_since_epoch().count()));
	std::set<CPosition> pos_set;
	std::string ending = filename.substr(filename.rfind(".") + 1, filename.length());
	if (!validEnding(ending)) "File ending not supported.";


	while (pos_set.size() < numPos)
	{
		uint64_t P, O;
		ResetPosition(P, O, ETH);
		int plies = 60 - numEmpties;
		while (plies > 0)
		{
			uint64_t possibles = PossibleMoves(P, O);
			if (!possibles)
			{
				std::swap(P, O);
				possibles = PossibleMoves(P, O);
				if (!possibles)
				{
					ResetPosition(P, O, ETH);
					plies = 60 - numEmpties;
					continue;
				}
			}
			for (int k = rnd() % PopCount(possibles); k > 0; k--)
				RemoveLSB(possibles);
			PlayStone(P, O, BitScanLSB(possibles));
			//PlayStone(P, O, PDep(1ULL << (rnd() % PopCount(possibles)), possibles));
			plies--;
		}
		if (pos_set.count(CPosition(P, O)) == 0)
			pos_set.insert(CPosition(P, O));
	}
	
	if (ending.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec;
		for (const auto& it : pos_set)
			vec.push_back(CDataset_Position_Score_PV(it));
		std::random_shuffle(vec.begin(), vec.end());
		write_to_file(filename, vec);
	}
	else if (ending.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0)
	{
		std::vector<CDataset_Position_FullScore> vec;
		for (const auto& it : pos_set)
			vec.push_back(CDataset_Position_FullScore(it));
		std::random_shuffle(vec.begin(), vec.end());
		write_to_file(filename, vec);
	}
	else if (ending.compare(DATASET_ENDING_POSITION_HR) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec;
		for (const auto& it : pos_set)
			vec.push_back(CDataset_Position_Score_PV(it));
		std::random_shuffle(vec.begin(), vec.end());
		write_to_hr_file(filename, vec);
	}
	std::cout << "Wrote " << pos_set.size() << " positions to " << filename << "." << std::endl;
}

void GenPerft(const uint64_t P, const uint64_t O, std::set<CPosition> & pos_set, const int depth)
{
	if (depth == 0){
		if (pos_set.count(CPosition(P, O)) == 0)
			pos_set.insert(CPosition(P, O));
		return;
	}

	uint64_t BitBoardPossible = PossibleMoves(P, O);

	if (BitBoardPossible == 0)
		if (HasMoves(O, P))
			return GenPerft(O, P, pos_set, depth-1);
		else
			return;
		
	unsigned long LSB; // Holds the Least Significant Bit
	uint64_t flipped;

	while (BitBoardPossible)
	{
		LSB = BitScanLSB(BitBoardPossible);
		RemoveLSB(BitBoardPossible);
		flipped = flip(P, O, LSB);
		GenPerft(O ^ flipped, P ^ flipped ^ (1ULL << LSB), pos_set, depth-1);
	}
}

void GeneratePerftPositions(const std::string & filename, const int depth, const bool ETH)
{
	std::set<CPosition> pos_set;
	std::string ending = filename.substr(filename.rfind(".") + 1, filename.length());
	if (!validEnding(ending)) "File ending not supported.";
	uint64_t P, O;
	ResetPosition(P, O, ETH);
	if (depth == 0)
		GenPerft(P, O, pos_set, depth);
	else
	{
		if (ETH)
		{
			uint64_t tmp = PossibleMoves(P, O);
			PlayStone(P, O, BitScanLSB(tmp));
			GenPerft(P, O, pos_set, depth-1);

			ResetPosition(P, O, ETH);
			RemoveLSB(tmp);
			PlayStone(P, O, BitScanLSB(tmp));
			GenPerft(P, O, pos_set, depth-1);
		}
		else
		{
			PlayStone(P, O, BitScanLSB(PossibleMoves(P, O)));
			GenPerft(P, O, pos_set, depth-1);
		}
	}
	
	if (ending.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec;
		for (const auto& it : pos_set)
			vec.push_back(CDataset_Position_Score_PV(it));
		std::random_shuffle(vec.begin(), vec.end());
		write_to_file(filename, vec);
	}
	else if (ending.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0)
	{
		std::vector<CDataset_Position_FullScore> vec;
		for (const auto& it : pos_set)
			vec.push_back(CDataset_Position_FullScore(it));
		std::random_shuffle(vec.begin(), vec.end());
		write_to_file(filename, vec);
	}
	else if (ending.compare(DATASET_ENDING_POSITION_HR) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec;
		for (const auto& it : pos_set)
			vec.push_back(CDataset_Position_Score_PV(it));
		std::random_shuffle(vec.begin(), vec.end());
		write_to_hr_file(filename, vec);
	}

	std::cout << "Wrote " << pos_set.size() << " positions to " << filename << "." << std::endl;
}
