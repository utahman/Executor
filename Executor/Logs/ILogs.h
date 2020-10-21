// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"

class ILogs : public DataProcessor
{
public:
	ILogs()
	{

	}
	~ILogs()
	{

	}
	virtual void Add(int msec, std::string* str) = 0; // To overwrite

	void Show()
	{
	}
	void Stop()
	{
	}
};
