#include <Windows.h>
#include "scanthread.h"


typedef void (*LPSEARCHFUNC)(wchar_t * lpszFileName,wchar_t * tPath);

/*************************************** Настройки ********************************************/
wchar_t   extlist[20][8]; // список расширений
wchar_t   path[MAX_PATH]; // папка для копирования файлов
int       maxsize;        // максимальный размер файла все что больше пропустить
/**********************************************************************************************/


/************************  Функция создания дерева каталогов***********************************/
void ForceDirectories(wchar_t * Path)
{
	wchar_t tmp[MAX_PATH];
	int pLen;
	lstrcpyW(tmp,Path);
	pLen=lstrlenW(tmp);

	for (int i=pLen;i>=0;i--)
	{
		if (tmp[i]=='\\')
		{
			tmp[i]='\0';
			ForceDirectories(tmp);
			break;
		}
	}
	CreateDirectory(tmp,NULL);	 
}

/******************* Функция получения серийного номера тома***********************************/
wchar_t * GetFlashVolumSerial(wchar_t * drive)
{
	DWORD serial;
	wchar_t tmp[MAX_PATH];	
	GetVolumeInformation(drive,NULL,NULL,&serial,NULL,NULL,NULL,NULL);
	wsprintf(tmp,L"%X",serial);
	return tmp;
}

/*********************Функция извлечения имени файла из его пути******************************/
wchar_t * ExtractFileName(wchar_t * lpszFileName)
{
	wchar_t tmp[MAX_PATH]; // временный массив
	wchar_t name[MAX_PATH];
	LPWSTR part;
	if(GetFullPathName(lpszFileName, MAX_PATH, tmp, &part) == 0) 
		return L"";
	lstrcpyW(name, part);	
	return name;
}

/******************************функция извелечения пути до файла*****************************/
wchar_t * ExtractFilePath(wchar_t * lpszFileName)
{
	wchar_t tmp[MAX_PATH]; // временный массив

	for (int i=lstrlenW(lpszFileName)-1;i>0;i--)
	{
		if (lpszFileName[i]!='\\')
		{
			lpszFileName[i]='\0';
		}
		else
		{
			lstrcpyW(tmp,lpszFileName);
			return tmp;
		}
	}
	return L"";
}


/*****************************фуникция обработки найденного файла*************/
void DoSomething(wchar_t * lpszFileName,wchar_t * tPath)
{
	wchar_t fName[MAX_PATH];
	wchar_t ftmp[MAX_PATH];

	wchar_t fPath[MAX_PATH];
	wchar_t fNewPath[MAX_PATH];

	lstrcpyW(ftmp,lpszFileName); // извлекаем имя файла	
	lstrcpyW(fName,ExtractFileName(ftmp)); // извлекаем имя файла	
	lstrcpyW(fPath,ExtractFilePath(ftmp)); // извлекаем путь 

	wsprintf(fNewPath,L"%s\\%s",tPath,&fPath[3]);
	ForceDirectories(fNewPath);
	lstrcatW(fNewPath,fName);
	CopyFile(lpszFileName,fNewPath,TRUE);
}

/***************************функция поиска*****************************************************/
BOOL SearchFiles(LPCWSTR lpszFileName, LPSEARCHFUNC lpSearchFunc,wchar_t * tPath)
{	
	LPWSTR part;
	wchar_t tmp[MAX_PATH]; // временный массив
	wchar_t name[MAX_PATH];

	HANDLE hSearch = NULL;
	WIN32_FIND_DATAW wfd;
	memset(&wfd, 0, sizeof(WIN32_FIND_DATAW));
	// сначало поиск внутри вложенных папках ...
	if(TRUE)
	{
		if(GetFullPathName(lpszFileName, MAX_PATH, tmp, &part) == 0) 
			return FALSE;
		lstrcpyW(name, part);
		lstrcpyW(part, L"*.*");

		// если папки существуют, то делаем поиск
		wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		if (!((hSearch = FindFirstFile(tmp, &wfd)) == INVALID_HANDLE_VALUE))
			do
			{
				// в каждой папке есть две папки с именами "." и ".."
				// и эти папки мы не трогаем
				// пропускаем папки "." и ".."
				if (!lstrcmpW(wfd.cFileName, L".") || !lstrcmpW(wfd.cFileName, L".."))            
					continue;

				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) // если мы нашли папку
				{
					wchar_t next[MAX_PATH];
					if(GetFullPathName(lpszFileName, MAX_PATH, next, &part) == 0) 
						return FALSE;
					lstrcpyW(part, wfd.cFileName);
					lstrcatW(next, L"\\");
					lstrcatW(next, name);
					SearchFiles(next, lpSearchFunc,tPath);
				}
			}
			while (FindNextFile(hSearch, &wfd)); // ищем следующий файл
			FindClose (hSearch); // заканчиваем поиск
	}
	if ((hSearch = FindFirstFile(lpszFileName, &wfd)) == INVALID_HANDLE_VALUE) 
		return TRUE; // в противном случае выходим
	do
	if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // если мы нашли файл
	{
		wchar_t file[MAX_PATH];
		if(GetFullPathName(lpszFileName, MAX_PATH, file, &part) == 0) 
			return FALSE;
		lstrcpyW(part, wfd.cFileName);

		if (wfd.nFileSizeLow < maxsize) 
		{
			lpSearchFunc(file,tPath);
		}
	}
	while (FindNextFile(hSearch, &wfd)); // ищем следующий файл
	FindClose (hSearch); // заканчиваем поиск
	return TRUE;
}

/***************************функция потока для поиска******************************************/
DWORD WINAPI scanthr(LPVOID lpThreadParameter)
{
	ThreadParam * data=(ThreadParam*) lpThreadParameter;	
	wchar_t buf[30];

	for (int i=0;lstrlen(extlist[i])>0;i++)
	{	
		wsprintf(buf,L"%s%s",data->drive,extlist[i]);
		SearchFiles(buf,DoSomething,data->topath);
	}
	
	return 0;
}
/***********************запуск сканирования****************************************************/
void startscan(wchar_t* drive)
{	
	DWORD ID;
	wchar_t temp[MAX_PATH];	
	ThreadParam * data;
	memset(data, 0, sizeof(ThreadParam));
	ZeroMemory(data,sizeof(ThreadParam));
	wsprintf(temp,L"%s\\%s",path,GetFlashVolumSerial(drive));
	lstrcat(data->drive,drive);		
	lstrcat(data->topath,temp);		
	CreateThread(NULL,0,scanthr,data,0,&ID);
}

void SetSittings(wchar_t* Path,LPVOID ExtL,int MaxS)
{
	maxsize=MaxS;
	lstrcpy(path,Path);
	memcpy(extlist,ExtL,sizeof(extlist));	
}