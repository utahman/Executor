// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"

class IAlgo : public DataProcessor
{
public:
	IAlgo()
	{
	}
	~IAlgo()
	{
	}
	virtual bool CheckToAddSymbol(int symbol_type, int exchange, char* symbol_name) = 0;
	virtual void Stop() = 0;
};
