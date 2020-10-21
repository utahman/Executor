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

	virtual void Stop() = 0;
};
