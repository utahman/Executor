// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#include "pch.h"
#include "Utils.h"

void MSecToTimeGlobal(int glmsec, int& hour, int& min, int& sec, int& msec)
{
	hour = glmsec / 3600000;
	min = glmsec / 60000;
	sec = glmsec / 1000;
	msec = glmsec - 1000 * sec;
}


int TimeToSecGlobal(int hour, int minute, int second)
{
	return hour * 3600 + minute * 60 + second;
}


int TimeToMillisecGlobal(int hour, int minute, int second, int milliseond)
{
	return TimeToSecGlobal(hour, minute, second) * 1000 + milliseond;
}


std::string* format(const char* fmt_str, ...)
{
	int final_n, n = ((int)strlen(fmt_str)) * 2;
	std::unique_ptr<char[]> formatted;
	va_list ap;
	while (1) {
		formatted.reset(new char[n]);
		strncpy_s(&formatted[0], n, fmt_str, n);
		va_start(ap, fmt_str);
		final_n = vsnprintf(&formatted[0], n, fmt_str, ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	return new std::string(formatted.get());
}


std::string sformat(const char* fmt_str, ...)
{
	int final_n, n = ((int)strlen(fmt_str)) * 2;
	std::unique_ptr<char[]> formatted;
	va_list ap;
	while (1) {
		formatted.reset(new char[n]);
		strncpy_s(&formatted[0], n, fmt_str, n);
		va_start(ap, fmt_str);
		final_n = vsnprintf(&formatted[0], n, fmt_str, ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
			n += abs(final_n - n + 1);
		else
			break;
	}
	return std::string(formatted.get());
}
