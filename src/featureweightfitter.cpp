#include "featureweightfitter.h"

typedef CMatrix_CSR<uint8_t, unsigned int> Matrix;

std::vector<CPositionScore> read_vector_next_pos(const std::string& filename)
{
	std::vector<CPositionScore> out;
	std::vector<CDataset_Position_Score_PV> in = read_vector<CDataset_Position_Score_PV>(filename);
	for (auto& pos : in)
	{
		out.push_back(static_cast<CPositionScore>(pos));
		if (pos.PV[0] == 64)
			std::swap(pos.P, pos.O);
		else
			PlayStone(pos.P, pos.O, pos.PV[0]);
		pos.score = -pos.score;
		out.push_back(static_cast<CPositionScore>(pos));
	}
	return out;
}

void Load_Data(const std::vector<std::string>& Filenames, Matrix& A, std::vector<double>& scores, const bool next)
{
	std::vector<CPositionScore> M;
	std::vector<CPositionScore> tmp;

	// Load files into a position-score-matrix
	for (const auto& filename : Filenames) {
		if (next)
			tmp = read_vector_next_pos(filename);
		else
			tmp = read_vector<CPositionScore>(filename);
		M.insert(M.end(), tmp.begin(), tmp.end());
	}
	tmp.clear();
	
	const int size = M.size();

	#pragma omp parallel
	{
		std::vector<int> Indices;
		std::map<Matrix::size_type, Matrix::value_type> map;
		
		#pragma omp for nowait schedule(static, 1024)
		for (int i = 0; i < size; ++i)
		{
			map.clear();
			FillConfigurationVecOffsetted(M[i].P, M[i].O, Indices);

			for (const auto & it : Indices)
				map[it]++;

			#pragma omp critical
			{
				for (const auto& it : map)
					A.push_back(it.first, it.second);
				A.endRow();
				scores.push_back(M[i].score);
			}
		}
	}
}

std::size_t Calc_and_Remove_Active_Configurations(Matrix& A, const unsigned int Threshold)
{
	const int size1 = A.n();
	const int size2 = A.m();
	std::vector<std::size_t> ones(size1);
	for (auto& it : ones) it = 1;

	std::vector<std::size_t> Counters = A.ATx(ones);

	std::vector<bool> ColumnSieve(size2);
	std::size_t ActiveCounter = 0;
	for (int i = 0; i < size2; ++i)
	{
		if (Counters[i] >= Threshold)
			ActiveCounter++;
		ColumnSieve[i] = (Counters[i] > 0 && Counters[i] < Threshold);
	}

	// Set Zero
	#pragma omp parallel for schedule(static, 1024)
	for (int i = 0; i < size1; ++i)
	{
		for (Matrix::size_type j = A.row_starts[i]; j < A.row_starts[i + 1]; ++j)
			if (ColumnSieve[A.data[j]])
				A.data[j] = 0;
	}

	//A.remove_zeros();

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
	}

	for (int i = 0; i < size2; ++i)
		if (C[i] == 0)
			C[i] = 1.0;
		else
			C[i] = 1.0 / C[i];

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
	for (const auto& it : FileNames)
		printf_s("\t%s\n", it.c_str());

	printf_s("Using starting vector: ");
	if (file_start_x.empty()){
		printf_s("no\n");
		x = std::vector<double>(Features::ReducedSize);
	}
	else{
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
	
	printf_s("Number of positions: %s\n", ThousandsSeparator(A.n()).c_str());
	printf_s("Number of non-zeros: %s\n", ThousandsSeparator(A.nnz()).c_str());

	printf_s("Calculating active configurations...");
	startTime = std::chrono::high_resolution_clock::now();
	const std::size_t Number_of_Active_Configurations = Calc_and_Remove_Active_Configurations(A, Threshold);
	endTime = std::chrono::high_resolution_clock::now();
	printf_s("done!\t %14s\n", time_format(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)).c_str());

	printf_s("Configurations: %d\n", Features::ReducedSize);
	printf_s("Active configurations: %d\n", Number_of_Active_Configurations);
	printf_s("Active percentage: %.2f%%\n\n", Number_of_Active_Configurations * 100.0 / Features::ReducedSize);

	printf_s("Calculating preconditioner...");
	startTime = std::chrono::high_resolution_clock::now();
	C = JacobiPreconditioner(A);
	endTime = std::chrono::high_resolution_clock::now();
	printf_s("done!\t %14s\n", time_format(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)).c_str());
		
	printf_s("Calculating vectors r_0, h_0, d_0...");
	startTime = std::chrono::high_resolution_clock::now();
	r = A.ATx(scores) - A.ATAx(x);
	for (int i = 0; i < Features::ReducedSize; ++i) h[i] = C[i] * r[i];
	d = h;
	endTime = std::chrono::high_resolution_clock::now();
	printf_s("done!\t\t %14s\n\n", time_format(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)).c_str());

	printf_s(" it | norm(res) | avg(abs(err)) |  avg(err)  |  stdev(err)  |       time      \n");
	printf_s("----+-----------+---------------+------------+--------------+-----------------\n");
	
	res = norm(r);
	DoStatsMuSigma(A, x, scores, mu, sigma, avg_abs_err);
	printf_s("%4d| %.2e |  %.4e  | % .2e | % .4e |\n", 0, res, avg_abs_err, mu, sigma);

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

		if (k % 10 == 0)
		{
			DoStatsMuSigma(A, x, scores, mu, sigma, avg_abs_err);
			endTime = std::chrono::high_resolution_clock::now();
			//printf_s(" it | norm(res) | avg(abs(err)) |  avg(err)  |  stdev(err)  |      time     \n");
			printf_s("%4d| %.2e |  %.4e  | % .2e | % .4e | %14s\n", k, res, avg_abs_err, mu, sigma, time_format(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)).c_str());
		}
		else
		{
			endTime = std::chrono::high_resolution_clock::now();
			//printf_s(" it | norm(res) | avg(abs(err)) |  avg(err)  |  stdev(err)  |      time     \n");
			printf_s("%4d| %.2e |               |            |              | %14s\n", k, res, time_format(std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)).c_str());
		}

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
		else if (std::string(argv[i]) == "-h") { Print_help(); return 0; }
	}
	
	//ConfigFile::Initialize(argv[0], std::string("ProjectBrutus.ini"));
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

	Features::Finalize();
	return 0;
}