// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"

class AlgoMovingAverage : public IAlgo
{
public:
	AlgoMovingAverage()
	{

	}
	virtual bool CheckToAddSymbol(int symbol_type, int exchange, char* symbol_name)
	{
		// Here we must decide if we are going to use this symbol
		//
		symbol_type;
		exchange;
		symbol_name;

		return true;
	}

	void Stop()
	{
	}
};
