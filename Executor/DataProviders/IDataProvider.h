// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"

class IDataProvider : public DataProcessor
{
protected:
	int _time_sec = 0;			// (Nanex tape time) Seconds since midnight
	int _time_msec = 0;			// (Nanex tape time) Milliseconds since midnight
	volatile bool _market_open_US_stocks = false;
	const int _sec_market_opens_US_stocks = TimeToSecGlobal(9, 30);
	const int _sec_market_closes_US_stocks = TimeToSecGlobal(16, 0);

public:
	// Keep growing as power of two
	enum TYPE { TYPE_STOCK = 1, TYPE_FUTURE = 2, TYPE_FOREX = 4, TYPE_FUTURE_OPT = 8, TYPE_UNK = 16 };

public:
	IDataProvider()
	{

	}
	~IDataProvider()
	{

	}
	void set_time(int hour, int minute, int second, int millisecond)
	{
		_time_sec = hour * 3600 + minute * 60 + second;
		_time_msec = _time_sec * 1000 + millisecond;

		// Set US stock markets status
		if(_time_sec >= _sec_market_opens_US_stocks &&
			_time_sec < _sec_market_closes_US_stocks)
			_market_open_US_stocks = true;
	}

	virtual void Stop() = 0;
};
