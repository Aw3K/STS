/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <dedkam2@wp.pl> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.
 * ----------------------------------------------------------------------------
 */

#include <tchar.h>
#include <iostream>
#include <windows.h>
#include <fstream>
#include <shlobj.h>
#include <shlwapi.h>
#include <objbase.h>
using namespace std;

char textCut[100000], name[40];
string OUTNAME;
HWND textfield, button, textEnter, ErrorBox, outName, fileShow, hwnd, fileList;
WNDCLASSEX wc;
MSG msg;

void sCMD(string cmd) {
	ShellExecute(NULL, "open", "cmd.exe", string("/C " + cmd).c_str(), NULL, SW_HIDE);
}

int ERRORBOX(int IORENR, string IORESTR)
{
	string winTxt;
	switch (IORENR)
	{
		case 1:
			winTxt = "[INFO] Pomyœlnie wczytano plik.";
			break;
		case 2:
			winTxt = "[ERROR] Plik nie istnieje!";
			break;
		case 3:
			winTxt = "[ERROR] Dane wyjœcie nie istnieje!";
			if (IORESTR == "fileShow") SetWindowText(fileShow, "");
			break;
		case 4:
			winTxt = "[INFO] Pomyœlnie zmieniono nazwe wyjœcia na: \"" + IORESTR + "\".";
			break;
		case 5:
			winTxt = "[ERROR] Musisz wprowadziæ text!";
			break;
		case 6:
			winTxt = "[INFO] Pomyœlnie zapisano text!";
			break;
		case 7:
			winTxt = "[INFO] Pomyœlnie usuniêto plik z wyjœciem!";
			break;
		case 8:
			winTxt = "[ERROR] Dane wyjœcie nie istnieje!";
			break;
		case 9:
			winTxt = "[INFO] Pomyœlnie wczytano program.";
			break;
	}
	SetWindowText(ErrorBox, winTxt.c_str());
}

int roFunc()
{
	fstream check;
	check.open(OUTNAME.c_str(), ios::in);
	if (check.is_open())
	{
		string roIN, roALL = "";
		while (!check.eof())
		{
			getline ( check, roIN );
			roALL += roIN + "\r\n";
		}
		SetWindowText(textEnter, roALL.c_str());
		ERRORBOX(1, "");
		check.close();
	} else {
		check.close();
		ERRORBOX(2, "");
	}
}

int fileListFunc()
{
	fstream txtff;
	sCMD("CD STS&Chcp 65001&DIR /B *.STS > STS\\list.cfg&exit");
	txtff.open("STS\\list.cfg", ios::in);
	string txtin = " ", txtall = "", txtf = "";
	while(txtin != "")
	{
		getline( txtff, txtin );
		for (int i = 0; i<txtin.length(); i++)
		{
			if (i == txtin.length()-4) break;
			txtf += txtin[i];
		}
		txtall += txtf + "\r\n";
		txtf = "";
	}
	txtff.close();
	if (txtall == "\r\n") SetWindowText(fileList, "Nic nie znaleziono.");
	else SetWindowText(fileList, txtall.c_str());
}

int displayOut()
{
	fstream check;
	check.open(OUTNAME.c_str(), ios::in);
	if (check.is_open())
	{
		check.close();
		fstream openOut;
		openOut.open(OUTNAME.c_str(), ios::in);
		string openOuts, openOutall = "";
		do
		{
			getline( openOut, openOuts );
			openOutall += openOuts + "\r\n";
		} while(!openOut.eof());
		SetWindowText(fileShow, openOutall.c_str());
	} else {
		check.close();
		ERRORBOX(3, "fileShow");
	}
}

int nameCheck(int iksde)
{
	GetWindowText(outName, &name[0], 40);
	string namee(name), changedname = "";
	for (int i = 0; i<namee.length(); i++)
	{
		if (namee[i] == ' ') changedname += '_';
		else changedname += namee[i];
	}
	namee = changedname;
	if (namee == "default") OUTNAME = "out.STS";
	else OUTNAME = namee + ".STS";
	if (iksde == 1)
	{
		string winTxt;
		if (namee == "default") ERRORBOX(4, "out.STS");
		else ERRORBOX(4, namee);
		iksde = 0;
	}
	fstream save2;
	save2.open(".\\STS\\config.cfg", ios::out);
	save2 << namee;
	save2.close();
}

int config()
{
	fstream start;
	string namech;
	start.open(".\\STS\\config.cfg", ios::in);
	if (start.is_open())
	{
		start >> namech;
		start.close();
		SetWindowText(outName, namech.c_str());
	} else {
		start.close();
		sCMD("MKDIR STS");
	}
}

int startup()
{
	config();
	nameCheck(0);
	displayOut();
	fileListFunc();
}

int saveTofile(int XX)
{
	GetWindowText(textEnter, &textCut[0], 100000);
	if (string(textCut) == "")
	{
		ERRORBOX(5, "");
	} else {
		if (XX == 0)
		{
			fstream save;
			save.open(OUTNAME.c_str(), ios::app);
			save << &textCut[0] << endl;
			save.close();
		}
		else if (XX == 1)
		{
			fstream save;
			save.open(OUTNAME.c_str(), ios::out);
			save << &textCut[0] << endl;
			save.close();
		}
		SetWindowText(textEnter, "");
		ERRORBOX(6, "");
	}
	displayOut();
	fileListFunc();
}

int removeOut()
{
	fstream check;
	check.open(OUTNAME.c_str(), ios::in);
	if (check.is_open())
	{
		check.close();
		string DEL = "DEL /Q " + OUTNAME;
		sCMD(DEL.c_str());
		ERRORBOX(7, "");
		SetWindowText(outName, "default");
		nameCheck(0);
	} else {
		check.close();
		ERRORBOX(8, "");
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam) {
	switch(Message) {
		
		case WM_CREATE:
		{
			textfield = CreateWindow(
				"STATIC",
				"Prosty program do zapisywania textu w pliku. By Damian Kreñski.",
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
				0, 0, 500, 20,
				hwnd, NULL, NULL, NULL);
									 
			button = CreateWindow(
				"BUTTON",
				"ZAPISZ",
				WS_VISIBLE | WS_CHILD | WS_BORDER,
				0, 350, 250, 20,
				hwnd, (HMENU) 1, NULL, NULL);
				
			CreateWindow(
				"BUTTON",
				"NADPISZ",
				WS_VISIBLE | WS_CHILD | WS_BORDER,
				250, 350, 250, 20,
				hwnd, (HMENU) 4, NULL, NULL);
				
			textEnter = CreateWindow(
				"EDIT",
				"",
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL,
				10, 30, 490, 310,
				hwnd, NULL, NULL, NULL);
				
			CreateWindow(
				"BUTTON",
				"USUÑ WYJŒCIE",
				WS_VISIBLE | WS_CHILD | WS_BORDER,
				0, 370, 250, 20,
				hwnd, (HMENU) 2, NULL, NULL);
				
			CreateWindow(
				"BUTTON",
				"WCZYTAJ WYJŒCIE",
				WS_VISIBLE | WS_CHILD | WS_BORDER,
				250, 370, 250, 20,
				hwnd, (HMENU) 5, NULL, NULL);
				
			CreateWindow(
				"STATIC",
				"",
				WS_VISIBLE | WS_CHILD | WS_BORDER,
				500, 0, 10, 500,
				hwnd, NULL, NULL, NULL);
			
			CreateWindow(
				"STATIC",
				"",
				WS_VISIBLE | WS_CHILD | WS_BORDER,
				1000, 0, 10, 500,
				hwnd, NULL, NULL, NULL);

			CreateWindow(
				"EDIT",
				"Wpisz nazwê wyjœcia poni¿ej.",
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER | ES_READONLY,
				0, 390, 500, 20,
				hwnd, NULL, NULL, NULL);
			
			fileShow = CreateWindow(
				"EDIT",
				"",
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_READONLY,
				515, 5, 480, 460,
				hwnd, NULL, NULL, NULL);
			
			outName = CreateWindow(
				"EDIT",
				"default",
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
				0, 410, 500, 20,
				hwnd, NULL, NULL, NULL);
				
			CreateWindow(
				"BUTTON",
				"ZAPISZ NAZWE",
				WS_VISIBLE | WS_CHILD | WS_BORDER,
				0, 430, 500, 20,
				hwnd,  (HMENU) 3, NULL, NULL);
			
			CreateWindow(
				"STATIC",
				"Znalezione txt:",
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_CENTER,
				1010, 0, 200, 20,
				hwnd, NULL, NULL, NULL);
			
			fileList = CreateWindow(
				"EDIT",
				"",
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_READONLY,
				1014, 20, 190, 445,
				hwnd, NULL, NULL, NULL);
			
			ErrorBox = CreateWindow(
				"EDIT",
				" INFO BOX ",
				WS_VISIBLE | WS_CHILD | WS_BORDER | ES_READONLY | ES_CENTER,
				0, 450, 480, 20,
				hwnd, NULL, NULL, NULL);
				
			CreateWindow(
				"BUTTON",
				"X",
				WS_VISIBLE | WS_CHILD | WS_BORDER,
				480, 450, 20, 20,
				hwnd, (HMENU) 6, NULL, NULL);
			
			startup();
			break;
		}
		
		case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
				case 1:
				{
					saveTofile(0);
					break;
				}
				case 2:
				{
					removeOut();
					displayOut();
					fileListFunc();
					break;
				}
				case 3:
				{
					nameCheck(1);
					displayOut();
					fileListFunc();
					break;
				}
				case 4:
				{
					saveTofile(1);
					break;
				}
				case 5:
				{
					roFunc();
					break;
				}
				case 6:
				{
					sCMD("DEL /Q *.STS&RMDIR \"STS\" /S /Q");
					PostQuitMessage(0);
					break;
				}
			}
			break;
		}
		
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			break;
		}
		
		default:
			return DefWindowProc(hwnd, Message, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	
	memset(&wc,0,sizeof(wc));
	wc.cbSize		 = sizeof(WNDCLASSEX);
	wc.lpfnWndProc	 = WndProc;
	wc.hInstance	 = hInstance;
	wc.hCursor		 = LoadCursor(NULL, IDC_ARROW);
	
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+3);
	wc.lpszClassName = "WindowClass";
	wc.hIcon		 = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm		 = LoadIcon(NULL, IDI_APPLICATION);

	if(!RegisterClassEx(&wc)) {
		MessageBox(NULL, "Nie mo¿na wczytaæ programu!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	}

	hwnd = CreateWindowEx (
		WS_EX_CLIENTEDGE,
		"WindowClass",
		"STS v2.0",
		WS_VISIBLE | WS_SYSMENU | WS_MINIMIZEBOX,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1220,
		503,
		NULL,NULL,hInstance,NULL);

	if(hwnd == NULL) {
		MessageBox(NULL, "Nie mo¿na stworzyæ okna!","Error!",MB_ICONEXCLAMATION|MB_OK);
		return 0;
	} else {
		ERRORBOX(9, "");
	}

	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}
