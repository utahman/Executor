// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include <winnt.h>
#include <minwinbase.h>
#include <synchapi.h>
#include <process.h>

struct FastData
{
	int			_type;
	FastData*	_next;
};


class DataProcessor
{
	HANDLE		_event = NULL;
	FastData*	_begin1 = NULL;
	FastData*	_end = NULL;
	bool		_exiting = false;
	bool		_exited = true;
	CRITICAL_SECTION _cs;

public:
	DataProcessor()
	{
		InitializeCriticalSection(&_cs);
		_event = ::CreateEvent(0, FALSE, FALSE, 0);
		_beginthreadex(0, 0, &DataProcessor::data_Thread, this, 0, 0);
	}

	~DataProcessor()
	{
		// Important to call StopDataProcessor() in above class!
		//
		StopDataProcessor();
		CloseHandle(_event);
		DeleteCriticalSection(&_cs);
	}

	void StartExiting()
	{
		_exiting = true;
	}

	void StopDataProcessor()
	{
		_exiting = true;
		while (!_exited)
		{
			SetEvent(_event);
			Sleep(1);
		}
	}

	void Data_Post(FastData* pdata = NULL)
	{
		if (!_exiting) // Do not accept new data if exiting
		{
			// Insert data as fast as possible
			//
			EnterCriticalSection(&_cs);
			if (!_end) // If no data
				_begin1 = pdata;
			else
				_end->_next = pdata;
			_end = pdata;
			pdata->_next = NULL;
			LeaveCriticalSection(&_cs);

			// Trigger event to let Thread know to receive data
			// or do any other current processing
			//
			SetEvent(_event);
		}
	}

	bool IsDataExiting()
	{
		return _exiting;
	}

private:
	virtual void Data_Process(FastData* pdata) = 0; // To overwrite

	static unsigned int __stdcall data_Thread(void* p_this)
	{
		DataProcessor* obj = static_cast<DataProcessor*>(p_this);
		obj->_exited = false;
		FastData* pdata = NULL;

		while (1)
		{
			WaitForSingleObject(obj->_event, 1000); // No more than 1 sec: important
			if (!obj->_exiting)
			{

				if (obj->_begin1)
				{
					// Retrieve data as quick as possible
					//
					EnterCriticalSection(&obj->_cs);
					pdata = obj->_begin1;
					obj->_begin1 = obj->_end = NULL;
					LeaveCriticalSection(&obj->_cs);
				}

				if (pdata)
				{
					do
					{
						obj->Data_Process(pdata);
						FastData* todel = pdata;
						pdata = pdata->_next;
						delete todel;
					} while (pdata);
				}
				else
					obj->Data_Process(NULL); // Must call anyway once a sec
			}
			else
				break;
		}

		// We do not care about freeing memory since app is closing

		obj->_exited = true;
		return 0;
	}
};
