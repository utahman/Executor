// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#include "pch.h"
#include "GuiSimple.h"
#include "resource.h"


INT_PTR CALLBACK GuiSimple::DlgProc(HWND hwnd, UINT uMsg, WPARAM wp, LPARAM lp)
{
	GuiSimple* pDlg = reinterpret_cast<GuiSimple*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
	if (!pDlg)
	{
		if (uMsg == WM_INITDIALOG)
		{
			pDlg = reinterpret_cast<GuiSimple*>(lp);
			pDlg->set_hwnd(hwnd);
			SetWindowLongPtr(hwnd, GWLP_USERDATA, lp);

			pDlg->init();
		}
		else if (uMsg == WM_CTLCOLORDLG)
		{
			return (INT_PTR)GetStockObject(GRAY_BRUSH);
		}
		else
		{
			return 0;
		}
	}
	return pDlg->ClassDlgProc(uMsg, wp, lp);
}


void GuiSimple::Show()
{

	std::thread([this] {
		GuiSimple my_dlg(IDD_SIMPLE_GUI);
		}).detach();
}

void GuiSimple::init()
{
	// Initialize control: list of orders
	//
	_list_orders.init(_hwnd, IDC_LIST_ORDERS);
	_list_orders.add_column(FIELDo_LAST, "Last", LVCFMT_LEFT, 70);
	_list_orders.add_column(FIELDo_BID, "Bid", LVCFMT_RIGHT, 70);
	_list_orders.add_column(FIELDo_ASK, "Ask", LVCFMT_LEFT, 70);
	_list_orders.add_column(FIELDo_NAME, "Name", LVCFMT_LEFT, 70);
	_list_orders.add_column(FIELDo_PRICE, "Price", LVCFMT_CENTER, 70);
	_list_orders.add_column(FIELDo_SIZE, "Size", LVCFMT_CENTER, 60);
	_list_orders.add_column(FIELDo_SIZE_EXEC, "Exec", LVCFMT_CENTER, 60);
	_list_orders.add_column(FIELDo_TYPE, "Type", LVCFMT_CENTER, 65);
	_list_orders.add_column(FIELDo_STATUS, "Status", LVCFMT_CENTER, 65);
	_list_orders.add_column(FIELDo_INVESTED, "Invest", LVCFMT_CENTER, 70);
	_list_orders.add_column(FIELDo_TIME, "Time", LVCFMT_CENTER, 60);
	_list_orders.add_column(FIELDo_ORDER_NUM, "Num", LVCFMT_CENTER, 50);
	_list_orders.add_column(FIELDo_COMMENT, "Comment", LVCFMT_CENTER, 110);


	ListView_DeleteAllItems(GetDlgItem(_hwnd, IDC_LIST_ORDERS));

	for (int idx = 0; idx < 10; idx++)
	{
		int nItemIndex;
		LVITEM item = { LVIF_TEXT, (int)idx, 0, 0, 0, (char*)"AAA" };

		nItemIndex = ListView_InsertItem(GetDlgItem(_hwnd, IDC_LIST_ORDERS), &item);
		ListView_SetItemText(GetDlgItem(_hwnd, IDC_LIST_ORDERS), nItemIndex, 1, (char*)"BBB");
		ListView_SetItemText(GetDlgItem(_hwnd, IDC_LIST_ORDERS), nItemIndex, 2, (char*)"CCC");
	}

	ListView_SetColumnWidth(GetDlgItem(_hwnd, IDC_LIST_ORDERS), 0, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetColumnWidth(GetDlgItem(_hwnd, IDC_LIST_ORDERS), 1, LVSCW_AUTOSIZE_USEHEADER);
	ListView_SetColumnWidth(GetDlgItem(_hwnd, IDC_LIST_ORDERS), 2, LVSCW_AUTOSIZE_USEHEADER);

	// Initialize control: list of positions
	//
	_list_positions.init(_hwnd, IDC_LIST_POSITIONS);
	_list_positions.add_column(FIELDp_TR_TIME, "tm Trade", LVCFMT_LEFT, 60);
	_list_positions.add_column(FIELDp_BA_TIME, "tm B/A", LVCFMT_LEFT, 60);
	_list_positions.add_column(FIELDp_LAST, "Last", LVCFMT_LEFT, 60);
	_list_positions.add_column(FIELDp_BID, "Bid", LVCFMT_RIGHT, 60);
	_list_positions.add_column(FIELDp_ASK, "Ask", LVCFMT_LEFT, 60);
	_list_positions.add_column(FIELDp_CHOPEN, "ChOpen", LVCFMT_LEFT, 60);
	_list_positions.add_column(FIELDp_LOW, "Low", LVCFMT_RIGHT, 60);
	_list_positions.add_column(FIELDp_NAME, "Name", LVCFMT_LEFT, 70);
	_list_positions.add_column(FIELDp_PRICE, "Bought", LVCFMT_CENTER, 100);
	_list_positions.add_column(FIELDp_SIZE, "Size", LVCFMT_CENTER, 60);
	_list_positions.add_column(FIELDp_TIME, "Time", LVCFMT_CENTER, 60);
	_list_positions.add_column(FIELDp_DIFF_D, "Diff$", LVCFMT_CENTER, 70);
	_list_positions.add_column(FIELDp_DIFF_P, "Diff%", LVCFMT_CENTER, 60);
	_list_positions.add_column(FIELDp_INVESTED, "Invested", LVCFMT_CENTER, 80);

	// Initialize control: list of executions
	//
	_list_executions.init(_hwnd, IDC_LIST_EXECUTIONS);
	_list_executions.add_column(FIELDe_NAME, "Name", LVCFMT_LEFT, 80);
	_list_executions.add_column(FIELDe_PRICE, "Price", LVCFMT_CENTER, 110);
	_list_executions.add_column(FIELDe_SIZE, "Size", LVCFMT_CENTER, 60);
	_list_executions.add_column(FIELDe_ETYPE, "Type", LVCFMT_CENTER, 80);
	_list_executions.add_column(FIELDe_TIME, "Time", LVCFMT_CENTER, 60);
	_list_executions.add_column(FIELDe_ORDER_NUM, "Num", LVCFMT_CENTER, 50);

	// Initialize control: list of trades
	//
	_list_trades.init(_hwnd, IDC_LIST_TRADES);
	_list_trades.add_column(FIELDt_NAME, "Name", LVCFMT_LEFT, 80);
	_list_trades.add_column(FIELDt_PRICE_B, "Bought", LVCFMT_CENTER, 110);
	_list_trades.add_column(FIELDt_PRICE_S, "Sold", LVCFMT_CENTER, 60);
	_list_trades.add_column(FIELDt_SIZE, "Size", LVCFMT_CENTER, 60);
	_list_trades.add_column(FIELDt_DIFF, "Diff", LVCFMT_CENTER, 70);
	_list_trades.add_column(FIELDt_TIME_BUY, "Buy", LVCFMT_CENTER, 90);
	_list_trades.add_column(FIELDt_TIME_SELL, "Sell", LVCFMT_CENTER, 90);
}

