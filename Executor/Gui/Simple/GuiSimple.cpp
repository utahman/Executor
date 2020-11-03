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
	// Initialize list of order control
	//
	_list_orders.init(_hwnd, IDC_LIST_ORDERS);
	_list_orders.add_column((char*)"aaa");
	_list_orders.add_column((char*)"bbb");
	_list_orders.add_column((char*)"ccc");

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
}

