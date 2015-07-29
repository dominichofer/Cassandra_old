#pragma once
#include <chrono>
#include <cmath>
#include <ctime>
#include <cstdint>
#include <ratio>
#include <string>
#include "macros_hell.h"

template <typename T> inline T ABS(const T & a) { return a > 0 ? a : -a; }
template <typename T> inline T MIN(const T & a, const T & b) { return a < b ? a : b; }
template <typename T> inline T MAX(const T & a, const T & b) { return a > b ? a : b; }
template <typename T> inline T BIND(const T & a, const T & min, const T & max) { return a < min ? min : (a > max ? max : a); }

inline int RoundInt(const double d) { return static_cast<int>(std::round(d)); }
inline int RoundInt(const float  f) { return static_cast<int>(std::round(f)); }
//inline int RoundInt(double d) { int i = static_cast<int>(d); return i + (d - i >= .5 ) - (d - i <= -.5 ); }
//inline int RoundInt(float  f) { int i = static_cast<int>(f); return i + (f - i >= .5f) - (f - i <= -.5f); }

std::string time_format(const std::chrono::milliseconds duration);
inline std::string time_format(const std::chrono::duration<std::chrono::system_clock::rep, std::chrono::system_clock::period> duration) { return time_format(std::chrono::duration_cast<std::chrono::milliseconds>(duration)); }
std::string short_time_format(std::chrono::duration<long long, std::pico> duration);
std::string ThousandsSeparator(uint64_t n);
std::string DateTimeNow();
