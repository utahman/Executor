// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"

// Common interface for all symbols classes
//
class ISymbol : public DataProcessor
{
public:
	ISymbol()
	{

	}
};


// Storage for ISymbol type
//
class Symbols
{
public:
	Symbols()
	{

	}
	~Symbols()
	{

	}
	ISymbol* FindFast()
	{
		return 0;
	}
};
