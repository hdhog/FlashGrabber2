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
#include "resource.h"
#include "resursedit.h"
wchar_t binpath[MAX_PATH];

/******************* Функция Диалога открытия файла *******************/
wchar_t * ShowOpenDialog(HWND hWnd)
{
	wchar_t filename[MAX_PATH];
	OPENFILENAMEW openfile;	
	ZeroMemory(&openfile, sizeof(openfile));
	openfile.lStructSize = sizeof(openfile);
	openfile.hwndOwner = hWnd;
	openfile.lpstrFile = filename;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	openfile.lpstrFile[0] = '\0';
	openfile.nMaxFile = sizeof(filename);
	openfile.lpstrFilter = L"Exe\0*.EXE\0";
	openfile.nFilterIndex = 1;
	openfile.lpstrFileTitle = NULL;
	openfile.nMaxFileTitle = 0;
	openfile.lpstrInitialDir = NULL;
	openfile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;	
	if (GetOpenFileName(&openfile)==TRUE)
	{		
		lstrcpy(filename,openfile.lpstrFile);		
		return filename;
	}	
	return L"";
}
/************************главная оконная процедура *******************/
INT_PTR CALLBACK DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{			
	wchar_t tmp[MAX_PATH]={0};
	param  tmpd={0};
	switch (uMsg)
	{	
	case WM_COMMAND:
		switch (wParam)
		{	
		case IDC_BUTTON1:	
			{							
				int n=0;
				ZeroMemory(&tmpd,sizeof(param));
				tmpd.maxsize=GetDlgItemInt(hwndDlg,IDC_EDIT2,NULL,false);
				GetDlgItemText(hwndDlg,IDC_EDIT3,tmp,MAX_PATH);																						
				n=SendDlgItemMessage(hwndDlg,IDC_COMBO1,CB_GETCOUNT,NULL,NULL);
				for (int i=0;i<=n;i++)
				{
					SendDlgItemMessage(hwndDlg,IDC_COMBO1,CB_GETLBTEXT,i,(LPARAM)tmpd.extlist[i]);
				}				
				GetDlgItemText(hwndDlg,IDC_EDIT4,tmp,MAX_PATH);				
				lstrcpy(tmpd.path,tmp);											
				UpdateSettings(&tmpd,sizeof(tmpd),binpath);								
			}
			return INT_PTR(TRUE);

		case IDC_BUTTON2:			
			{
				EndDialog(hwndDlg,wParam);
				ExitProcess(0);
			}			
			return INT_PTR(TRUE);	

		case IDC_BUTTON3:	
			{
				GetDlgItemText(hwndDlg,IDC_EDIT3,tmp,MAX_PATH);	
				SendDlgItemMessage(hwndDlg,IDC_COMBO1,CB_ADDSTRING,NULL,(LPARAM)tmp);
				SendDlgItemMessage(hwndDlg,IDC_COMBO1,CB_SETCURSEL,NULL,0);			
			}
			return INT_PTR(TRUE);

		case IDC_BUTTON4:
			{		
				int cur=SendDlgItemMessage(hwndDlg,IDC_COMBO1,CB_GETCURSEL,NULL,NULL);
				SendDlgItemMessage(hwndDlg,IDC_COMBO1,CB_DELETESTRING,cur,NULL);
				SendDlgItemMessage(hwndDlg,IDC_COMBO1,CB_SETCURSEL,NULL,0);
			}
			return INT_PTR(TRUE);	
		}  
	case WM_INITDIALOG:			
		{			
			if (lParam==IDD_DIALOG1)
			{
				lstrcpy(binpath,ShowOpenDialog(hwndDlg));				
				ZeroMemory(&tmpd,sizeof(param));
				if (ReadSitting(&tmpd,binpath)==true)
				{
					SetDlgItemInt	(hwndDlg,IDC_EDIT2,tmpd.maxsize,true);
					SetDlgItemText	(hwndDlg,IDC_EDIT4,tmpd.path);
					for (int i=0;lstrlen(tmpd.extlist[i])>0;i++)
					{						
						SendDlgItemMessage(hwndDlg,IDC_COMBO1,CB_ADDSTRING,NULL,(LPARAM)tmpd.extlist[i]);
					}
					SendDlgItemMessage(hwndDlg,IDC_COMBO1,CB_SETCURSEL,NULL,0);
					SetDlgItemText	(hwndDlg,IDC_EDIT3,tmp);					
				}
			}				
			return INT_PTR(TRUE);
		}
	case WM_CLOSE	: 
		{		
			EndDialog(hwndDlg,wParam); 
			ExitProcess(0);
		}
		return INT_PTR(TRUE);			
	}
	return 0;
}

int WINAPI myWinMain(void)
{	
	HINSTANCE hInstance=GetModuleHandle(NULL);
	DialogBoxParam(hInstance, MAKEINTRESOURCE	(IDD_DIALOG1), HWND_DESKTOP, (DLGPROC)DialogProc, IDD_DIALOG1);
	return 0;
}