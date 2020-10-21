// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"

class IBrokerage : public DataProcessor
{
public:
	IBrokerage()
	{

	}
	~IBrokerage()
	{

	}

	virtual void Stop() = 0;
};
