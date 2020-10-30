// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"

class IGui : public DataProcessor
{
public:
	IGui()
	{
	}
	~IGui()
	{
	}
	virtual void Show() = 0;
	virtual void Stop() = 0;
};
