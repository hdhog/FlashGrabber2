#include <windows.h>
#include "debugconsole.h"


BOOL CreateConsole(void)
{
	FreeConsole();        //на всякий случай
	if ( AllocConsole() )
	{
		int hCrt = _open_osfhandle((long)GetStdHandle(STD_OUTPUT_HANDLE), _O_TEXT);
		*stdout = *(::_fdopen(hCrt, "w"));
		::setvbuf(stdout, NULL, _IONBF, 0);
		*stderr = *(::_fdopen(hCrt, "w"));
		::setvbuf(stderr, NULL, _IONBF, 0);
		return TRUE;
	}return FALSE;
}
