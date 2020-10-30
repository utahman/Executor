// Copyright(C) 2020 utahman https://github.com/utahman
// MIT License http://www.opensource.org/licenses/mit-license.php

#include "pch.h"
#include "GuiSimple.h"
#include "resource.h"
#include <commctrl.h>


INT_PTR CALLBACK MainDialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
	{
		HWND hList = GetDlgItem(hDlg, IDC_LIST_ORDERS);

		LVITEM LvItem;
		LVCOLUMN LvCol = { 0 };
		LvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;    // Type of mask
		LvCol.cx = 0x28;                                   // width between each coloum
		LvCol.pszText = (LPSTR)"Item";                            // First Header Text
		LvCol.cx = 0x42;                                   // width of column
		SendMessage(hList, LVM_SETEXTENDEDLISTVIEWSTYLE,
			0, LVS_EX_FULLROWSELECT); // Set style

		// Inserting Couloms as much as we want
		SendMessage(hList, LVM_INSERTCOLUMN, 0, (LPARAM)&LvCol); // Insert/Show the coloum
		LvCol.pszText = (LPSTR)"Sub Item1";                            // Next coloum
		SendMessage(hList, LVM_INSERTCOLUMN, 1, (LPARAM)&LvCol); // ...
		LvCol.pszText = (LPSTR)"Sub Item2";                            //
		SendMessage(hList, LVM_INSERTCOLUMN, 2, (LPARAM)&LvCol); //
		LvCol.pszText = (LPSTR)"Sub Item3";                            //
		SendMessage(hList, LVM_INSERTCOLUMN, 3, (LPARAM)&LvCol); //
		LvCol.pszText = (LPSTR)"Sub Item4";                            //
		SendMessage(hList, LVM_INSERTCOLUMN, 4, (LPARAM)&LvCol); //
		LvCol.pszText = (LPSTR)"Sub Item5";                            //
		SendMessage(hList, LVM_INSERTCOLUMN, 5, (LPARAM)&LvCol); // ...same as above

		memset(&LvItem, 0, sizeof(LvItem)); // Zero struct's Members

		//  Setting properties Of members:

		LvItem.mask = LVIF_TEXT;   // Text Style
		LvItem.cchTextMax = 256; // Max size of test
		LvItem.iItem = 0;          // choose item  
		LvItem.iSubItem = 0;       // Put in first coluom
		LvItem.pszText = (LPSTR)"Item 0"; // Text to display (can be from a char variable) (Items)

		SendMessage(hList, LVM_INSERTITEM, 0, (LPARAM)&LvItem); // Send info to the Listview

		char Temp[100];
		for (int i = 1; i <= 5; i++) // Add SubItems in a loop
		{
			LvItem.iSubItem = i;
			sprintf_s(Temp, 100, "SubItem %d", i);
			LvItem.pszText = Temp;
			SendMessage(hList, LVM_SETITEM, 0, (LPARAM)&LvItem); // Enter text to SubItems
		}

	}
	return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


HWND FindConsoleHandle()
{
	static LPCTSTR temptitle = "{32C15E1B-CC67-45F8-BFFD-0AE7869FD74C}";
	TCHAR title[512];
	if (GetConsoleTitle(title, sizeof(title) / sizeof(TCHAR)) == 0)
		return NULL;
	SetConsoleTitle(temptitle);
	HWND wnd = FindWindow(NULL, temptitle);
	SetConsoleTitle(title);
	return wnd;
}

void GuiSimple::Show()
{
    std::thread([] {
        DialogBox(NULL, MAKEINTRESOURCE(IDD_SIMPLE_GUI), FindConsoleHandle(), MainDialogProc);
        }).detach();
}
