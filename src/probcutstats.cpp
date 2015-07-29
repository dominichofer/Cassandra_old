#pragma once
#include <cstdint>
#include <vector>
#include <iostream>
#include <fstream>
#include <functional>
#include "datamanipulation.h"
#include "statistics.h"

class CBitmap
{
public:
	int width, height;
	CBitmap(int width, int height);
	~CBitmap() { delete[] pixels; }
	void SetPixel(int x, int y, int8_t r, int8_t g, int8_t b);
	void SetPixel(int x, int y, float value);
	void Clear(int8_t r, int8_t g, int8_t b);
	void Clear();
	void DrawLine(float x1, float y1, float x2, float y2, int8_t r, int8_t g, int8_t b);
	void DrawDigit(int digit, int x, int y, int8_t r, int8_t g, int8_t b);
	template <class T> void DrawNumber(const char* format,T number, int x, int y, int8_t r, int8_t g, int8_t b);
	void DrawHeatmap(int x, int y, const uint64_t* Heatmap);
	void DrawHistogram(int x, int y, const uint64_t* Histogram);
	void Save(const std::string& filename);
private:
	uint8_t* pixels;
	std::vector<uint8_t> get_4_bytes(const uint32_t & data);
	std::vector<uint8_t> get_2_bytes(const uint16_t & data);
	void insert_4_bytes(std::vector<uint8_t> & dest, const uint32_t & data);
	void insert_2_bytes(std::vector<uint8_t> & dest, const uint16_t & data);
	size_t bitmap_encode_rgb(const uint8_t* rgb, int width, int height, uint8_t*& output);
};

CBitmap::CBitmap(int height, int width) : width(width), height(height)
{
	pixels = new uint8_t[width * height * 3];
	memset(pixels, 0, width * height * 3);
}
void CBitmap::SetPixel(int x, int y, int8_t r, int8_t g, int8_t b)
{
	pixels[(x * width + y) * 3 + 0] = r;
	pixels[(x * width + y) * 3 + 1] = g;
	pixels[(x * width + y) * 3 + 2] = b;
}
void CBitmap::SetPixel(int x, int y, float value)
{
	     if (value < 0.25) SetPixel(x, y, 0, value * 4.0 * 255, 255);
	else if (value < 0.50) SetPixel(x, y, 0, 255, (2.0 - value * 4.0) * 255);
	else if (value < 0.75) SetPixel(x, y, (value * 4.0 - 2.0) * 255, 255, 0);
	else                   SetPixel(x, y, 255, (3.0 - value * 4.0) * 255, 0);
}
void CBitmap::Clear(int8_t r, int8_t g, int8_t b)
{
	for (int x = 0; x < height; x++)
		for (int y = 0; y < width; y++)
			SetPixel(x, y, r, g, b);
}
void CBitmap::Clear()
{
	memset(pixels, 0, width * height * 3);
}
void CBitmap::DrawLine(float x1, float y1, float x2, float y2, int8_t r, int8_t g, int8_t b)
{
	// Bresenham's line algorithm
	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
	if (steep) {std::swap(x1, y1); std::swap(x2, y2); }
	if (x1 > x2) { std::swap(x1, x2); std::swap(y1, y2); }
 
	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);
	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = static_cast<int>(y1);
	const int maxX = static_cast<int>(x2);
 
	for (int x = static_cast<int>(x1); x <= maxX; x++)
	{
		if(steep) SetPixel(y, x, r, g, b);
		else      SetPixel(x, y, r, g, b);
 
		error -= dy;
		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}
}
void CBitmap::DrawDigit(int digit, int x, int y, int8_t r, int8_t g, int8_t b)
{
	int* tmp;
	switch (digit)
	{
	case 0: tmp = new int[15]{1,1,1,1,0,1,1,0,1,1,0,1,1,1,1}; break;
	case 1: tmp = new int[15]{0,0,1,0,0,1,0,0,1,0,0,1,0,0,1}; break;
	case 2: tmp = new int[15]{1,1,1,0,0,1,1,1,1,1,0,0,1,1,1}; break;
	case 3: tmp = new int[15]{1,1,1,0,0,1,0,1,1,0,0,1,1,1,1}; break;
	case 4: tmp = new int[15]{1,0,1,1,0,1,1,1,1,0,0,1,0,0,1}; break;
	case 5: tmp = new int[15]{1,1,1,1,0,0,1,1,1,0,0,1,1,1,1}; break;
	case 6: tmp = new int[15]{1,1,1,1,0,0,1,1,1,1,0,1,1,1,1}; break;
	case 7: tmp = new int[15]{1,1,1,0,0,1,0,0,1,0,0,1,0,0,1}; break;
	case 8: tmp = new int[15]{1,1,1,1,0,1,1,1,1,1,0,1,1,1,1}; break;
	case 9: tmp = new int[15]{1,1,1,1,0,1,1,1,1,0,0,1,1,1,1}; break;
	default: tmp = new int[15]{0,0,0,0,0,0,0,0,0,0,0,0,0,1,0}; break; // Dot
	}
	for (int i = 0; i < 5; i++)
		for (int j = 0; j < 3; j++)
			if (tmp[i*3+j])
				SetPixel(x+i,y+j,r,g,b);
	delete[] tmp;
}

template <class T>
void CBitmap::DrawNumber(const char* format,T number, int x, int y, int8_t r, int8_t g, int8_t b)
{
	char str[128];
	sprintf(str, format, number);
	std::string s(str);
	int j = 0;
	for (int i = 0; i < s.size(); i++, j += 4)
	{
		std::string t = s.substr(i, 1);
		if (t.compare(".") == 0)
			DrawDigit(10, x, y + j, r, g, b);
		else
			DrawDigit(std::stoi(t), x, y + j, r, g, b);
	}
}
void CBitmap::DrawHeatmap(int x, int y, const uint64_t* HeatMap)
{
	// Draw Boarders
	DrawLine(x    , y    , x    , y+130, 0, 0, 0);
	DrawLine(x    , y+130, x+130, y+130, 0, 0, 0);
	DrawLine(x+130, y+130, x+130, y    , 0, 0, 0);
	DrawLine(x+130, y    , x    , y    , 0, 0, 0);

	// Draw zero markers
	SetPixel(x+ 65,y    , 128, 128, 128);
	SetPixel(x    ,y+ 65, 128, 128, 128);
	SetPixel(x+ 65,y+130, 128, 128, 128);
	SetPixel(x+130,y+ 65, 128, 128, 128);

	uint64_t max = 1;
	for (int i = 0; i < 129; i++)
		for (int j = 0; j < 129; j++)
			if (HeatMap[i*129+j] > max)
				max = HeatMap[i*129+j];

	for (int i = 0; i < 129; i++)
		for (int j = 0; j < 129; j++)
			SetPixel(x+1+i,y+1+j, static_cast<float>(HeatMap[i*129+j]) / max);
}
void CBitmap::DrawHistogram(int x, int y, const uint64_t* Histogram)
{
	// Draw Boarders
	DrawLine(x    , y    , x    , y+130, 0, 0, 0);
	DrawLine(x    , y+130, x+130, y+130, 0, 0, 0);
	DrawLine(x+130, y+130, x+130, y    , 0, 0, 0);
	DrawLine(x+130, y    , x    , y    , 0, 0, 0);

	// Draw zero markers
	SetPixel(x+ 65,y    , 128, 128, 128);
	SetPixel(x    ,y+ 65, 128, 128, 128);
	SetPixel(x+ 65,y+130, 128, 128, 128);
	SetPixel(x+130,y+ 65, 128, 128, 128);

	uint64_t max = 1;
	for (int i = 0; i < 129; i++)
		if (Histogram[i] > max)
			max = Histogram[i];

	for (int i = 0; i < 129; i++)
		DrawLine(x+129,y+1+i,x+129-static_cast<float>(Histogram[i])*128/max, y+1+i, 255, 0, 0);
}

void CBitmap::Save(const std::string& filename)
{
	uint8_t* output = nullptr;
	std::size_t output_size = bitmap_encode_rgb(pixels, width, height, output);
	std::ofstream file_output;
	file_output.open(filename, std::ios_base::out | std::ios_base::binary);
	file_output.write((const char*)output, output_size);
	file_output.close();
	delete [] output;
}

std::vector<uint8_t> CBitmap::get_4_bytes(const uint32_t & data)
{
	std::vector<uint8_t> ret;
	uint8_t* tmp = (uint8_t*)&data;
	ret.push_back(tmp[0]);
	ret.push_back(tmp[1]);
	ret.push_back(tmp[2]);
	ret.push_back(tmp[3]);
return ret;
}
std::vector<uint8_t> CBitmap::get_2_bytes(const uint16_t & data)
{
	std::vector<uint8_t> ret;
	uint8_t* tmp = (uint8_t*)&data;
	ret.push_back(tmp[0]);
	ret.push_back(tmp[1]);
	return ret;
}
void CBitmap::insert_4_bytes(std::vector<uint8_t> & dest, const uint32_t & data)
{
	std::vector<uint8_t> separated_data = get_4_bytes(data);
	std::copy(separated_data.begin(), separated_data.end(), back_inserter(dest));
}
void CBitmap::insert_2_bytes(std::vector<uint8_t> & dest, const uint16_t & data)
{
	std::vector<uint8_t> separated_data = get_2_bytes(data);
	copy(separated_data.begin(), separated_data.end(), back_inserter(dest));
}

size_t CBitmap::bitmap_encode_rgb(const uint8_t* rgb, int width, int height, uint8_t*& output)
{
    std::vector<uint8_t> data;
    data.push_back(0x42); //B
    data.push_back(0x4D); //M
    size_t file_size_offset = data.size();
    insert_4_bytes(data, 0xFFFFFFFF); //File Size, fill later
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    size_t pixel_info_offset_offset = data.size();
    insert_4_bytes(data, 0); //pixel info offset, fill later
    insert_4_bytes(data, 40); //Size of BITMAPINFOHEADER
    insert_4_bytes(data, width);
    insert_4_bytes(data, height);
    insert_2_bytes(data, 1); //Number of color planes
    insert_2_bytes(data, 24); //Bits per pixel
    insert_4_bytes(data, 0); //No compression
    size_t raw_pixel_array_size_offset = data.size();
    insert_4_bytes(data, 0); //size of raw data in pixel array, fill later
    insert_4_bytes(data, 2835); //Horizontal Resolution
    insert_4_bytes(data, 2835); //Vertical Resolution
    insert_4_bytes(data, 0); //Number of colors
    insert_4_bytes(data, 0); //Important colors
    {
        uint32_t data_size = data.size();
        memcpy(&data[pixel_info_offset_offset], &data_size, 4);
    }
    uint32_t size_of_header = data.size();
    for (uint_fast32_t y = 0; y < height; ++y)
    {
        for (uint_fast32_t x = 0; x < width; ++x)
        {
            //Write bottom pixels first since image is flipped
            //Also write pixels in BGR
            data.push_back(rgb[(height-1-y)*(width*3) + x*3 + 2]);
            data.push_back(rgb[(height-1-y)*(width*3) + x*3 + 1]);
            data.push_back(rgb[(height-1-y)*(width*3) + x*3 + 0]);
        }
        while ((data.size() - size_of_header) % 4 != 0) data.push_back(0);
    }
    {
        uint32_t file_size = data.size();
        memcpy(&data[file_size_offset], &file_size, 4);
    }
    {
        uint32_t pixel_data_size = data.size() - size_of_header;
        memcpy(&data[raw_pixel_array_size_offset], &pixel_data_size, 4);
    }
    output = new uint8_t[data.size()];
    memcpy(output, &data[0], data.size());
    return data.size();
}

template <class T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	std::vector<T> c(a.size());
	for (int i = 0; i < a.size(); ++i)
		c[i] = a[i] + b[i];
	return c;
}

template <class T>
std::vector<T> operator-(const std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	std::vector<T> c(a.size());
	for (int i = 0; i < a.size(); ++i)
		c[i] = a[i] - b[i];
	return c;
}

template <class T>
std::vector<T> operator*(const T& a, const std::vector<T>& b)
{
	std::vector<T> c(b.size());
	for (int i = 0; i < b.size(); ++i)
		c[i] = a * b[i];
	return c;
}

template <class T>
long double dot(const std::vector<T>& a, const std::vector<T>& b)
{
	assert(a.size() == b.size());
	long double sum = 0.0;
	const std::size_t size = a.size();
	for (int i = 0; i < size; ++i)
		sum += a[i] * b[i];
	return sum;
}

template <class T>
inline long double norm(const std::vector<T>& a) { return std::sqrtl(dot(a, a)); }

struct Three { int d, D, E; };
class CMatrix
{
	double* elem;
	std::size_t n;
public:
	CMatrix(std::size_t n) : n(n) { elem = new double[n * 5]; }
	CMatrix(const CMatrix& o) { n = o.n; elem = new double[n * 5]; memcpy(elem, o.elem, n * 5 * sizeof(double)); }
	~CMatrix() { delete[] elem; n = 0; }
	CMatrix& operator=(const CMatrix& o)
	{
		if (this != &o) {
			if (n != o.n) {
				n = o.n;
				delete[] elem;
				elem = new double[n * 5];
			}
			memcpy(elem, o.elem, n * 5 * sizeof(double));
		}
		return *this;
	}
	inline double& operator()(int i, int j) const { return elem[i * 5 + j]; }
};

CMatrix Jacobian(const std::vector<Three>& x, const std::vector<double>& param)
{
	const double alpha = param[0];
	const double beta = param[1];
	const double gamma = param[2];
	const double delta = param[3];
	const double epsilon = param[4];
	CMatrix J(x.size());
	for (unsigned int i = 0; i < x.size(); i++)
	{
		J(i, 0) = x[i].d * exp(alpha * x[i].d) * pow(x[i].D - x[i].d, gamma) * (delta * x[i].E + epsilon);
		J(i, 1) = pow(x[i].D - x[i].d, gamma) * (delta * x[i].E + epsilon);
		J(i, 2) = (exp(alpha * x[i].d) + beta) * log(x[i].D - x[i].d) * pow(x[i].D - x[i].d, gamma) * (delta * x[i].E + epsilon);
		J(i, 3) = (exp(alpha * x[i].d) + beta) * pow(x[i].D - x[i].d, gamma) * x[i].E;
		J(i, 4) = (exp(alpha * x[i].d) + beta) * pow(x[i].D - x[i].d, gamma);
	}
	return J;
}

CMatrix Cholesky(CMatrix A)
{
	for (int i = 0; i < 5; i++)
		for (int j = 0; j <= i; j++)
		{
			if (i == j)
			{
				for (int k = 0; k < j; k++)
					A(j, j) -= A(j, k) * A(j, k);
				A(j, j) = std::sqrt(A(j, j));
			}
			else
			{
				for (int k = 0; k < j; k++)
					A(i, j) -= A(i, k) * A(j, k);
				A(i, j) /= A(j, j);
				A(j, i) = A(i, j);
			}
		}
	return A;
}

std::vector<double> forward_substitution(const CMatrix& L, std::vector<double> b)
{
	for (int i = 0; i < b.size(); i++)
	{
		for (int j = 0; j < i; j++)
			b[i] -= L(i, j) * b[j];
		b[i] /= L(i, i);
	}
	return b;
}

std::vector<double> backward_substitution(const CMatrix& U, std::vector<double> b)
{
	for (int i = b.size() - 1; i >= 0; i--)
	{
		for (int j = i + 1; j < b.size(); j++)
			b[i] -= U(i, j) * b[j];
		b[i] /= U(i, i);
	}
	return b;
}

std::vector<double> NonLinearLeastSquaresFitter(const std::vector<Three>& x, const std::vector<double>& y, const std::vector<double>& var_y)
{
	assert(x.size() == y.size());
	assert(y.size() == var_y.size());
	const unsigned int size = x.size();
	std::vector<double> beta = { -0.3, 1, 0.3, 0, 2 }; // Starting values
	std::vector<double> delta_beta = beta;
	std::vector<double> delta_y = y;
	std::vector<double> JTWdelty_y = beta;
	CMatrix JTWJ(5);
	double S_sq_new;
	double S_sq_old = 1e300;
	double S_tot = 0;
	double avg = 0;
	for (unsigned int i = 0; i < size; i++) avg += y[i];
	avg /= size;
	for (unsigned int i = 0; i < size; i++) S_tot += (y[i] - avg) * (y[i] - avg);
	
	for (int iter = 0; iter < 1000000; iter++)
	{
		CMatrix J = Jacobian(x, beta);
		for (unsigned int i = 0; i < 5; i++)
			for (unsigned int j = 0; j < 5; j++)
			{
				JTWJ(i, j) = 0;
				for (unsigned int k = 0; k < size; k++)
					JTWJ(i, j) += J(k, i) * J(k, j) / var_y[k];
			}
		for (unsigned int i = 0; i < size; i++)
			delta_y[i] = y[i] - (exp(beta[0] * x[i].d) + beta[1]) * pow(x[i].D - x[i].d, beta[2]) * (beta[3] * x[i].E + beta[4]);
		S_sq_new = dot(delta_y, delta_y);
		if ((S_sq_old - S_sq_new) / S_sq_old < 0.0001) break;
		S_sq_old = S_sq_new;
		for (unsigned int i = 0; i < 5; i++)
		{
			JTWdelty_y[i] = 0;
			for (unsigned int k = 0; k < size; k++)
				JTWdelty_y[i] += J(k, i) / var_y[k] * delta_y[k];
		}
		CMatrix C = Cholesky(JTWJ);
		std::vector<double> yy = forward_substitution(C, JTWdelty_y);
		delta_beta = backward_substitution(C, yy);


		//for (int i = 0; i < 5; i++)
		//{
		//	for (int j = 0; j < 5; j++)
		//		std::cout << JTWJ(i, j) << "   ";
		//	std::cout << std::endl;
		//}
		//std::cout << std::endl;
		//for (int i = 0; i < 5; i++)
		//{
		//	for (int j = 0; j < 5; j++)
		//		std::cout << C(i, j) << "   ";
		//	std::cout << std::endl;
		//}
		//std::cout << std::endl;

		//for (int i = 0; i < 5; i++) std::cout << JTWdelty_y[i] << std::endl;
		//std::cout << std::endl;

		//for (int i = 0; i < 5; i++) std::cout << yy[i] << std::endl;
		//std::cout << std::endl;

		//for (int i = 0; i < 5; i++) std::cout << delta_beta[i] << std::endl;
		//std::cout << std::endl;


		beta = beta + delta_beta;

		printf("%4.4f     %4.4f     %4.4f     %4.4f     %4.4f     %4.4f     %4.4f\n", S_sq_new, 1-S_sq_new/S_tot, beta[0], beta[1], beta[2], beta[3], beta[4]);
	}
	return beta;
}

bool ParseFile(const std::string& filename, std::vector<Three>& x, std::vector<double>& y, std::vector<double>& var_y)
{
	FILE* file = fopen(filename.c_str(), "r");
	if (!file){
		std::cerr << "ERROR: File '" << filename << "' could not be opened!" << std::endl;
		throw "File could not be opened.";
		return false;
	}

	x.clear();
	y.clear();
	var_y.clear();
	const std::size_t N = 4 * 1024;
	std::vector<CDataset_Position_Score_PV> vector;
	char DataArray[N];
	while (fgets(DataArray, N, file))
	{
		std::string s(DataArray);
		Three three;
		three.E = std::stoi(s.substr( 1, 2));
		three.D = std::stoi(s.substr( 6, 2));
		three.d = std::stoi(s.substr(11, 2));
		x.push_back(three);
		y.push_back(std::stod(s.substr(16, 5)));
		var_y.push_back(std::stod(s.substr(24, 8)));
	}

	fclose(file);
	return true;
}

void print_help()
{
	std::cout << "Calculates Prob Cut Statistics.\n" <<
		"Arguments:\n" <<
		"-i abc.xyz      Input filename.\n" <<
		"-o abc.xyz      Output filename.\n" <<
		"-d 123          Depth small.\n" <<
		"-D 123          Depth big.\n" <<
		"-q              quiet.\n" <<
		"-h              Display help." << std::endl;
}

int main(int argc, char* argv[])
{
	std::string in, out;
	int m_d = -1;
	int m_D = -1;
	bool verbose = 1;
	bool b_fit = false;

	for (int i = 0; i < argc; ++i)
	{
		     if (std::string(argv[i]) == "-i") in = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-o") out = std::string(argv[++i]);
		else if (std::string(argv[i]) == "-d") m_d = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-D") m_D = atoi(argv[++i]);
		else if (std::string(argv[i]) == "-q") verbose = 0;
		else if (std::string(argv[i]) == "-fit") b_fit = true;
		else if (std::string(argv[i]) == "-h") { print_help(); return 0; }
	}

	//b_fit = true;
	//in = "C:\\Cassandra\\probcutstats.log";

	// Fit the magic formula
	if (b_fit)
	{
		std::vector<Three> x;
		std::vector<double> y, var_y;
		ParseFile(in, x, y, var_y);
		std::vector<double> beta = NonLinearLeastSquaresFitter(x, y, var_y);
		return 0;
	}
	
	uint64_t HeatMap[129*129];
	uint64_t Histogram[129];
	std::vector<CDataset_Position_FullScore> vec = read_vector<CDataset_Position_FullScore>(in);
	int e = Empties(vec[0].P, vec[0].O);
	int size = e - 5;

	if (verbose)
	{
		printf("  e |  D |  d | sigma |  var  |  R_sq \n");
		printf("----+----+----+-------+-------+-------\n");
	}

	if ((m_d != -1) && (m_D != -1)) // Only print statistics in text
	{
		CSimpleLinearRegression<int> LinReg;
		CRunningStatistic<int> Stat;
		for (const auto& it : vec)
		{
			if ((it.score[m_d] != DATASET_DEFAULT_score) && (it.score[m_D] != DATASET_DEFAULT_score))
			{
				LinReg.Add(it.score[m_d], it.score[m_D]);
				Stat.Add(it.score[m_d] - it.score[m_D]);
			}
		}

		float R = LinReg.R_sq();
		float sigma = Stat.StandardDeviation();
		float tmp = 0.18393972058572116079776188508073 * (Stat.size() - 3) * (Stat.size() - 3) / (Stat.size() - 2) * powf((Stat.size() - 2)/(Stat.size() - 3), Stat.size());
		float var = sigma * sigma / vec.size() * (vec.size() - 1 - 2 * tmp);
		printf(" %2u | %2u | %2u | %5.3f | %7.3f | %5.3f \n", e, m_D, m_d, sigma, var, R);

		return 0;
	}

	const int x_off = 10;
	const int y_off = 20;
	CBitmap bmp(133 * size + x_off, 133 * size + y_off);
	bmp.Clear(255, 255, 255);
	for (int i = 0; i < size; i++)
	{
		int d = (i == size - 1 ? e : i);
		bmp.DrawNumber("%d", d, 3, 78 + 133 * i, 0, 0, 0);
		bmp.DrawNumber("%d", d, 78 + 133 * i, 3, 0, 0, 0);
	}

	for (int i = 0; i < size; i++)
		for (int j = 0; j < size; j++)
		{
			int d = (i == size-1 ? e : i);
			int D = (j == size-1 ? e : j);
			CSimpleLinearRegression<int> LinReg;
			CRunningStatistic<int> Stat;
			memset(HeatMap, 0, 129 * 129 * sizeof(uint64_t));
			memset(Histogram, 0, 129 * sizeof(uint64_t));
			for (const auto& it : vec)
			{
				if ((it.score[d] != DATASET_DEFAULT_score) && (it.score[D] != DATASET_DEFAULT_score))
				{
					HeatMap[(it.score[d] + 64) * 129 + it.score[D] + 64]++;
					Histogram[it.score[d] + 64]++;
					LinReg.Add(it.score[d], it.score[D]);
					Stat.Add(it.score[d] - it.score[D]);
				}
			}
			
			float R = LinReg.R_sq();
			float sigma = Stat.StandardDeviation();

			if ((j > i) && Stat.size())
			{
				float tmp = 0.18393972058572116079776188508073 * (Stat.size() - 3) * (Stat.size() - 3) / (Stat.size() - 2) * powf((Stat.size() - 2)/(Stat.size() - 3), Stat.size());
				float var = sigma * sigma / Stat.size() * (Stat.size() - 1 - 2 * tmp);
				printf(" %2u | %2u | %2u | %5.3f | %7.3f | %5.3f \n", e, D, d, sigma, var, R);
			}

			if (d == D)
				bmp.DrawHistogram(i*133+x_off, j*133+y_off, Histogram);
			else if (Stat.size())
			{
				bmp.DrawHeatmap(i*133+x_off, j*133+y_off, HeatMap);
				if (!isnan(R)) bmp.DrawNumber("%3.3f", R, i*133+x_off+10, j*133+y_off+100, 255, 255, 255);
				bmp.DrawNumber("%3.2f", sigma, i*133+x_off+17, j*133+y_off+100, 255, 255, 255);
			}
		}

	if (!out.empty()) bmp.Save(out);
    return 0;
}
