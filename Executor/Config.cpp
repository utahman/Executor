// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#include "pch.h"
#include <string>
#include <fstream>
#include <streambuf>
#include "Config.h"

// Here is the only place where we include files with real implementation
// of all objects. Everywhere else we must use common interfaces only
//
#include "ToFile/LogsToFile.h"
#include "MovingAverage/AlgoMovingAverage.h"
#include "IB/BrokerageIB.h"
#include "Nanex/DataProviderNanex.h"
#include "Simple/GuiSimple.h"


void Config::updateSystemTime()
{
	GetLocalTime(&_system_tm);
	_system_sec = _system_tm.wHour * 3600 + _system_tm.wMinute * 60;
	_system_msec = _system_sec + _system_tm.wMilliseconds;
}


std::string Config::get(const wchar_t* param)
{
	if (_obj.HasKey(param))
		return winrt::to_string(_obj.GetNamedString(param));
	return std::string();
}


std::string Config::get(const wchar_t* section, const wchar_t* param)
{
	if (_obj.HasKey(section))
	{
		JsonObject st = _obj.GetNamedObject(section);
		if (st.HasKey(param))
			return winrt::to_string(st.GetNamedString(param));
	}
	return std::string();
}


int Config::get(const wchar_t* section, const wchar_t* param, int def_value)
{
	if (_obj.HasKey(section))
	{
		JsonObject st = _obj.GetNamedObject(section);
		if (st.HasKey(param))
			return int(st.GetNamedNumber(param) + 0.5);
	}
	return def_value;
}


void Config::LoadAndCreateObjects()
{
	std::ifstream t("_executor_config.txt");
	std::wstring str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	_obj = JsonObject::Parse(hstring(str.c_str()));

	// First of all create logs object
	_logs_obj = get(L"logs_obj");
	_logs_path = get(L"logs_path");
	_logs_file_name = get(L"logs_file_name");
	if (_logs_obj == "ToFile")
		_logs = new LogsToFile(_logs_path, _logs_file_name);
	_logs->Add(_system_msec, format("Starting"));

	_name_algo = get(L"algo");
	_name_gui = get(L"gui");
	_name_data_provider = get(L"data_provider");
	_name_brokerage = get(L"brokerage");

	if (g.GetAlgoName() == "MovingAverage")
		_algo = new AlgoMovingAverage();
	_logs->Add(_system_msec, format("Using algo: %s", g.GetAlgoName().c_str()));

	if (g.GetDataProviderName() == "Nanex")
		_data = new DataProviderNanex();
	_logs->Add(_system_msec, format("Using data provider: %s", g.GetDataProviderName().c_str()));

	if (g.GetBrokerageName() == "IB")
		_broker = new BrokerageIB();
	_logs->Add(_system_msec, format("Using brokerage: %s", g.GetBrokerageName().c_str()));

	if (g.GetGuiName() == "GuiSimple")
		_gui = new GuiSimple();
	_logs->Add(_system_msec, format("Using GUI: %s", g.GetGuiName().c_str()));
}
