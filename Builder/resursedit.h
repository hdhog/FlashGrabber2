/*************************************** Настройки ********************************************
static const wchar_t * extlist[]	 =	{L"*.doc",L"*.txt"}; // список расширений
static const int	   counts =2;
static const wchar_t   path[255]	 =	L"C:\\temp\\saves";  // папка для копирования файлов
static const int       maxsize 		 =	1000000;			 // максимальный размер файла все что больше пропустить
/**********************************************************************************************/
#define IDC_SITDATA  1111
struct param
{	
	int       maxsize;
	wchar_t   extlist[20][8];
	wchar_t   path[MAX_PATH];
};
bool ReadSitting(LPVOID ,wchar_t *);
int UpdateSettings(LPVOID,int ,wchar_t *);