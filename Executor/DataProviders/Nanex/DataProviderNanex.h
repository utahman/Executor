// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"

class DataProviderNanex : public IDataProvider
{
public:
	DataProviderNanex()
	{
	}
	void Stop()
	{
		//g_nc.MarketDataStop();
	}

protected:
	virtual void Data_Process(FastData* /*pdata*/)	// From DataProcessor
	{
	}
};
