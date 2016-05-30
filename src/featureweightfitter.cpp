#include "featureweightfitter.h"

typedef CMatrix_CSR<uint8_t, unsigned int> Matrix;

void Load_Data(const std::vector<std::string>& Filenames, Matrix& A, std::vector<double>& scores, const bool next)
{
	std::vector<CDataset_Position_Score_PV> in;
	std::vector<CPositionScore> in2;

	// Load all files into "in"
	for (const auto& filename : Filenames)
	{
		std::vector<CDataset_Position_Score_PV> tmp = read_vector<CDataset_Position_Score_PV>(filename);
		in.insert(in.end(), tmp.begin(), tmp.end());
	}
	
	// Transform all positions
	for (auto& pos : in) in2.push_back(static_cast<CPositionScore>(pos));

	// Generate and transform PV-next-positions
	if (next)
	{
		for (int i = 0; i < in.size(); ++i)
		{
			PlayStone(in[i].P, in[i].O, in[i].PV[0]);
			in[i].score = -in[i].score;
			in2.push_back(static_cast<CPositionScore>(in[i]));
		}
	}

	in.clear(); // Free memory
	
	const std::size_t size = in2.size();

	#pragma omp parallel
	{
		std::vector<int> indices(Features::NumberOfPatternWithSymmetrie);
		std::map<Matrix::size_type, Matrix::value_type> map;

		#pragma omp for schedule(static, 1024)
		for (int i = 0; i < size; ++i)
		{
			FillConfigurationVecOffsetted(in2[i].P, in2[i].O, indices.begin());
			
			map.clear();
			for (const auto& it : indices) map[it]++;

			#pragma omp critical
			{
				for (const auto& it : map) A.push_back(it.first, it.second);
				A.endRow();
				scores.push_back(in2[i].score);
			}
		}
	}
}

std::size_t Filter_non_Active_Configurations(Matrix& A, const std::size_t Threshold)
{
	const int size1 = A.n();
	const int size2 = A.m();
	std::vector<std::size_t> Counter(size2);
	std::size_t ActiveCounter = 0;

	#pragma omp parallel
	{
		std::vector<std::size_t> local_Counter(size2);

		#pragma omp for nowait schedule(static, 1024)
		for (long long i = 0; i < size1; ++i)
		{
			for (Matrix::size_type j = A.row_starts[i]; j < A.row_starts[i + 1]; ++j)
				local_Counter[A.col_indices[j]] += A.data[j];
		}

		#pragma omp critical
		{
			for (std::size_t i = 0; i < size2; ++i)
				Counter[i] += local_Counter[i];
		}

		#pragma omp barrier
		
		// Set Zero
		#pragma omp for schedule(static, 1024)
		for (long long i = 0; i < size1; ++i)
		{
			for (Matrix::size_type j = A.row_starts[i]; j < A.row_starts[i + 1]; ++j)
				if (Counter[A.col_indices[j]] < Threshold)
					A.data[j] = 0;
		}
	}

	//std::map<unsigned int, unsigned int> HMap;
	//for (int i = 0; i < size2; ++i) HMap[Counter[i]]++;

	//for (auto it : HMap)
	//	std::cout << "Penis: " << it.first << "   " << it.second << std::endl;

	//A.remove_zeros();
	
	for (int i = 0; i < size2; ++i)
		if (Counter[i] >= Threshold)
			ActiveCounter++;

	return ActiveCounter;
}

std::vector<double> JacobiPreconditioner(const Matrix& A)
{
	const int size1 = A.n();
	const int size2 = A.m();
	std::vector<double> C(size2);

	#pragma omp parallel
	{
		std::vector<double> local_C(size2);

		#pragma omp for nowait schedule(static, 1024)
		for (int i = 0; i < size1; ++i)
		{
			for (Matrix::size_type j = A.row_starts[i]; j < A.row_starts[i + 1]; ++j)
				local_C[A.col_indices[j]] += static_cast<double>(A.data[j]) * static_cast<double>(A.data[j]);
		}
		#pragma omp critical
		{
			for (std::size_t i = 0; i < size2; ++i)
				C[i] += local_C[i];
		}

		#pragma omp barrier

		#pragma omp for schedule(static, 1024)
		for (int i = 0; i < size2; ++i)
		{
			if (C[i] == 0)
				C[i] = 1.0;
			else
				C[i] = 1.0 / C[i];
		}
	}

	return C;
}

void DoStatsMuSigma(const Matrix& A, const std::vector<double>& x, const std::vector<double>& scores, double& mu, double& sigma, double& avg_abs_err)
{
	assert(A.m() == x.size());
	CRunningStatistic<double> stats = CRunningStatistic<double>();
	avg_abs_err = 0;
	std::size_t counter = 0;

	std::vector<double> err = A * x - scores;

	for (int i = 0; i < err.size(); ++i)
	{
		stats.Add(err[i]);
		avg_abs_err += (std::abs(err[i]) - avg_abs_err) / static_cast<double>(++counter);
	}
	mu = stats.mu();
	sigma = stats.sigma();
}

void SolveInRAM(const std::vector<std::string>& FileNames, const std::string& file_start_x, const std::string& file_end_x, const unsigned int Iterations, const unsigned int Threshold, const bool next)
{
	std::chrono::high_resolution_clock::time_point startTime, endTime;
	std::vector<double> C(Features::ReducedSize);
	std::vector<double> d(Features::ReducedSize);
	std::vector<double> r(Features::ReducedSize);
	std::vector<double> h(Features::ReducedSize);
	std::vector<double> z(Features::ReducedSize);
	std::vector<double> x;
	std::vector<double> scores;
	Matrix A(Features::ReducedSize);
	double alpha, beta, tmp1, res, mu, sigma, avg_abs_err;
	const double tol = 1;

	printf_s("Matrices:\n");
	for (const auto& it : FileNames) printf_s("\t%s\n", it.c_str());

	printf_s("Using starting vector: ");
	if (file_start_x.empty()) {
		printf_s("no\n");
		x = std::vector<double>(Features::ReducedSize);
	}
	else {
		printf_s("%s\n", file_start_x.c_str());
		x = read_vector<double>(file_start_x);
	}

	printf_s("Iteration limit: %d\n", Iterations);
	printf_s("Active configuration threshold: %d\n\n", Threshold);

	if (next)
		printf_s("Loading files and calculating next PV position...");
	else
		printf_s("Loading files...");
	startTime = std::chrono::high_resolution_clock::now();
	Load_Data(FileNames, A, scores, next);
	endTime = std::chrono::high_resolution_clock::now();
	printf_s("done!\t\t\t %14s\n", time_format(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)).c_str());

	printf_s("Writing matrix to file...");
	startTime = std::chrono::high_resolution_clock::now();
	A.write_to_file("tmp.csr");
	std::vector<float> b(scores.size());
	for (std::size_t i = 0; i < scores.size(); i++) b[i] = scores[i];
	write_to_file("tmp.vec", b);
	endTime = std::chrono::high_resolution_clock::now();
	printf_s("done!\t\t\t %14s\n", time_format(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)).c_str());
	return;

	printf_s("Calculating active configurations...");
	startTime = std::chrono::high_resolution_clock::now();
	const std::size_t Number_of_Active_Configurations = Filter_non_Active_Configurations(A, Threshold);
	endTime = std::chrono::high_resolution_clock::now();
	printf_s("done!\t %14s\n", time_format(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)).c_str());

	printf_s("Number of Pattern: %d\n", Features::NumberOfPattern);
	printf_s("Number of Pattern with symmetrie: %d\n", Features::NumberOfPatternWithSymmetrie);
	printf_s("Configurations: %d\n", Features::ReducedSize); 
	printf_s("Active configurations: %d\n", Number_of_Active_Configurations);
	printf_s("Active percentage: %.2f%%\n\n", Number_of_Active_Configurations * 100.0 / Features::ReducedSize);
	
	printf_s("Number of positions: %s\n", ThousandsSeparator(A.n()).c_str());
	printf_s("Number of non-zeros: %s\n", ThousandsSeparator(A.nnz()).c_str());
	printf_s("Number of zeros: %s\n", ThousandsSeparator(A.nz()).c_str());

	printf_s("Calculating preconditioner...");
	startTime = std::chrono::high_resolution_clock::now();
	C = JacobiPreconditioner(A);
	endTime = std::chrono::high_resolution_clock::now();
	printf_s("done!\t %14s\n", time_format(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)).c_str());
		
	printf_s("Calculating vectors r_0, h_0, d_0...");
	startTime = std::chrono::high_resolution_clock::now();
	r = A.ATx(scores - A * x);
	for (unsigned int i = 0; i < Features::ReducedSize; ++i) h[i] = C[i] * r[i];
	d = h;
	endTime = std::chrono::high_resolution_clock::now();
	printf_s("done!\t\t %14s\n\n", time_format(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)).c_str());

	//printf_s(" it | norm(res) | avg(abs(err)) |  avg(err)  |  stdev(err)  |       time      \n");
	//printf_s("----+-----------+---------------+------------+--------------+-----------------\n");

	printf_s(" it |  norm(err)  |  norm(res)  | norm(err - res) \n");
	printf_s("----+-------------+-------------+-----------------\n");
	
	res = norm(r);
	DoStatsMuSigma(A, x, scores, mu, sigma, avg_abs_err);
	//printf_s("%4d| %.2e |  %.4e  | % .2e | % .4e |\n", 0, res, avg_abs_err, mu, sigma);
	printf_s("%4d %.3e %.3e %.3e\n", 0, norm(A*x - scores), norm(A.ATAx(x) - A.ATx(scores)), norm(r));

	for (unsigned int k = 1; k <= Iterations; ++k)
	{
		startTime = std::chrono::high_resolution_clock::now();
		z = A.ATAx(d);
		tmp1 = dot(r, h);
		alpha = tmp1 / dot(d, z);
		x = x + alpha * d;
		r = r - alpha * z;
		for (int i = 0; i < Features::ReducedSize; ++i) h[i] = C[i] * r[i];
		beta = dot(r, h) / tmp1;
		d = h + beta * d;
		res = norm(r);

		//if (k % 10 == 0)
		//{
		//	DoStatsMuSigma(A, x, scores, mu, sigma, avg_abs_err);
		//	endTime = std::chrono::high_resolution_clock::now();
		//	//printf_s(" it | norm(res) | avg(abs(err)) |  avg(err)  |  stdev(err)  |      time     \n");
		//	printf_s("%4d| %.2e |  %.4e  | % .2e | % .4e | %14s\n", k, res, avg_abs_err, mu, sigma, time_format(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)).c_str());
		//}
		//else
		//{
			endTime = std::chrono::high_resolution_clock::now();
			//printf_s(" it | norm(res) | avg(abs(err)) |  avg(err)  |  stdev(err)  |      time     \n");
			printf_s("%4d %.3e %.3e %.3e\n", k, norm(A*x - scores), norm(A.ATAx(x) - A.ATx(scores)), norm(r));
			//printf_s("%4d| %.2e |               |            |              | %14s\n", k, norm(A*x-scores), time_format(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)).c_str());
		//}
		if (res < tol) break;
	}
	printf_s("\nConjugate gradient method terminated!\n");

	printf_s("Saving vector x...");
	write_to_file(file_end_x, x);
	printf_s("done!\n");
}

void Print_help()
{
	std::cout << std::endl << "Calculates the feature weights." << std::endl
						   << "Attributes:" << std::endl
						   << "-f\tsolved position files" << std::endl
						   << "-initial\tinitial coefficient file" << std::endl
						   << "-x\toutput coefficient file" << std::endl 
						   << "-iterations\tnumber of iterations of the CG-method" << std::endl
						   << "-n\tlower treshold for number of active configurations" << std::endl
						   << "-h\thelp" << std::endl;
}

int main(int argc, char* argv[])
{
	int Threshold = 0;
	std::vector<std::string> filename_vec;
	std::string file_start_x;
	std::string file_end_x;
	std::string pattern;
	int Iterations = 100;
	bool next = false;

	for (int i = 0; i < argc; ++i) 
	{
		if (std::string(argv[i]) == "-f") {
			++i;
			while ((i < argc) && (static_cast<char>(*argv[i]) != '-'))
				filename_vec.push_back(std::string(argv[i++]));
			--i;
		}
		else if (std::string(argv[i]) == "-initial") file_start_x = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-x") file_end_x = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-iterations") Iterations = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-n") Threshold = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-next") next = true;
		else if (std::string(argv[i]) == "-pattern") {
			++i;
			while ((i < argc) && (static_cast<char>(*argv[i]) != '-'))
				pattern.append(std::string(argv[i++])).append(" ");
			--i;
		}
		else if (std::string(argv[i]) == "-h") { Print_help(); return 0; }
	}
	
	ConfigFile::Initialize("C:\\Cassandra\\config.ini");
	if (!pattern.empty()) ConfigFile::Configurations["active pattern"] = pattern;
	Features::Initialize();
	
	//filename_vec.push_back(std::string("C:\\Cassandra\\pos\\rnd_e6_1M.psp"));
	//filename_vec.push_back(std::string("C:\\Cassandra\\pos\\rnd_e7_1M.psp"));
	//filename_vec.push_back(std::string("C:\\Cassandra\\pos\\rnd_e8_1M.psp"));
	//filename_vec.push_back(std::string("C:\\Cassandra\\pos\\rnd_e9_1M.psp"));
	//filename_vec.push_back(std::string("C:\\Cassandra\\pos\\rnd_e10_1M.psp"));
	//SolveInRAM(filename_vec, file_start_x, std::string("C:\\Cassandra\\Penis.b"), 100, 4, true);
	//return 0;
	
	if (filename_vec.size())
		SolveInRAM(filename_vec, file_start_x, file_end_x, Iterations, Threshold, next);

	return 0;
}