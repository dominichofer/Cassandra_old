#include "screen.h"

CScreen Screen;

void CScreen::Initialize(int max_number, int positions)
{
	if (max_number == 0)
		width_number = 0;
	else
		width_number = std::to_string(max_number).length();
	width_nodes = std::stoi(ConfigFile::Configurations["output width nodes"]);
	width_nps = std::stoi(ConfigFile::Configurations["output width nps"]);
	width_PV = std::stoi(ConfigFile::Configurations["output PV length"]) * 3 + 1;
	this->positions = positions;
	positionCounter.store(0, std::memory_order_release);
}

void CScreen::GetLines(std::string& L1, std::string& L2)
{
	if (width_number && (verbose < 3))
	{
		L1.append(std::string(width_number - 2, ' ')).append("# |");
		L2.append(std::string(width_number, '-')).append("+");
	}
	L1.append(" depth|score|       time (s) |");
	L2.append("------+-----+----------------+");

	L1.append(std::string(width_nodes - 10, ' ')).append("nodes (N) |");
	L2.append(std::string(width_nodes, '-')).append("+");

	L1.append(std::string(width_nps - 8, ' ')).append("N/s     |");
	L2.append(std::string(width_nps, '-')).append("+");

	L1.append(" PV").append(std::string(width_PV - 3, ' '));
	L2.append(std::string(width_PV, '-'));
}

void CScreen::printHead()
{
	if (verbose == 2)
	{
		std::string L1, L2;
		GetLines(L1, L2);
		std::unique_lock<std::mutex> lock(mtx);
		std::cout << L1 << std::endl << L2 << std::endl;
	}
	startTime = std::chrono::high_resolution_clock::now();
}

void CScreen::printTail()
{
	if (verbose == 1)
		std::cout << "\r";
	else if (verbose == 2)
	{
		std::string L1, L2;
		GetLines(L1, L2);
		std::unique_lock<std::mutex> lock(mtx);
		std::cout << L2 << std::endl;
	}
}

void CScreen::printStats(const int depth, const int selectivity, const uint64_t P, const uint64_t O, const int score, const std::chrono::milliseconds duration, const uint64_t NodeCounter, const std::string& PV)
{
	if (verbose >= 3)
	{
		std::unique_lock<std::mutex> lock(mtx);
		printf("%6s| %+2.2d |%16s|%*s|%*s| %s\n", DepthSelectivity(depth, selectivity, P, O).c_str(), score,
			time_format(duration).c_str(), width_nodes, ThousandsSeparator(NodeCounter).c_str(),
			width_nps, (duration.count() == 0 ? nullptr : ThousandsSeparator(NodeCounter * 1000ULL / duration.count()).c_str()),
			PV.c_str());
	}
}

void CScreen::printResult(const int number, const int depth, const int selectivity, const uint64_t P, const uint64_t O, const int score, const std::chrono::milliseconds duration, const uint64_t NodeCounter, const std::string& PV)
{
	positionCounter.fetch_add(1, std::memory_order_relaxed);

	if (verbose == 1)
	{
		if (!flag.test_and_set(std::memory_order_acquire))
		{
			if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - lastPrint).count() > 500)
			{
				if (positions)
				{
					std::unique_lock<std::mutex> lock(mtx);
					std::cout << "\r" << ThousandsSeparator(positionCounter.load(std::memory_order_acquire)) << " positions solved.   " << positionCounter.load(std::memory_order_acquire) * 100 / positions << "%    " << ETA();
					fflush(stdout);
				}
				else
				{
					std::unique_lock<std::mutex> lock(mtx);
					std::cout << "\r" << ThousandsSeparator(positionCounter.load(std::memory_order_acquire)) << " positions solved in             " << time_format(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime));
					fflush(stdout);
				}
				lastPrint = std::chrono::high_resolution_clock::now();
			}
			flag.clear(std::memory_order_release);
		}
	}
	else if (verbose == 2)
	{
		std::unique_lock<std::mutex> lock(mtx);
		printf("%*d|%6s| %+2.2d |%16s|%*s|%*s| %s\n", width_number, number, DepthSelectivity(depth, selectivity, P, O).c_str(), score,
			time_format(duration).c_str(), width_nodes, ThousandsSeparator(NodeCounter).c_str(),
			width_nps, (duration.count() == 0 ? nullptr : ThousandsSeparator(NodeCounter * 1000ULL / duration.count()).c_str()),
			PV.c_str());
	}
	else if (verbose == 3)
	{
		std::string L1, L2;
		GetLines(L1, L2);
		std::unique_lock<std::mutex> lock(mtx);
		std::cout << L2 << std::endl;
	}
}

void CScreen::printPosition(const int number, const uint64_t P, const uint64_t O)
{
	if (verbose == 3)
	{
		std::string L1, L2;
		GetLines(L1, L2);
		uint64_t MobilityP = PopCount(PossibleMoves(P, O));
		uint64_t MobilityO = PopCount(PossibleMoves(O, P));
		std::unique_lock<std::mutex> lock(mtx);
		printf("\n*** problem # %d ***\n\n", number);
		printf("%s\n\nPlayer  : discs = %2u    mobility = %2u\nOpponent: discs = %2u    mobility = %2u\n        empties = %2u         ply = %2u\n\n",
			board2D(P, O).c_str(), PopCount(P), MobilityP, PopCount(O), MobilityO, Empties(P, O), 60 - Empties(P, O));
		printf("%s\n%s\n", L1.c_str(), L2.c_str());
	}
}

void CScreen::printSummary(uint64_t NodeCounter)
{
	std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
	if (positionCounter.load(std::memory_order_acquire) == 0)
		printf("No positions were solved.\n");
	else if (verbose == 0)
	{
		std::unique_lock<std::mutex> lock(mtx);
		printf("%16s %6s %*s %*s\n", time_format(duration).c_str(),
			short_time_format(std::chrono::duration_cast<std::chrono::duration<long long, std::pico>>(duration) / positionCounter.load(std::memory_order_acquire)).c_str(),
			width_nodes, ThousandsSeparator(NodeCounter).c_str(),
			width_nps, duration.count() == 0 ? nullptr : ThousandsSeparator(NodeCounter * 1000 / duration.count()).c_str());
	}
	else
	{
		std::unique_lock<std::mutex> lock(mtx);
		printf("%s positions solved in: %16s\nAverage time per position: %6s\nNodes: %s\nSpeed: %s N/s\n",
			ThousandsSeparator(positionCounter.load(std::memory_order_acquire)).c_str(),
            time_format(duration).c_str(),
			short_time_format(std::chrono::duration_cast<std::chrono::duration<long long, std::pico>>(duration) / positionCounter.load(std::memory_order_acquire)).c_str(),
			ThousandsSeparator(NodeCounter).c_str(),
			duration.count() == 0 ? nullptr : ThousandsSeparator(NodeCounter * 1000 / duration.count()).c_str());
	}
}

std::string CScreen::ETA()
{
    int64_t PC = positionCounter.load(std::memory_order_acquire);
	std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - startTime);
	std::string ret = "ETA: ";
	ret.append(time_format(duration * (positions - PC) / PC));
	return ret;
}
