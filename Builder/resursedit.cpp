#include <Windows.h>
#include "resursedit.h"


bool ReadSitting(LPVOID data,wchar_t * Path)
{
	HRSRC hRes; 
	HGLOBAL hResLoad;   // handle to loaded resource
	LPVOID lpResLock;   // pointer to resource data
	HINSTANCE hHandle = LoadLibrary (Path);

	if (hHandle)
	{
		hRes = FindResource(hHandle, MAKEINTRESOURCE(IDC_SITDATA), RT_RCDATA);
		if (hRes == NULL)
		{
			MessageBox(0,L"Немогу найти ресурс RT_RCDATA",L"",MB_OK);		
			FreeLibrary(hHandle);
			return false;
		}

		hResLoad = LoadResource(hHandle, hRes);
		if (hResLoad == NULL)
		{
			MessageBox(0,L"Не могу загрузить ресурс",L"",MB_OK);			
			FreeLibrary(hHandle);
			return false;
			
		}		
		lpResLock = LockResource(hResLoad);
		if (lpResLock == NULL)
		{
			MessageBox(0,L"Немогу прочитать ресурс",L"",MB_OK);					
			FreeLibrary(hHandle);
			return false;
			
		}
		memcpy(data,lpResLock,SizeofResource(hHandle,hRes));				
		FreeLibrary(hHandle);
	}
	FreeLibrary(hHandle);
	return true;
}

int UpdateSettings(LPVOID data,int size,wchar_t *path)
{		
	HANDLE hUHandle = BeginUpdateResource (path, false);		
	if (hUHandle == NULL) 
	{
		MessageBox(0,L"Не могу открыть файл для записи.",L"",MB_OK);			
		return 0;
	}
	BOOL result = UpdateResource (hUHandle, 
					MAKEINTRESOURCE(RT_RCDATA), 
					MAKEINTRESOURCE (IDC_SITDATA), 
					MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), 
					data, size);		
	
	if (result == false)
	{
		MessageBox(0,L"Не могу добавить ресурс.",L"",MB_OK);
		return 0;
	}
	if (!EndUpdateResource(hUHandle, false)) 
	{		
		MessageBox(0,L"Не могу записать изменения в файл.",L"",MB_OK);
		return 0;
	}	
 	return 1;
}