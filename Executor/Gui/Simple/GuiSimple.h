// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"
#include <commctrl.h>

class ListView
{
	HWND _item = NULL;
	int _icolumn = 0;

public:
	void init(HWND dlg, DWORD idc)
	{
		_item = GetDlgItem(dlg, idc);
	}
	int add_column(char* text, int width = 100, int align = LVCFMT_LEFT)
	{
		LVCOLUMN lvc = { 0 };
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

		lvc.iSubItem = _icolumn++;
		lvc.pszText = text;
		lvc.cx = width;
		lvc.fmt = align;
		return ListView_InsertColumn(_item, lvc.iSubItem, &lvc);
	}
};


class GuiSimple : public IGui
{
	ListView _list_orders;
	HWND _hwnd = 0;

public:
	GuiSimple(const int id = 0, const HWND hParent = 0)
	{
		DialogBoxParam(GetModuleHandle(0), MAKEINTRESOURCE(id), hParent, DlgProc,
			reinterpret_cast<LPARAM>(this));
	}
	~GuiSimple()
	{
	}

	void Show();
	void Stop()
	{
	}
	void set_hwnd(HWND hwnd) { _hwnd = hwnd; }

private:
	void init();
	static INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp);

	INT_PTR ClassDlgProc(UINT uMsg, WPARAM wp, LPARAM)
	{
		if (uMsg == WM_COMMAND)
		{
			if (LOWORD(wp) == IDOK || LOWORD(wp) == IDCANCEL)
			{
				EndDialog(_hwnd, LOWORD(wp));
				return 1;
			}
		}
		return 0;
	}

private:
	virtual void Data_Process(FastData* /*pdata*/)	// From DataProcessor
	{
	}
};

