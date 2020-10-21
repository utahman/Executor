// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#include "pch.h"
#include <string>
#include <fstream>
#include <streambuf>
#include <winrt/Windows.Data.json.h>
#include "Config.h"

// Here is the only place where we include files with real implementation
// of all objects. Everywhere else we must use common interfaces only
//
#include "ToFile/LogsToFile.h"
#include "IB/BrokerageIB.h"
#include "Nanex/DataProviderNanex.h"
#include "Simple/GuiSimple.h"

using namespace winrt;
using namespace Windows::Data::Json;


void Config::updateSystemTime()
{
	GetLocalTime(&_system_tm);
	_system_sec = _system_tm.wHour * 3600 + _system_tm.wMinute * 60;
	_system_msec = _system_sec + _system_tm.wMilliseconds;
}

std::string get(JsonObject& obj, const wchar_t* param)
{
	return winrt::to_string(obj.GetNamedString(param));
}

void Config::Load()
{
	std::ifstream t("_executor_config.txt");
	std::wstring str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());
	JsonObject obj = JsonObject::Parse(hstring(str.c_str()));

	// First of all create logs object
	_logs_obj = get(obj, L"g_logs_obj");
	_logs_path = get(obj, L"g_logs_path");
	_logs_file_name = get(obj, L"g_logs_file_name");
	if (_logs_obj == "ToFile")
		_logs = new LogsToFile(_logs_path, _logs_file_name);
	_logs->Add(_system_msec, format("Starting"));

	_data_provider = get(obj, L"data_provider");
	_brokerage = get(obj, L"brokerage");

	if (g.GetDataProvider() == "Nanex")
		_data = new DataProviderNanex();
	_logs->Add(_system_msec, format("Added data provider: %s", g.GetDataProvider().c_str()));

	if (g.GetBrokerage() == "IB")
		_broker = new BrokerageIB();
	_logs->Add(_system_msec, format("Added brokerage: %s", g.GetBrokerage().c_str()));
}