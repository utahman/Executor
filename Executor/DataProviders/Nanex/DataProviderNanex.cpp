// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#include "pch.h"
#include "DataProviderNanex.h"
#include "ToFile/LogsToFile.h"
#include "Stock/SymbolStock.h"

DataProviderNanex* DataProviderNanex::_object = NULL;


DataProviderNanex::DataProviderNanex()
{
	_object = this;
	const wchar_t* section = L"Nanex";
	auto logs_obj = g.get(section, L"logs_obj");
	auto logs_path = g.get(section, L"logs_path");
	auto logs_file_name = g.get(section, L"logs_file_name");

	// Always use a copy of the dll, not the one in Program Files
	_dll_path = g.get(section, L"dll_path");

	if (logs_obj == "ToFile")
		_logs = new LogsToFile(logs_path, logs_file_name);
	_logs->Add(g._system_msec, format("Nanex starting"));
	_UseSecurities = g.get(section, L"use_securities", 1);

	_tapes_folder = g.get(section, L"tapes_folder");
	_tape_name = g.get(section, L"tape_name"); // Leave blank to run latest tape
	_tape_name = sformat("%s/%s", _tapes_folder.c_str(), _tape_name.c_str());

	Start();
}


int DataProviderNanex::processNxCoreStatClk(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* /*pNxCoreMsg*/)
{
	int rett = NxCALLBACKRETURN_CONTINUE;
	_tapeStatus = pNxCoreSys->Status;
	if (pNxCoreSys->ClockUpdateInterval >= NxCLOCK_CLOCK)
	{
		const NxTime& t = pNxCoreSys->nxTime;
		set_time(t.Hour, t.Minute, t.Second, t.Millisecond);
	}

	switch (pNxCoreSys->Status)
	{
	case NxCORESTATUS_RUNNING:
		break;
	case NxCORESTATUS_INITIALIZING: // Tape starting
		_bEndOfTape = false;
		//g_Symbols.DestroyAllSymbols();
		break;
	case NxCORESTATUS_COMPLETE:		// Tape ended
		_bEndOfTape = true;
		break;
	case NxCORESTATUS_SYNCHRONIZING:
		break;
	case NxCORESTATUS_ERROR:
		OnError(pNxCoreSys->StatusData, pNxCoreSys->StatusDisplay);
		break;
	case NxCORESTATUS_WAITFORCOREACCESS:
		break;
	case NxCORESTATUS_RESTARTING_TAPE:
		break;
	case NxCORESTATUS_LOADED_STATE:
		break;
	case NxCORESTATUS_SAVING_STATE:
		break;
	case NxCORESTATUS_SYMBOLSPIN:
		break;
	}
	return rett;
}

bool DataProviderNanex::IsCorrectExchange(USHORT exchange)
{
	switch (exchange)
	{
	case NC_EXCHNAGE_NYSE:
		return _UseSecurities & TYPE_STOCK;
	case NC_EXCHNAGE_AMEX:
		return _UseSecurities & TYPE_STOCK;
	case NC_EXCHNAGE_NASDAQ_NATIONAL:
		return _UseSecurities & TYPE_STOCK;
	case NC_EXCHNAGE_NASDAQ_SM_CAPS:
		return _UseSecurities & TYPE_STOCK;
	case NC_EXCHNAGE_PACF:
		return _UseSecurities & TYPE_STOCK;

	case NC_EXCHNAGE_CME:
		return (_UseSecurities & TYPE_FUTURE) != 0;
	case NC_EXCHNAGE_CBOT:
		return (_UseSecurities & TYPE_FUTURE) != 0;

	case NC_EXCHNAGE_BARK:
		return (_UseSecurities & TYPE_FOREX) != 0;

	default:
		return false; // Ignore exchange
	}
}

int DataProviderNanex::processNxCoreExgQuote(const NxCoreSystem* /*pNxCoreSys*/, const NxCoreMessage* pNxCoreMessage)
{
	if (_exiting)
		return NxCALLBACKRETURN_STOP;

	int idx = pNxCoreMessage->coreHeader.pnxStringSymbol->UserData1;
	if (idx <= 0)
		return NxCALLBACKRETURN_CONTINUE; // It is not a symbol that we are watching

	USHORT exchange = pNxCoreMessage->coreHeader.ListedExg;
	if (IsCorrectExchange(exchange) == false)
		return NxCALLBACKRETURN_CONTINUE; //------------- Unsupported exchange --------------------------->

	if (pNxCoreMessage->coreHeader.pnxStringSymbol == NULL)
	{
		OnError(0, "pNxCoreMessage->coreHeader.pnxStringSymbol == NULL");
		return NxCALLBACKRETURN_CONTINUE; //------------- Error ---------------------------------------->
	}

	const NxCoreExgQuote& quote = pNxCoreMessage->coreData.ExgQuote;
	const NxCoreQuote& cquote = quote.coreQuote;
	if (cquote.Refresh == 0)	// Refresh==0 when everything is ok
	{
		ISymbol* s = g._symbols.ByIndex(idx);
		bool changed = false;
		float bid = 0, ask = 0;
		int bid_size = 0, ask_size = 0;

		if (s->get_security_type() & TYPE_STOCK) // Stock
		{
			if (quote.BestBidPriceChange || quote.BestAskPriceChange)
			{
				bid = (float)_core.PriceToDouble(quote.BestBidPrice, cquote.PriceType);
				ask = (float)_core.PriceToDouble(quote.BestAskPrice, cquote.PriceType);
				bid_size = quote.BestBidSize * 100;
				ask_size = quote.BestAskSize * 100;
				changed = true;
			}
		}
		else		 // Future or Forex
		{
			if (cquote.BidPriceChange || cquote.AskPriceChange)
			{
				bid = (float)_core.PriceToDouble(cquote.BidPrice, cquote.PriceType);
				ask = (float)_core.PriceToDouble(cquote.AskPrice, cquote.PriceType);
				bid_size = cquote.BidSize * 100;
				ask_size = cquote.AskSize * 100;
				changed = true;
			}
		}

		if (changed)
		{
			// Use locally retrieved time_msec
			const NxTime& t = pNxCoreMessage->coreHeader.nxExgTimestamp;
			int time_msec = TimeToMillisecGlobal(t.Hour, t.Minute, t.Second, t.Millisecond);
			s->set_bid_ask(bid, ask, bid_size, ask_size, time_msec);
		}
	}

	return NxCALLBACKRETURN_CONTINUE;	// return this value to continue processing messages.
}


int DataProviderNanex::processNxCoreTrade(const NxCoreSystem* /*pNxCoreSys*/, const NxCoreMessage* pNxCoreMessage)
{
	if (_exiting)
		return NxCALLBACKRETURN_STOP;

	int idx = pNxCoreMessage->coreHeader.pnxStringSymbol->UserData1;
	if (idx <= 0)
		return NxCALLBACKRETURN_CONTINUE; // It is not a symbol that we are watching

	USHORT exchange = pNxCoreMessage->coreHeader.ListedExg;
	if (IsCorrectExchange(exchange) == false)
		return NxCALLBACKRETURN_CONTINUE; //------------- Unsupported exchange --------------------------->

	const NxCoreTrade& nt = pNxCoreMessage->coreData.Trade;
	if ((nt.PriceFlags & (NxTPF_EXGINSERT | NxTPF_EXGCANCEL)) == 0)	// If not insertion or cancellation
	{
		if (pNxCoreMessage->coreHeader.pnxStringSymbol == NULL)
		{
			OnError(2, "Error-2: pNxCoreMessage->coreHeader.pnxStringSymbol == NULL");
			return NxCALLBACKRETURN_CONTINUE; //------------- Error ---------------------------------------->
		}

		const NxTime& t = pNxCoreMessage->coreHeader.nxExgTimestamp;
		int time_msec = TimeToMillisecGlobal(t.Hour, t.Minute, t.Second, t.Millisecond);

		if ((nt.ConditionFlags & NxTCF_NOLAST) == 0 && (nt.PriceFlags & NxTPF_SETLAST) != 0)
		{
			float last = (float)_core.PriceToDouble(nt.Last, nt.PriceType);
			if (last > 0)
			{
				// Continue if market open OR update is not for stock
				//
				ISymbol* s = g._symbols.ByIndex(idx);
				if (_market_open_US_stocks || s->get_security_type() != TYPE_STOCK)
				{
					// Use locally retrieved time_msec
					int size = nt.VolumeType == NxTVT_INCRVOL ? nt.Size : 1;
					s->set_last(last, float(nt.TotalVolume), size, time_msec);
				}
			}
		}
		else // This is pre-market last for stocks
		{
			float price = (float)_core.PriceToDouble(nt.Price, nt.PriceType);
			if (price > 0)
			{
				// Use locally retrieved time_msec
				ISymbol* s = g._symbols.ByIndex(idx);
				int size = nt.VolumeType == NxTVT_INCRVOL ? nt.Size : 1;
				s->set_price_premarket(price, float(nt.TotalVolume), size, time_msec);
			}
		}
	}
	return NxCALLBACKRETURN_CONTINUE;	// return this value to continue processing messages.
}

bool DataProviderNanex::IsGoodNCStockSymbolName(char* raw_name)
{
	if (raw_name == NULL || raw_name[0] != 'e')	// 'e' is used by NcCore for equity symbols
		return false;
	int len = (int)strlen(raw_name) - 1; // -1 to remove 'e'
	if (len < 1 || len > SYMBOL_NAME_MAX_SIZE)
		return false;
	for (int ii = 1; ii < len; ii++)	// Start from 1 because first is 'e' (see comment above)
	{
		if (raw_name[ii] < 'A' || raw_name[ii] > 'Z')
			return false;
	}
	return true;
}

bool DataProviderNanex::IsGoodNCFuturesSymbolName(char* raw_name)
{
	if (raw_name == NULL || raw_name[0] != 'f')	// 'f' is used by NcCore for futures symbols
		return false;
	int len = (int)strlen(raw_name) - 1; // -1 to remove 'f'
	if (len < 1 || len > SYMBOL_NAME_MAX_SIZE) // Same size for futures
		return false;
	for (int ii = 1; ii < len; ii++)	// Start from 1 because first is 'f' (see comment above)
	{
		if ((raw_name[ii] >= 'A' && raw_name[ii] <= 'Z') ||
			(raw_name[ii] >= '0' && raw_name[ii] <= '9') ||
			raw_name[ii] == '.' ||
			raw_name[ii] == '$')
		{
		}
		else
			return false;
	}
	return true;
}

bool DataProviderNanex::IsGoodNCForexSymbolName(char* raw_name)
{
	if (raw_name == NULL || raw_name[0] != 'c')	// 'c' is used by NcCore for currency symbols
		return false;
	int len = (int)strlen(raw_name) - 1; // -1 to remove 'c'
	if (len != 7)		// We use only currencies data as "USD.EUR" and skip futures and options as "USD.EUR.6M"
		return false;
	return true;
}

//FILE* fh = NULL;


char FindSecurityType(char* name)
{
	switch (name[0])
	{
	case 'e':
		return IDataProvider::TYPE_STOCK;
	case 'f':
		return IDataProvider::TYPE_FUTURE;
	case 'c':
		return IDataProvider::TYPE_FOREX;
	default:
		return IDataProvider::TYPE_UNK;
	}
}

int DataProviderNanex::processNxCoreSymSpin(const NxCoreSystem* /*pNxCoreSys*/, const NxCoreMessage* pNxCoreMessage)
{
	USHORT exchange = pNxCoreMessage->coreHeader.ListedExg;
	if (IsCorrectExchange(exchange) == false)
		return NxCALLBACKRETURN_CONTINUE; //------------- Unsupported exchange --------------------------->

	if (pNxCoreMessage->coreHeader.pnxStringSymbol == NULL)
	{
		OnError(3, "pNxCoreMessage->coreHeader.pnxStringSymbol == NULL");
		return NxCALLBACKRETURN_CONTINUE; //------------- Error ---------------------------------------->
	}
	char* raw_name = pNxCoreMessage->coreHeader.pnxStringSymbol->String;
	char SecurityType = FindSecurityType(raw_name);

	if (SecurityType == TYPE_STOCK) // Stocks
	{
		if (IsGoodNCStockSymbolName(raw_name) == false)
			return NxCALLBACKRETURN_CONTINUE; //----------- not a good equity symbol ------------->
	}
	else if (SecurityType == TYPE_FUTURE) // Futures
	{
		if (IsGoodNCFuturesSymbolName(raw_name) == false)
			return NxCALLBACKRETURN_CONTINUE; //----------- not a good futures symbol ------------->
	}
	else if (SecurityType == TYPE_FOREX) // Currencies
	{
		if (IsGoodNCForexSymbolName(raw_name) == false)
			return NxCALLBACKRETURN_CONTINUE; //----------- not a good forex symbol ------------->
	}
	else
	{
		return NxCALLBACKRETURN_CONTINUE;	// Error
	}

	char* clean_name = GetNameFromNxCoreName(raw_name);

	// Check with current Algo if it wants to use this symbol
	//
	if(g._algo->CheckToAddSymbol(SecurityType, exchange, clean_name))
		AddSymbol(clean_name, exchange, SecurityType, pNxCoreMessage);

	return NxCALLBACKRETURN_CONTINUE;	// return this value to continue processing messages.
}


char* DataProviderNanex::GetNameFromNxCoreName(char* core_name)
{
	if (core_name)
		return core_name + 1;	// Just remove first symbol which is 'e' added by NxCore (for stocks) or other
	else
		return NULL;
}


void DataProviderNanex::AddSymbol(char* clean_name, USHORT exchange, char /*SecurityType*/,
	const NxCoreMessage* pNxCoreMessage)
{
	// Set default values:
	pNxCoreMessage->coreHeader.pnxStringSymbol->UserData1 = -1;
	pNxCoreMessage->coreHeader.pnxStringSymbol->UserData2 = -1;

	if (g._symbols.GetNumSymbols() == 0)	// Make symbol with zero index invalid
	{
		ISymbol* sb = new SymbolStock(_UnusedStockName, exchange);
		g._symbols.Add(sb);
	}

	// Before adding symbol to the list make sure it has unique name (otherwise we can't find later which is which)
	// To find uniqueness we add name to the 'm_QuickSearchList' which returns 'false' if name is not unique
	// Skipping some stocks with non-unique name will not impact us because such event is rare (2 or so a day)
	//
	auto s = new SymbolStock(clean_name, exchange);
	//Symbol* s = MD_CreateNewSymbolObj(clean_name, exchange, SecurityType);
	if (s) // Since we ignore some names it can be NULL
	{
		if (true == g._symbols.Add(s))
		{
			if (pNxCoreMessage) // For NC native symbols only
				pNxCoreMessage->coreHeader.pnxStringSymbol->UserData1 = s->get_index();
		}
		else
		{
			_logs->Add(0, format("Warning : delete symbol %s", s->get_name()));
			delete s; // Important to delete because it was created using 'new'
		}
	}
}

void DataProviderNanex::AddForegnSymbol(char* name, USHORT exchange, char SecurityType)
{
	// This is used when we add symbol which is not in the NC stream
	//
	AddSymbol(name, exchange, SecurityType, NULL);
}

int DataProviderNanex::processNxCoreSymChg(const NxCoreSystem* /*pNxCoreSys*/, const NxCoreMessage* pNxCoreMessage)
{
	// Here we are interested in Modify only:
	// If new symbol added we can ignore today
	// If symbol deleted it still be ok until end of day
	//
	if (pNxCoreMessage->coreData.SymbolChange.Status != NxSS_MOD ||
		pNxCoreMessage->coreData.SymbolChange.StatusChar != 'M')
		return NxCALLBACKRETURN_CONTINUE;	// -------------- not a modify ---------->

	NxString* old = pNxCoreMessage->coreData.SymbolChange.pnxsSymbolOld;
	if (old == NULL)
	{
		OnError(4, "pNxCoreMessage->coreData.SymbolChange.pnxsSymbolOld == NULL");
		return NxCALLBACKRETURN_CONTINUE;	// -------------- no data ---------->
	}

	int idx = old->UserData1;
	if (idx <= 0 || idx >= g._symbols.GetNumSymbols())
		return NxCALLBACKRETURN_CONTINUE;	// -------------- not a subscribed symbol ---------->

	char* old_name = GetNameFromNxCoreName(old->String);
	USHORT old_exchange = pNxCoreMessage->coreData.SymbolChange.ListedExgOld;

	if (pNxCoreMessage->coreHeader.pnxStringSymbol == NULL)
	{
		OnError(5, "pNxCoreMessage->coreHeader.pnxStringSymbol == NULL!!!!!!!!!");
		return NxCALLBACKRETURN_CONTINUE; //------------- Error ---------------------------------------->
	}

	NxString* now = pNxCoreMessage->coreHeader.pnxStringSymbol;
	if (IsGoodNCStockSymbolName(now->String) == false)
		return NxCALLBACKRETURN_CONTINUE;	// -------------- new symbol name is not good---------->

	USHORT new_exchange = pNxCoreMessage->coreHeader.ListedExg;
	char* new_name = GetNameFromNxCoreName(now->String);

	ISymbol* s = g._symbols.ByIndex(idx);
	if (s == NULL)
	{
		OnError(6, "processNxCoreSymChg");
		return NxCALLBACKRETURN_CONTINUE;
	}
	if (strcmp(s->get_name(), old_name) != 0 || s->get_exchange() != old_exchange)
	{
		OnError(7, "processNxCoreSymChg");
		return NxCALLBACKRETURN_CONTINUE;
	}

	// Add new name to quick search list
	// (we don't have to remove old name; nothing will happen)
	//
	if (strcmp(s->get_name(), new_name) != 0)	// If name is really new; not just exchange change
	{
		g._symbols.AddToQuickSearchList(idx, new_name);
	}

	s->set_name(new_name);				// Set new name (name is already normilized)
	s->set_exchange(new_exchange);		// Set new exchange

	now->UserData1 = idx;		// Set index for new symbol
	old->UserData1 = -1;		// Zeroing old index

	// Add names to _renames vector but only if names are different
	// (names will be the same if only exchange is changed)
	//
	if (strcmp(old_name, new_name) != 0)
		_renames.push_back(std::make_pair(old_name, new_name));

	_logs->Add(0, format("processNxCoreSymChg: from [%s] to [%s]", old_name, new_name));
	return NxCALLBACKRETURN_CONTINUE;	// return this value to continue processing messages.
}


float DataProviderNanex::FieldToFloat(NxCategoryField& f)
{
	if (f.FieldType == NxCFT_PRICE)
		return (float)_core.PriceToDouble(f.data.nxPrice.Price, f.data.nxPrice.PriceType);
	else if (f.FieldType == NxCFT_32BIT)
		return (float)f.data.i32Bit;
	else
	{
		_logs->Add(0, format("Error: FieldToFloat: Unknown field type: %d", int(f.FieldType)));
		return 0;
	}
}


int DataProviderNanex::processNxCoreCategory(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMessage)
{
	if (pNxCoreMessage->coreHeader.pnxStringSymbol == NULL)
	{
		OnError(5, "pNxCoreMessage->coreHeader.pnxStringSymbol == NULL");
		return NxCALLBACKRETURN_CONTINUE; //------------- Error ---------------------------------------->
	}

	const NxCoreCategory& c = pNxCoreMessage->coreData.Category;
	USHORT ctype = c.pnxStringCategory->Atom;

	int idx = pNxCoreMessage->coreHeader.pnxStringSymbol->UserData1;
	if (idx <= 0 || idx >= (int)g._symbols.GetNumSymbols())
		return NxCALLBACKRETURN_CONTINUE;

	const NxTime& t = pNxCoreSys->nxTime;
	set_time(t.Hour, t.Minute, t.Second, t.Millisecond);

	ISymbol* symbol = g._symbols.ByIndex(idx);

	if (ctype == CATEGORY_TYPE_DTNEquityFundamental)
	{
		for (int ii = 0; ii < c.NFields; ii++)
		{
			NxCategoryField& f = c.pnxFields[ii];
			if (strcmp(f.FieldName, CATEGORY_STR_Volatility) == 0)
				symbol->set_volatility(FieldToFloat(f));
			else if (strcmp(f.FieldName, CATEGORY_STR_AvgVolume) == 0)
				symbol->set_average_volume(FieldToFloat(f));
			else if (strcmp(f.FieldName, CATEGORY_STR_PE) == 0)
				symbol->set_pe(FieldToFloat(f));
			else if (strcmp(f.FieldName, CATEGORY_STR_Yield) == 0)
				symbol->set_yield(FieldToFloat(f));
		}
	}
	else if (ctype == CATEGORY_TYPE_EquityFundamental)
	{
		for (int ii = 0; ii < c.NFields; ii++)
		{
			NxCategoryField& f = c.pnxFields[ii];
			if (strcmp(f.FieldName, CATEGORY_STR_Beta) == 0)
				symbol->set_beta(FieldToFloat(f));
		}
	}
	else if (ctype == CATEGORY_TYPE_52WeekHiLo)
	{
		for (int ii = 0; ii < c.NFields; ii++)
		{
			NxCategoryField& f = c.pnxFields[ii];
			if (strcmp(f.FieldName, CATEGORY_STR_52WeekHigh) == 0)
				symbol->set_52WeekHigh(FieldToFloat(f));
			else if (strcmp(f.FieldName, CATEGORY_STR_52WeekLow) == 0)
				symbol->set_52WeekLow(FieldToFloat(f));
		}
	}
	else if (ctype == CATEGORY_TYPE_OHLC)
	{
		if (t.Hour < 6) // We saw OHCL sometimes coming at the end of day - ignore those
		{
			float open = 0, high = 0, low = 0, close = 0;
			for (int ii = 0; ii < c.NFields; ii++)
			{
				NxCategoryField& f = c.pnxFields[ii];
				if (strcmp(f.FieldName, CATEGORY_STR_Open) == 0)
					open = FieldToFloat(f);
				else if (strcmp(f.FieldName, CATEGORY_STR_High) == 0)
					high = FieldToFloat(f);
				else if (strcmp(f.FieldName, CATEGORY_STR_Low) == 0)
					low = FieldToFloat(f);
				else if (strcmp(f.FieldName, CATEGORY_STR_Close) == 0)
					close = FieldToFloat(f);
				else if (strcmp(f.FieldName, CATEGORY_STR_Volume) == 0)
				{
					// For futures we do not recive Average65 volume
					// So lets use for now previous day volume as Average65
					//
					if (symbol->get_security_type() == TYPE_FUTURE)
					{
						float prevDayVolume = FieldToFloat(f);
						symbol->set_average_volume(prevDayVolume);
					}
				}
			}
			symbol->set_prev_day_OHLC(open, high, low, close);
		}
	}

	return NxCALLBACKRETURN_CONTINUE;	// return this value to continue processing messages.
}


int DataProviderNanex::ExceptionFilter(unsigned int code, struct _EXCEPTION_POINTERS* ep)
{
	printf("\nError: InFilter: code = %x  code2 = %x",
		code,
		ep->ExceptionRecord->ExceptionCode);
	PEXCEPTION_RECORD rec = ep->ExceptionRecord;
	while (rec)
	{
		printf("  addr = %p", rec->ExceptionAddress);
		rec = rec->ExceptionRecord;
	}
	printf("\n");
	return EXCEPTION_EXECUTE_HANDLER;
}


UINT DataProviderNanex::ThreadNxCoreAPI(void*)
{
	_object->_thread_running = true;

	__try
	{
		if (_object->_core.LoadNxCore(_object->_dll_path.c_str()))	// Load dll
		{
			// call the function to process a tape. If NxCoreAccess is running -- passing a string
			// of "" will select the active stream that NxCoreAccess is receiving.
			//
			unsigned int flags = 0;			// NxCF_EXCLUDE_CRC_CHECK;
			_object->_core.ProcessTape(_object->_tape_name.c_str(), 0, flags, 0, CoreCallback);

			// ProcessTape will not return until either your callback function (NxCoreAPICallback)
			// returns NxCALLBACKRETURN_STOP, or the tape has been processed to completion,
			// or an unrecoverable error has occurred.
			//
			_object->_core.UnloadNxCore();
		}
		else
		{
			_object->OnError(0, "LoadNxCore failed");
		}
	}
	__except (ExceptionFilter(GetExceptionCode(), GetExceptionInformation()))
	{
		_object->OnError(0, "Exception in NC!!!");
	}

	_object->_thread_running = false;
	_object->_bEndOfTape = true;
	_object->_initialized = false;

	return 0;
}


int __stdcall DataProviderNanex::CoreCallback(const NxCoreSystem* pNxCoreSys, const NxCoreMessage* pNxCoreMsg)
{
	if (_object)
	{
		if (_object->_exiting == true)
			return NxCALLBACKRETURN_STOP;

		switch (pNxCoreMsg->MessageType)
		{
		case NxMSG_STATUS:			return _object->processNxCoreStatClk(pNxCoreSys, pNxCoreMsg);
		case NxMSG_EXGQUOTE:		return _object->processNxCoreExgQuote(pNxCoreSys, pNxCoreMsg);
		case NxMSG_MMQUOTE:			return _object->processNxCoreMMQuote(pNxCoreSys, pNxCoreMsg);
		case NxMSG_TRADE:			return _object->processNxCoreTrade(pNxCoreSys, pNxCoreMsg);
		case NxMSG_CATEGORY:		return _object->processNxCoreCategory(pNxCoreSys, pNxCoreMsg);
		case NxMSG_SYMBOLCHANGE:	return _object->processNxCoreSymChg(pNxCoreSys, pNxCoreMsg);
		case NxMSG_SYMBOLSPIN:		return _object->processNxCoreSymSpin(pNxCoreSys, pNxCoreMsg);
			break;
		default:
			_object->OnError(0, "Unknown MessageType");
		}
	}
	return NxCALLBACKRETURN_CONTINUE;
}


bool DataProviderNanex::Start()
{
	if (_initialized)
		return false; // -------- only once ------->
	_initialized = true;
	_bEndOfTape = false;

	DWORD idThread;
	HANDLE hndThread = ::CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ThreadNxCoreAPI, 0, 0, &idThread);
	if (hndThread)
	{
		_logs->Add(0, format("Tape started: \"%s\"", _tape_name.c_str()));
		::CloseHandle(hndThread);
		return true;
	}
	OnError(0, "Error: Start tape failed");
	return false;
}
