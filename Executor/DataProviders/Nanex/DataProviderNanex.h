// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"
#include "C:\Program Files (x86)\Nanex\NxCoreAPI\NxCoreAPI_Wrapper_C++.h"

#define CATEGORY_TYPE_EquityFundamental		9
#define CATEGORY_TYPE_DTNEquityFundamental	10
#define CATEGORY_TYPE_52WeekHiLo			13
#define CATEGORY_TYPE_OHLC					16				// OHLC - Open, High, Low, Close
#define CATEGORY_TYPE_RootSymbolInfo		70

#define CATEGORY_STR_Beta					"Beta"			// In EquityFundamental
#define CATEGORY_STR_Volatility				"Volatility"	// In DTNEquityFundamental
#define CATEGORY_STR_AvgVolume				"AvgVolume"		// In DTNEquityFundamental
#define CATEGORY_STR_PE						"PE"			// In DTNEquityFundamental
#define CATEGORY_STR_Yield					"Yield"			// In DTNEquityFundamental
#define CATEGORY_STR_52WeekHigh				"High"			// In 52WeekHiLo
#define CATEGORY_STR_52WeekLow				"Low"			// In 52WeekHiLo
#define CATEGORY_STR_Open					"Open"			// In OHLC
#define CATEGORY_STR_High					"High"			// In OHLC
#define CATEGORY_STR_Low					"Low"			// In OHLC
#define CATEGORY_STR_Close					"Close"			// In OHLC
#define CATEGORY_STR_Volume					"Volume"		// In OHLC
#define CATEGORY_STR_QuotedUnit				"QuotedUnit"	// In RootSymbolInfo

#define NC_EXCHNAGE_NYSE				3	// Stocks
#define NC_EXCHNAGE_AMEX				4
#define NC_EXCHNAGE_PACF				7	// For DIA, SPY and more stocks
#define NC_EXCHNAGE_NASDAQ_NATIONAL		12
#define NC_EXCHNAGE_NASDAQ_SM_CAPS		13
#define NC_EXCHNAGE_CME					20	// Futures
#define NC_EXCHNAGE_CBOT				24	// Futures
#define NC_EXCHNAGE_BARK				45	// Forex quotes from Barklays

// For bitwise operations, so keep growing at power of 2
#define CHANGED_BID				1
#define CHANGED_ASK				2
#define CHANGED_LAST1			4
#define CHANGED_PREV_OPEN		8
#define CHANGED_PREV_HIGH		16
#define CHANGED_PREV_LOW		32
#define CHANGED_PREV_CLOSE		64
#define CHANGED_BETA			128
#define CHANGED_VOLATILITY		256
#define CHANGED_AVER_VOLUME		512
#define CHANGED_PE				1024
#define CHANGED_YIELD			2048
#define CHANGED_52WEEK_HIGH		4096
#define CHANGED_52WEEK_LOW		8192
#define CHANGED_PREMARKET_LAST	16384


class DataProviderNanex : public IDataProvider
{
protected:
	NxCoreClass			_core;
	volatile bool		_thread_running = false;
	volatile bool		_exiting = false;
	volatile int		_tapeStatus = -1;
	char				_tapeName[MAX_PATH] = { 0 };
	bool				_initialized = false;
	int					_recid = 0; // Record id in the nanex tape
	bool				_bEndOfTape = false;
	int					_UseSecurities = 0;
	char*				_UnusedStockName = (char*)"BADBAD";
	ILogs*				_logs = NULL;
	std::string			_dll_path;
	std::vector <std::pair<std::string, std::string>> _renames;
	static DataProviderNanex*	_object;

public:
	DataProviderNanex();
	~DataProviderNanex()
	{
		Stop();
	}
	void Stop()
	{
		_exiting = true;
		while (_thread_running)
			Sleep(10);
		//g_Symbols.DestroyAllSymbols();
	}
	bool EndOfTape() { return _bEndOfTape; }
	bool IsThreadRunning() { return _thread_running; }
	bool MD_Initialize(char* tapeName);
	//virtual Symbol* MD_CreateNewSymbolObj(const char* name, USHORT exchange, char Security) = 0; // here external app creates and returns an object inherited from Symbol
	//virtual void MD_ProcessFilteredUpdate(float last, Symbol* s) = 0;
	//virtual void MD_ProcessUpdate_OtherValues(WORD flags, int sec, Symbol* s) = 0;
	//virtual int  MD_ProcessUpdate_TimeMsec(int msec_nt) = 0;						// To check if real-time data is fresh
	//virtual void MD_Status(const char*);											// To display status

	static char* GetNameFromNxCoreName(char* core_name);
	static int ExceptionFilter(unsigned int code, struct _EXCEPTION_POINTERS* ep);
	void AddForegnSymbol(char* name, USHORT exchange, char SecurityType);
	std::vector < std::pair<std::string, std::string>>& GetRenamesVector() { return _renames; }
	int GetCurrentRID() { return _recid; }

private:	// Internal methods

	virtual void Data_Process(FastData* /*pdata*/)	// From DataProcessor
	{
	}

	int RID() { return _recid++; }
	float FieldToFloat(NxCategoryField& f);
	bool IsGoodNCStockSymbolName(char* name);
	bool IsGoodNCFuturesSymbolName(char* name);
	bool IsGoodNCForexSymbolName(char* name);
	void AddSymbol(char* clean_name, USHORT exchange, char SecurityType, const NxCoreMessage* pNxCoreMessage);

	void OnError(const int StatusData, const char* StatusString)
	{
		_logs->Add(0, format("Error : %d %s", StatusData, StatusString));
	}
	void OnWarning(const int StatusData, const char* StatusString)
	{
		_logs->Add(0, format("Warning : %d %s", StatusData, StatusString));
	}

	static UINT ThreadNxCoreAPI(void* pparam);
	static int __stdcall CoreCallback(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMsg);

	bool IsCorrectExchange(USHORT exchange);

	int processNxCoreStatClk(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMsg);
	int processNxCoreExgQuote(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage);
	int processNxCoreTrade(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage);
	int processNxCoreSymSpin(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage);
	int processNxCoreSymChg(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage);
	int processNxCoreCategory(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage);
	int processNxCoreMMQuote(const NxCoreSystem* /*pNxCoreSys*/, const NxCoreMessage* /*pNxCoreMessage*/)
	{
		return NxCALLBACKRETURN_CONTINUE;	// return this value to continue processing messages.
	}
};

inline bool IsStockExchange(int exchange)
{
	if (exchange == NC_EXCHNAGE_NASDAQ_NATIONAL ||
		exchange == NC_EXCHNAGE_NASDAQ_SM_CAPS ||
		exchange == NC_EXCHNAGE_NYSE ||
		exchange == NC_EXCHNAGE_AMEX)
		return true;
	return false;
}

inline bool IsStockExchangeWithETF(int exchange)
{
	if (exchange == NC_EXCHNAGE_NASDAQ_NATIONAL ||
		exchange == NC_EXCHNAGE_NASDAQ_SM_CAPS ||
		exchange == NC_EXCHNAGE_PACF ||
		exchange == NC_EXCHNAGE_NYSE ||
		exchange == NC_EXCHNAGE_AMEX)
		return true;
	return false;
}

inline bool IsFuturesExchange(int exchange)
{
	if (exchange == NC_EXCHNAGE_CME ||
		exchange == NC_EXCHNAGE_CBOT)
		return true;
	return false;
}

inline bool IsForexExchange(int exchange)
{
	if (exchange == NC_EXCHNAGE_BARK)
		return true;
	return false;
}
