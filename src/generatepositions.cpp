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
		pos_set.insert(CPosition(P, O));
	}
	
	if (ending.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec(pos_set.begin(), pos_set.end());
		std::random_shuffle(vec.begin(), vec.end());
		write_to_file(filename, vec);
	}
	else if (ending.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0)
	{
		std::vector<CDataset_Position_FullScore> vec(pos_set.begin(), pos_set.end());
		std::random_shuffle(vec.begin(), vec.end());
		write_to_file(filename, vec);
	}
	else if (ending.compare(DATASET_ENDING_POSITION_HR) == 0)
	{
		std::vector<CDataset_Position_Score_PV> vec(pos_set.begin(), pos_set.end());
		std::random_shuffle(vec.begin(), vec.end());
		write_to_hr_file(filename, vec);
	}
	std::cout << "Wrote " << pos_set.size() << " positions to " << filename << "." << std::endl;
}

void Play(const std::vector<CPositionScore>& pos_old, std::vector<CPositionScore>& pos_new, std::mutex& mtx, std::atomic<std::size_t>& index, const int thinkDepth)
{
	std::vector<CPositionScore> pos_intern;
	int idx;
	uint64_t P, O;
	while ((idx = index.fetch_add(1, std::memory_order_acquire)) < pos_old.size())
	{
		P = pos_old[idx].P;
		O = pos_old[idx].O;
		assert((P & O) == 0);
		if (!HasMoves(P, O))
		{
			if (HasMoves(O, P))
				std::swap(P, O);
			else
				continue;
		}
		CSearch search(P, O, -64, 64, thinkDepth, NO_SELECTIVITY);
		search.Evaluate();
		if (search.PV(0) == 64) continue;
		PlayStone(P, O, search.PV(0));
		pos_intern.push_back(CPositionScore(P, O, search.score));
		if (idx % 10000 == 0)
		{
			mtx.lock();
			std::cout << "\r" << ThousandsSeparator(idx);
			mtx.unlock();
		}
	}
	mtx.lock();
	pos_new.insert(pos_new.end(), pos_intern.begin(), pos_intern.end());
	mtx.unlock();
}

void GeneratePlayedPositions(const std::string & in, const std::string & out, const std::size_t numPos, int thinkDepth)
{
	auto rnd = std::bind(std::uniform_int_distribution<unsigned int>(0,64), std::mt19937_64(std::chrono::system_clock::now().time_since_epoch().count()));
	int empties;
	std::mutex mtx;
	std::atomic<std::size_t> index;
	std::string in_ending = in.substr(in.rfind(".") + 1, in.length());
	std::string out_ending = out.substr(out.rfind(".") + 1, out.length());
	if (!validEnding(in)) "File ending not supported.";
	if (!validEnding(out)) "File ending not supported.";
	std::vector<CPositionScore> vec_in = read_vector<CPositionScore>(in);
	std::vector<CPositionScore> vec_out;

	//do
	//{
		std::cout << ThousandsSeparator(vec_in.size()); std::cout << " positions with " << Empties(vec_in[0].P, vec_in[0].O) << " empties in vec_in." << std::endl;
		// Calculate next move
		if (Empties(vec_in[0].P, vec_in[0].O)-thinkDepth)
		index.store(0, std::memory_order_release);
		std::vector<std::thread> ThreadVec;
		for (int i = 1; i < std::thread::hardware_concurrency(); i++)
			ThreadVec.push_back(std::thread(Play, std::ref(vec_in), std::ref(vec_out), std::ref(mtx), std::ref(index), thinkDepth));
		Play(vec_in, vec_out, mtx, index, thinkDepth);
		for (auto& it : ThreadVec) it.join();
		std::cout << ThousandsSeparator(vec_out.size()); std::cout << " positions with " << Empties(vec_out[0].P, vec_out[0].O) << " empties in vec_out." << std::endl;

		// Select -8 <= score <= +8
		vec_out.erase(std::remove_if(vec_out.begin(), vec_out.end(), [](const CPositionScore& pos){ return pos.score < -8 || pos.score > +8; }), vec_out.end());
		std::cout << ThousandsSeparator(vec_out.size()); std::cout << " positions after selecting." << std::endl;
		
		// Sort + unique
		std::sort(vec_out.begin(), vec_out.end());
		vec_out.erase(std::unique(vec_out.begin(), vec_out.end()), vec_out.end());
		std::cout << ThousandsSeparator(vec_out.size()); std::cout << " positions after sort and unique." << std::endl;

		// Shuffle
		std::random_shuffle(vec_out.begin(), vec_out.end());

		empties = Empties(vec_out[0].P, vec_out[0].O);
		std::string tmp = out.substr(0, out.rfind("x")).append(std::to_string(empties)).append(out.substr(out.rfind("x") + 1, out.length()));

		if (out_ending.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
		{
			std::vector<CDataset_Position_Score_PV> vec(vec_out.begin(), vec_out.begin() + numPos);
			write_to_file(tmp, vec);
		}
		else if (out_ending.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0)
		{
			std::vector<CDataset_Position_FullScore> vec(vec_out.begin(), vec_out.begin() + numPos);
			write_to_file(tmp, vec);
		}
		else if (out_ending.compare(DATASET_ENDING_POSITION_HR) == 0)
		{
			std::vector<CDataset_Position_Score_PV> vec(vec_out.begin(), vec_out.begin() + numPos);
			write_to_hr_file(tmp, vec);
		}
		std::cout << " Wrote " << numPos << " positions to " << tmp << std::endl;
		

		tmp = out.substr(0, out.rfind("x")).append(std::to_string(empties)).append("_tmp").append(out.substr(out.rfind("x") + 1, out.length()));

		if (out_ending.compare(DATASET_ENDING_POSITION_SCORE_PV) == 0)
		{
			std::vector<CDataset_Position_Score_PV> vec(vec_out.begin(), vec_out.end());
			write_to_file(tmp, vec);
		}
		else if (out_ending.compare(DATASET_ENDING_POSITION_FULLSCORE) == 0)
		{
			std::vector<CDataset_Position_FullScore> vec(vec_out.begin(), vec_out.end());
			write_to_file(tmp, vec);
		}
		else if (out_ending.compare(DATASET_ENDING_POSITION_HR) == 0)
		{
			std::vector<CDataset_Position_Score_PV> vec(vec_out.begin(), vec_out.end());
			write_to_hr_file(tmp, vec);
		}
		std::cout << " Wrote " << vec_out.size() << " positions to " << tmp << std::endl;
		vec_in = vec_out;
		vec_out.clear();
	//} while (empties > 0);
}

void GenPerft(const uint64_t P, const uint64_t O, std::set<CPosition> & pos_set, const int depth)
{
	if (depth == 0){
		if (pos_set.count(CPosition(P, O)) == 0)
			pos_set.insert(CPosition(P, O));
		return;
	}

	uint64_t BitBoardPossible = PossibleMoves(P, O);

	if (BitBoardPossible == 0){
		if (HasMoves(O, P))
			return GenPerft(O, P, pos_set, depth);
		else
			return;
	}
		
	unsigned long move;
	uint64_t flipped;

	while (BitBoardPossible)
	{
		move = BitScanLSB(BitBoardPossible);
		RemoveLSB(BitBoardPossible);
		flipped = flip(P, O, move);
		GenPerft(O ^ flipped, P ^ flipped ^ (1ULL << move), pos_set, depth-1);
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
