#pragma once
#include <atomic>

template <typename T>
long double StandardDeviation(const std::vector<T>& vec)
{
	long double E_of_X = 0;
	long double E_of_X_sq = 0;
	std::size_t N = 0;

	for (const auto& elem : vec) {
		N++;
		E_of_X += (static_cast<long double>(elem) - E_of_X) / static_cast<long double>(N);
		E_of_X_sq += (static_cast<long double>(elem)*static_cast<long double>(elem) - E_of_X_sq) / static_cast<long double>(N);
	}

	return std::sqrtl(E_of_X_sq - E_of_X * E_of_X);
}

template <typename T>
long double AverageAbs(const std::vector<T>& vec)
{
	long double E_of_X = 0;
	std::size_t N = 0;

	for (const auto& elem : vec)
		E_of_X += (static_cast<long double>(std::abs(elem)) - E_of_X) / static_cast<long double>(++N);

	return E_of_X;
}

template<class T>
class CRunningStatistic
{
public:
	CRunningStatistic() : _N(0), _E_of_X(0), _E_of_X_sq(0), _min(0), _max(0) { spinlock.clear(); }

	void Add(const T& value)
	{
		while (spinlock.test_and_set(std::memory_order_acquire)) ;

		if (_N == 0) {
			_min = value;
			_max = value;
		}
		_N++;
		if (value < _min) _min = value;
		if (_max < value) _max = value;
		_E_of_X += (static_cast<long double>(value) - _E_of_X) / static_cast<long double>(_N);
		_E_of_X_sq += (static_cast<long double>(value)*static_cast<long double>(value) - _E_of_X_sq) / static_cast<long double>(_N);

		spinlock.clear(std::memory_order_release);
	}

	inline std::size_t N() const { return _N; }
	inline std::size_t size() const { return N(); }

	inline long double mu() const { return _E_of_X; }
	inline long double Average() const { return mu(); }

	inline long double Var() const { return _E_of_X_sq - _E_of_X*_E_of_X; }
	inline long double Variance() const { return Var(); }

	inline long double sigma() const { return std::sqrtl(Var()); }
	inline long double StandardDeviation() const { return sigma(); }

	inline long double deltasigma() const { return std::sqrtl(Var()) / std::sqrtl(2.0 * (static_cast<long double>(_N) + 1.0)); }
	inline long double StandardDeviationError() const { return deltasigma(); }

	inline long double RelativeError() const { return sigma() / mu(); }

	inline T Min() const { return _min; }
	inline T Max() const { return _max; }

	CRunningStatistic& operator+=(const CRunningStatistic & rhs)
	{
		uint64_t sum = this->_N + rhs._N;

		this->_E_of_X = this->_E_of_X * (static_cast<long double>(this->_N) / static_cast<long double>(sum)) 
					  + rhs._E_of_X * (static_cast<long double>(rhs._N) / static_cast<long double>(sum));
		this->_E_of_X_sq = this->_E_of_X_sq * (static_cast<long double>(this->_N) / static_cast<long double>(sum))
						 + rhs._E_of_X_sq * (static_cast<long double>(rhs._N) / static_cast<long double>(sum));
		this->_N = sum;
		if (rhs._min < this->_min)
			this->_min = rhs._min;
		if (this->_max < rhs._max) 
			this->_max = rhs._max;

		return *this;
	}
	CRunningStatistic operator+(const CRunningStatistic & rhs) const
	{
		RunningStatistic<T> res = RunningStatistic<T>();
		res._N = this->_N + rhs._N;
		res._E_of_X = this->_E_of_X * (static_cast<long double>(this->_N) / static_cast<long double>(res._N))
					+ rhs._E_of_X * (static_cast<long double>(rhs._N) / static_cast<long double>(res._N));
		res._E_of_X_sq = this->_E_of_X_sq * (static_cast<long double>(this->_N) / static_cast<long double>(res._N)) 
					   + rhs._E_of_X_sq * (static_cast<long double>(rhs._N) / static_cast<long double>(res._N));
		res._min = (this->_min < rhs._min) ? this->_min : rhs._min;
		res._max = (rhs._max < this->_max) ? this->_max : rhs._max;
		return res;
	}

private:
	std::atomic_flag spinlock;
	std::size_t _N;
	long double _E_of_X;
	long double _E_of_X_sq;
	T _min;
	T _max;
};

template<class T>
class CSimpleLinearRegression
{
public:
	CSimpleLinearRegression() : _N(0), _E_of_X(0), _E_of_Y(0), _E_of_XY(0), _E_of_X_sq(0), _E_of_Y_sq(0) { spinlock.clear(); }

	void Add(const T& _x, const T& _y)
	{
		while (spinlock.test_and_set(std::memory_order_acquire)) ;

		_N++;
		const long double x = static_cast<long double>(_x);
		const long double y = static_cast<long double>(_y);
		const long double N = static_cast<long double>(_N);
		_E_of_X += (x - _E_of_X) / N;
		_E_of_Y += (y - _E_of_Y) / N;
		_E_of_XY += (x * y - _E_of_XY) / N;
		_E_of_X_sq += (x * x - _E_of_X_sq) / N;
		_E_of_Y_sq += (y * y - _E_of_Y_sq) / N;

		spinlock.clear(std::memory_order_release);
	}

	inline std::size_t N() const { return _N; }
	inline std::size_t size() const { return N(); }
	
	inline long double b() const { return (_E_of_XY - _E_of_X * _E_of_Y) / (_E_of_X_sq - _E_of_X * _E_of_X); }
	inline long double a() const { return _E_of_Y - b() * _E_of_X; }
	inline long double R_sq() const { return (_E_of_XY - _E_of_X * _E_of_Y) * (_E_of_XY - _E_of_X * _E_of_Y) / (_E_of_X_sq - _E_of_X * _E_of_X) / (_E_of_Y_sq - _E_of_Y * _E_of_Y); }

	CSimpleLinearRegression& operator+=(const CSimpleLinearRegression & rhs)
	{
		uint64_t sum = this->_N + rhs._N;

		this->_E_of_X = this->_E_of_X * (static_cast<long double>(this->_N) / static_cast<long double>(sum)) 
					  +   rhs._E_of_X * (static_cast<long double>(  rhs._N) / static_cast<long double>(sum));
		this->_E_of_Y = this->_E_of_Y * (static_cast<long double>(this->_N) / static_cast<long double>(sum)) 
					  +   rhs._E_of_Y * (static_cast<long double>(  rhs._N) / static_cast<long double>(sum));
		this->_E_of_XY = this->_E_of_XY * (static_cast<long double>(this->_N) / static_cast<long double>(sum)) 
					   +   rhs._E_of_XY * (static_cast<long double>(  rhs._N) / static_cast<long double>(sum));
		this->_E_of_X_sq = this->_E_of_X_sq * (static_cast<long double>(this->_N) / static_cast<long double>(sum))
						 +   rhs._E_of_X_sq * (static_cast<long double>(  rhs._N) / static_cast<long double>(sum));
		this->_E_of_Y_sq = this->_E_of_Y_sq * (static_cast<long double>(this->_N) / static_cast<long double>(sum))
						 +   rhs._E_of_Y_sq * (static_cast<long double>(  rhs._N) / static_cast<long double>(sum));
		this->_N = sum;

		return *this;
	}
	CSimpleLinearRegression operator+(const CSimpleLinearRegression & rhs) const
	{
		RunningStatistic<T> res = RunningStatistic<T>();
		res._N = this->_N + rhs._N;
		res._E_of_X = this->_E_of_X * (static_cast<long double>(this->_N) / static_cast<long double>(res._N))
					+   rhs._E_of_X * (static_cast<long double>(  rhs._N) / static_cast<long double>(res._N));
		res._E_of_Y = this->_E_of_Y * (static_cast<long double>(this->_N) / static_cast<long double>(res._N))
					+   rhs._E_of_Y * (static_cast<long double>(  rhs._N) / static_cast<long double>(res._N));
		res._E_of_XY = this->_E_of_XY * (static_cast<long double>(this->_N) / static_cast<long double>(res._N))
					 +   rhs._E_of_XY * (static_cast<long double>(  rhs._N) / static_cast<long double>(res._N));
		res._E_of_X_sq = this->_E_of_X_sq * (static_cast<long double>(this->_N) / static_cast<long double>(res._N)) 
					   +   rhs._E_of_X_sq * (static_cast<long double>(  rhs._N) / static_cast<long double>(res._N));
		res._E_of_Y_sq = this->_E_of_Y_sq * (static_cast<long double>(this->_N) / static_cast<long double>(res._N)) 
					   +   rhs._E_of_Y_sq * (static_cast<long double>(  rhs._N) / static_cast<long double>(res._N));
		return res;
	}

private:
	std::atomic_flag spinlock;
	std::size_t _N;
	long double _E_of_X;
	long double _E_of_Y;
	long double _E_of_XY;
	long double _E_of_X_sq;
	long double _E_of_Y_sq;
};