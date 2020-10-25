// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once

#include "Config.h"
#include "Utils.h"
#include "DataProcessor.h"
#include "IBrokerage.h"
#include "IDataProvider.h"
#include "ISymbol.h"
#include "SymbolsArray.h"
#include "IAlgo.h"
#include "IGui.h"
#include "ILogs.h"


class Config
{
	JsonObject _obj;
	std::string _logs_obj;
	std::string _logs_path;
	std::string _logs_file_name;
	std::string _data_provider;
	std::string _brokerage;

public:
	bool volatile _run = true;				// App will run until this is true
	bool volatile _gui_shown = false;		// If true then send messages to gui
	SYSTEMTIME _system_tm = { 0 };			// Current system time
	int _system_sec = 0;					// Seconds from midnight
	int _system_msec = 0;					// Milliseconds from midnight
	IDataProvider* _data = NULL;			// Data privider
	IBrokerage* _broker = NULL;				// Brokerage
	ILogs* _logs = NULL;					// Global app logging
	IGui* _gui = NULL;						// GUI
	IAlgo* _algo = NULL;					// Algorithm that does trading
	SymbolsArray _symbols2;					// Symbols that receive updates

public:
	void Load();
	void updateSystemTime();
	std::string get(const wchar_t* param);
	std::string get(const wchar_t* section, const wchar_t* param);
	int get(const wchar_t* section, const wchar_t* param, int def_value);
	std::string& GetDataProvider() { return _data_provider; }
	std::string& GetBrokerage() { return _brokerage; }
};

extern Config g;
