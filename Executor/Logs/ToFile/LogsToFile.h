// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"


class LogsToFile : public ILogs
{
	struct FastData_Logs : FastData
	{
		int _msec = 0;
		std::string* _str = NULL;
		FastData_Logs(int msec, std::string* str)
		{
			_msec = msec;
			_type = 1;
			_str = str;
		}
	};
	std::string _path;
	std::string _file_name;
	std::string _full_name;
	FILE* _fh = NULL;

public:
	LogsToFile(std::string path, std::string file_name)
	{
		_path = path;
		_file_name = file_name;

		SYSTEMTIME tm;
		GetLocalTime(&tm);

		// Find a name for a new (non-existing) file
		std::filesystem::path fn(_path);
		std::string fn_add = sformat("%02d-%02d-%02d  %02d-%02d-%02d %s",
			tm.wYear, tm.wMonth, tm.wDay,
			tm.wHour, tm.wMinute, tm.wSecond,
			_file_name.c_str());
		fn.append(fn_add);
		_full_name = fn.string();
		_fh = fopen(_full_name.c_str(), "a");
	}

	~LogsToFile()
	{
		if (_fh)
			fclose(_fh);
	}

	void Add(int msec, std::string* str) // str must be created with 'new'
	{
		this->Data_Post(new FastData_Logs(msec, str));
	}

	void Stop()
	{
	}

protected:
	virtual void DataProcessor::Data_Process(FastData* data) // From DataProcessor
	{
		if (!data || data->_type != 1 || !_fh)
			return;

		// Important to delete FastData_Logs._str which is created with 'new'
		//
		FastData_Logs* pdata = (FastData_Logs*) data;
		if (pdata && pdata->_str)
		{
			int hour, minn, sec, msec;
			MSecToTimeGlobal(pdata->_msec, hour, minn, sec, msec);

			fprintf(_fh, "%02d:%02d:%02d.%003d\t%s\n",
				hour,
				minn,
				sec,
				msec,
				pdata->_str->c_str());
			::fflush(_fh);

			delete pdata->_str;
		}
	}

};

