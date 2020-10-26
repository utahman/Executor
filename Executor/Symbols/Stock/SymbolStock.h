// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"

class SymbolStock : public ISymbol
{

public:
	SymbolStock(char* name, int exchange)
	{
		set_exchange(exchange);
		set_security_type(IDataProvider::TYPE_STOCK);
		set_name(name);
	}
	~SymbolStock()
	{
	}
	void Show()
	{
	}
	void Stop()
	{
	}

protected:
	virtual void Data_Process(FastData* /*pdata*/)	// From IDataProcessor
	{
	}
};
