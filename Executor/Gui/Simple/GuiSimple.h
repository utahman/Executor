// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"

class GuiSimple : public IGui
{
public:
	GuiSimple()
	{
	}
	~GuiSimple()
	{
	}

	void Show();
	void Stop()
	{
	}

private:	// Internal methods

	virtual void Data_Process(FastData* /*pdata*/)	// From DataProcessor
	{
	}
};
