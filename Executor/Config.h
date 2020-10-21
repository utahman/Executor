// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "Config.h"
#include "Utils.h"
#include "DataProcessor.h"
#include "IBrokerage.h"
#include "IDataProvider.h"
#include "ISymbol.h"
#include "IAlgo.h"
#include "IGui.h"
#include "ILogs.h"


class Config
{
	std::string _logs_obj;
	std::string _logs_path;
	std::string _logs_file_name;
	std::string _data_provider;
	std::string _brokerage;

public:
	bool volatile _run = true;					// App will run until this is true
	bool volatile _gui_shown = false;			// If true then send messages to gui
	int _system_sec = 0, _system_msec = 0;	// Current system time starting from midnight
	SYSTEMTIME _system_tm = { 0 };				// Current system time
	IDataProvider* _data = NULL;				// Data privider
	IBrokerage* _broker = NULL;				// Brokerage
	ILogs* _logs = NULL;						// Global app logging
	IGui* _gui = NULL;							// GUI

public:
	void Load();
	void updateSystemTime();
	std::string& GetDataProvider() { return _data_provider; }
	std::string& GetBrokerage() { return _brokerage; }
};

extern Config g;
