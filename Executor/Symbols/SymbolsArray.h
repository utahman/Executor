// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"
#include "ISymbol.h"

#define MAX_STOCK_SYMBOLS 100000	// 100K is enough to make sparse hash table


class SymbolIndex
{
	int _idx = -1;
	char _name[SYMBOL_NAME_MAX_SIZE] = { 0 };
	SymbolIndex* _next = NULL;

public:
	SymbolIndex(int idx = 0, char* name = NULL)
	{
		_idx = idx;
		if (name)
			strncpy_s(_name, SYMBOL_NAME_MAX_SIZE, name, SYMBOL_NAME_MAX_SIZE);
	}
	const char* Name() { return _name; }
	int GetIndex() { return _idx; }
	void SetNext(SymbolIndex* next) { _next = next; }
	SymbolIndex* GetNext() { return _next; }
	int Compare(const char* search_name)
	{
		return strncmp(search_name, this->Name(), SYMBOL_NAME_MAX_SIZE);
	}
};


// Storage for ISymbol type
//
class SymbolsArray
{
	std::vector <ISymbol*>			_all;
	TSearchStockName <SymbolIndex>	_QuickSearchList;			// To quickly find symbol by name

public:
	SymbolsArray()
		: _QuickSearchList(MAX_STOCK_SYMBOLS)
	{
	}
	//void DestroyAllSymbols();
	ISymbol* ByIndex(int idx)
	{
		// Symbol at index 0 is always bad
		return idx > 0 && idx < (int)_all.size() ? _all[idx] : NULL;
	}
	ISymbol* FindSymbolFast(const char* name)
	{
		SymbolIndex* p = _QuickSearchList.FindFast(name);
		if (p)
			return _all[p->GetIndex()];
		return NULL;
	}
	int GetNumSymbols() { return (int)_all.size(); }
	bool AddToQuickSearchList(int idx, char* name)
	{
		SymbolIndex search_elem(idx, name);
		return _QuickSearchList.AddToListFast(search_elem);
	}
	bool Add(ISymbol* s)
	{
		int new_index = (int)_all.size();
		s->set_index(new_index); // Important to set index

		if (AddToQuickSearchList(new_index, s->get_name()))
		{
			_all.push_back(s);
			return true;
		}
		return false;
	}
};
