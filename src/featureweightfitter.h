#pragma once
#include <atomic>
#include <algorithm>
#include <iostream>
#include <map>
#include <omp.h>
#include <vector>
#include "datamanipulation.h"
#include "features.h"
#include "position.h"
#include "statistics.h"

#ifdef DEBUG
	#define ASSERT(X) if (!(X)) std::cerr << #X << " in " << __FILE__ << " on line " << __LINE__ << " failed!" << std::endl
#else
	#define ASSERT(X) ((void)0)
#endif

template <class ValueType, class SizeType=std::size_t>
class CMatrix_CSR
{
private:
	std::size_t _n, _m;
public:
	typedef ValueType value_type;
	typedef SizeType size_type;

	std::vector<ValueType> data;
	std::vector<SizeType> col_indices;
	std::vector<SizeType> row_starts;
	
	CMatrix_CSR(const std::size_t m) : _n(0), _m(m), row_starts(1) {}

	std::size_t n() const { return _n; }
	std::size_t m() const { return _m; }

	//Numer of nonzero matrix elements
	std::size_t nnz() const
	{
		std::size_t counter = 0;
		for (const auto& it : data)
			if (it != 0)
				counter++;
		return counter;
	}

	void insert(const SizeType& i, const SizeType& j, const ValueType& entry);
	void push_back(std::vector<SizeType> Col_Indices, std::vector<ValueType> Data)
	{
		for (auto col_index : Col_Indices) ASSERT(col_index <= _m);
		data.insert(data.end(), Data.begin(), Data.end());
		col_indices.insert(col_indices.end(), Col_Indices.begin(), Col_Indices.end());
		row_starts.push_back(col_indices.size());
		++_n;
	}
	void push_back(const SizeType& col, const ValueType& Data)
	{
		ASSERT(col <= _m);
		if (col > _m) std::cerr << "Col: " << col << ", _m" << _m << std::endl;
		data.push_back(Data);
		col_indices.push_back(col);
	}
	void endRow()
	{
		row_starts.push_back(data.size()); 
		++_n; 
	}
	void remove_zeros()
	{
		int counter = 0;
		for (std::size_t i = 0; i < n; ++i)
		{
			SizeType j = row_starts[i];
			row_starts[i] -= counter;
			for (; j < row_starts[i + 1]; ++j)
			{
				if (data[j] == 0){
					counter++;
					continue;
				}
				data[j - counter] = data[j];
				col_indices[j - counter] = col_indices[j];
			}
		}
		row_starts[n] -= counter;
		data.erase(data.begin() + row_starts[n], data.end());
		col_indices.erase(col_indices.begin() + row_starts[n], col_indices.end());
	}

	template <class T>
	std::vector<T> operator*(const std::vector<T>& x) const
	{
		ASSERT(_m == x.size());
		std::vector<T> result(_n);
		#pragma omp parallel for schedule(static, 1024)
		for (long long i = 0; i < _n; ++i)
		{
			result[i] = 0;
			for (SizeType j = row_starts[i]; j < row_starts[i + 1]; ++j)
				result[i] += static_cast<T>(data[j]) * x[col_indices[j]];
		}
		return result;
	}

	template <class T>
	std::vector<T> ATx(const std::vector<T>& x) const
	{
		ASSERT(_n == x.size());
		std::vector<T> result(_m);
		#pragma omp parallel
		{
			std::vector<T> local_result(_m);

			#pragma omp for nowait schedule(static, 1024)
			for (long long i = 0; i < _n; ++i)
			{
				for (SizeType j = row_starts[i]; j < row_starts[i + 1]; ++j) // This equals A'*x
					local_result[col_indices[j]] += static_cast<T>(data[j]) * x[i];
			}

			#pragma omp critical
			{
				for (std::size_t i = 0; i < _m; ++i)
					result[i] += local_result[i];
			}
		}
		return result;
	}

	template <class T>
	std::vector<T> ATAx(const std::vector<T>& x) const
	{
		ASSERT(_m == x.size());
		ASSERT(_n == x.size());
		std::vector<T> result(_m);
		#pragma omp parallel
		{
			T tmp;
			std::vector<T> local_result(_m);

			#pragma omp for nowait schedule(static, 1024)
			for (long long i = 0; i < _n; ++i)
			{
				tmp = 0;
				for (SizeType j = row_starts[i]; j < row_starts[i + 1]; ++j)
					tmp += static_cast<T>(data[j]) * x[col_indices[j]];
				for (SizeType j = row_starts[i]; j < row_starts[i + 1]; ++j) // This equals A'*x
					local_result[col_indices[j]] += static_cast<T>(data[j]) * tmp;
			}

			#pragma omp critical
			{
				for (std::size_t i = 0; i < _m; ++i)
					result[i] += local_result[i];
			}
		}
		return result;
		//return ATx(this->operator*(x));
	}
};

template <class T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b)
{
	ASSERT(a.size() == b.size());
	std::vector<T> c(a.size());
	for (int i = 0; i < a.size(); ++i)
		c[i] = a[i] + b[i];
	return c;
}

template <class T>
std::vector<T> operator-(const std::vector<T>& a, const std::vector<T>& b)
{
	ASSERT(a.size() == b.size());
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
	ASSERT(a.size() == b.size());
	long double sum = 0.0;
	const std::size_t size = a.size();
	for (int i = 0; i < size; ++i)
		sum += a[i] * b[i];
	return sum;
}

template <class T>
inline long double norm(const std::vector<T>& a) { return std::sqrtl(dot(a, a)); }