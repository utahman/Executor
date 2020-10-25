// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"

#define STOCK_NAME_MAX_SIZE		12


// Common interface for all symbols classes
//
class ISymbol : public DataProcessor
{
	int	_index = 0;
	int _exchange = 0;
	int _security_type = IDataProvider::TYPE_UNK;
	char _name[STOCK_NAME_MAX_SIZE] = { 0 };

	const int POST_LAST = 0;
	const int POST_BID_ASK = 1;
	const int POST_ORDER_STATUS = 2;
	const int POST_PREMARKET_LAST = 3;

	struct _POST_LAST : FastData
	{
		float last;
		float volume;
		int size;
		int msec;
	};

	struct _POST_BID_ASK : FastData
	{
		float bid;
		float ask;
		int bid_size;
		int ask_size;
		int msec;
	};

	struct _POST_PREMARKET_LAST : FastData
	{
		float price;
		float volume;
		int size;
		int msec;
	};

public:
	ISymbol()
	{
	}
	void set_security_type(int stype) { _security_type = stype; }
	int get_security_type() { return _security_type;	}
	void set_name(char* name) { strncpy(_name, name, STOCK_NAME_MAX_SIZE); }
	char* get_name() { return _name; }
	void set_index(int index) { _index = index; }
	int get_index() { return _index; }
	void set_exchange(int e) { _exchange = e; }
	int get_exchange() { return _exchange; }

	inline void set_last(float last, float volume, int size, int msec)
	{
		_POST_LAST* post = new _POST_LAST;
		post->_type = POST_LAST;
		post->last = last;
		post->volume = volume;
		post->size = size;
		post->msec = msec;
		Data_Post(post);
	}
	inline void set_price_premarket(float price, float volume, int size, int msec)
	{
		_POST_PREMARKET_LAST* post = new _POST_PREMARKET_LAST;
		post->_type = POST_PREMARKET_LAST;
		post->price = price;
		post->volume = volume;
		post->size = size;
		post->msec = msec;
		Data_Post(post);
	}
	inline void set_bid_ask(float bid, float ask, int bid_size, int ask_size, int time_msec)
	{
		_POST_BID_ASK* post = new _POST_BID_ASK;
		post->_type = POST_BID_ASK;
		post->bid = bid;
		post->ask = ask;
		post->bid_size = bid_size;
		post->ask_size = ask_size;
		post->msec = time_msec;
		Data_Post(post);
	}

	float _volatility = 0;
	float _average_volume = 0; // Historical average volume
	float _yield = 0;
	float _pe = 0;
	float _beta = 0;
	float _52WeekHigh = 0;
	float _52WeekLow = 0;
	float _prev_day_open = 0;
	float _prev_day_high = 0;
	float _prev_day_low = 0;
	float _prev_day_close = 0;

	void set_volatility(float v) { _volatility = v; }
	float get_volatility() { return _volatility; }
	void set_average_volume(float v) { _average_volume = v; }
	float get_average_volume() { return _average_volume; }
	void set_pe(float v) { _pe = v; }
	float get_pe() { return _pe; }
	void set_yield(float v) { _yield = v; }
	float get_yield() { return _yield; }
	void set_beta(float v) { _beta = v; }
	float get_beta() { return _beta; }
	void set_52WeekHigh(float v) { _52WeekHigh = v; }
	float get_52WeekHigh() { return _52WeekHigh; }
	void set_52WeekLow(float v) { _52WeekLow = v; }
	float get_52WeekLow() { return _52WeekLow; }

	void set_prev_day_OHLC(float o, float h, float l, float c)
	{
		_prev_day_open = o;
		_prev_day_high = h;
		_prev_day_low = l;
		_prev_day_close = c;
	}
	float get_prev_day_Open() { return _prev_day_open; }
	float get_prev_day_High() { return _prev_day_high; }
	float get_prev_day_Low() { return _prev_day_low; }
	float get_prev_day_Close() { return _prev_day_close; }
};
