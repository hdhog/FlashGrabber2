#include <Windows.h>

struct params
{	
	int       maxsize;
	wchar_t   extlist[20][8];
	wchar_t   path[MAX_PATH];
};