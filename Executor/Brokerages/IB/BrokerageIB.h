// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"

class BrokerageIB : public IBrokerage
{
public:
	BrokerageIB()
	{

	}
	void Stop()
	{
	}

protected:
	virtual void Data_Process(FastData* /*pdata*/)	// From DataProcessor
	{

	}
};
