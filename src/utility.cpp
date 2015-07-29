#include "utility.h"

//ddd:hh:mm:ss.ccc
std::string time_format(const std::chrono::milliseconds duration)
{
	char time[17] = "            .   "; //ddd:hh:mm:ss.ccc
	int char_0 = static_cast<int>('0');

	typedef std::chrono::duration<int, std::ratio<24*3600> > days;
	long long c = duration.count() % 1000;
	long long s = std::chrono::duration_cast<std::chrono::seconds>(duration).count() % 60;
	int m = std::chrono::duration_cast<std::chrono::minutes>(duration).count() % 60;
	int h = std::chrono::duration_cast<std::chrono::hours>(duration).count() % 24;
	int d = std::chrono::duration_cast<days>(duration).count();

	if (d/100)
		time[0] = char_0 + d/100;
	if (d/10)
		time[1] = char_0 + (d/10)%10;
	if (d)
	{
		time[2] = char_0 + d%10;
		time[3] = ':';
	}
	if (d || h/10)
		time[4] = char_0 + h/10;
	if (d || h)
	{
		time[5] = char_0 + h%10;
		time[6] = ':';
	}
	if (d || h || m/10)
		time[7] = char_0 + m/10;
	if (d || h || m)
	{
		time[8] = char_0 + m%10;
		time[9] = ':';
	}
	if (d || h || m || s/10)
		time[10] = char_0 + s/10;

	time[11] = char_0 + s%10;
	time[13] = char_0 + c/100;
	time[14] = char_0 + (c/10)%10;
	time[15] = char_0 + c%10;

	return std::string(time);
}

std::string short_time_format(std::chrono::duration<long long, std::pico> duration)
{
	char buff[16];

	unsigned long long millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	unsigned long long micros = std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
	unsigned long long nanoss = std::chrono::duration_cast<std::chrono::nanoseconds >(duration).count();
	unsigned long long picos = duration.count();

	     if (millis >= 100) sprintf(buff, "%4lldms", millis);
	else if (millis >=  10) sprintf(buff, "%2.1fms", static_cast<double>(micros) / 1000.0);
	else if (millis >=   1) sprintf(buff, "%1.2fms", static_cast<double>(micros) / 1000.0);
	else if (micros >= 100) sprintf(buff, "%4lldus", micros);
	else if (micros >=  10) sprintf(buff, "%2.1fus", static_cast<double>(nanoss) / 1000.0);
	else if (micros >=   1) sprintf(buff, "%1.2fus", static_cast<double>(nanoss) / 1000.0);
	else if (nanoss >= 100) sprintf(buff, "%4lldns", nanoss);
	else if (nanoss >=  10) sprintf(buff, "%2.1fns", static_cast<double>(picos) / 1000.0);
	else if (nanoss >=   1) sprintf(buff, "%1.2fns", static_cast<double>(picos) / 1000.0);
	else if (picos        ) sprintf(buff, "%4lldps", picos);
	else
		sprintf(buff, "Error!");

	return std::string(buff);
}

std::string ThousandsSeparator(uint64_t n)
{
	if (n < 1000)
		return std::to_string(n);
	else
		return ThousandsSeparator(n / 1000ULL).append("'").append(std::string(3 - std::to_string(n % 1000ULL).length(), '0')).append(std::to_string(n % 1000ULL));
}

std::string DateTimeNow()
{
	std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
	std::time_t t = std::chrono::system_clock::to_time_t(p);
	return std::string(std::ctime(&t));
}
