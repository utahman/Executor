// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#pragma once
#include "Config.h"
#include <commctrl.h>

// Columns for Orders window
#define FIELDo_LAST			0
#define FIELDo_BID			1
#define FIELDo_ASK			2
#define FIELDo_NAME			3
#define FIELDo_PRICE		4
#define FIELDo_SIZE			5
#define FIELDo_SIZE_EXEC	6
#define FIELDo_TYPE			7
#define FIELDo_STATUS		8
#define FIELDo_INVESTED		9
#define FIELDo_TIME			10
#define FIELDo_ORDER_NUM	11
#define FIELDo_COMMENT		12

// Columns for Positions window
#define FIELDp_TR_TIME		0
#define FIELDp_BA_TIME		1
#define FIELDp_LAST			2
#define FIELDp_BID			3
#define FIELDp_ASK			4
#define FIELDp_CHOPEN		5
#define FIELDp_LOW			6
#define FIELDp_NAME			7
#define FIELDp_PRICE		8
#define FIELDp_SIZE			9
#define FIELDp_TIME			10
#define FIELDp_DIFF_D		11
#define FIELDp_DIFF_P		12
#define FIELDp_INVESTED		13

// Columns for Executions window
#define FIELDe_NAME			0
#define FIELDe_PRICE		1
#define FIELDe_SIZE			2
#define FIELDe_ETYPE		3
#define FIELDe_TIME			4
#define FIELDe_ORDER_NUM	5

// Columns for Trades window
#define FIELDt_NAME			0
#define FIELDt_PRICE_B		1
#define FIELDt_PRICE_S		2
#define FIELDt_SIZE			3
#define FIELDt_DIFF			4
#define FIELDt_TIME_BUY		5
#define FIELDt_TIME_SELL	6


class ListView
{
	HWND _item = NULL;

public:
	void init(HWND dlg, DWORD idc)
	{
		_item = GetDlgItem(dlg, idc);
	}

	int add_column(int nCol, LPCTSTR lpszColumnHeading,
		int nFormat = LVCFMT_LEFT, int nWidth = -1)
	{
		LVCOLUMN lvc = { 0 };
		lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;

		lvc.iSubItem = nCol;
		lvc.pszText = (LPSTR)lpszColumnHeading;
		lvc.cx = nWidth;
		lvc.fmt = nFormat;
		return ListView_InsertColumn(_item, lvc.iSubItem, &lvc);
	}
};


class GuiSimple : public IGui
{
	ListView _list_orders;
	ListView _list_positions;
	ListView _list_executions;
	ListView _list_trades;
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

