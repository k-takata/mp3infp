// io_utf8.cpp
//
// mpeg4ip で Unicode ファイル名を扱えるようにするためのラッパー関数


#include "stdafx.h"
#include "GlobalCommand.h"
#include <stdarg.h>
#include <sys/stat.h>
#include <io.h>


extern "C"
FILE *__cdecl fopen_utf8(const char *file, const char *mode)
{
	CString strPath = DataToCString(file, -1, DTC_CODE_UTF8);
	return _tfopen(strPath, CString(mode));
}

extern "C"
int __cdecl _open_utf8(const char *path, int oflag, ...)
{
	va_list ap;
	int pmode = 0;
	CString strPath = DataToCString(path, -1, DTC_CODE_UTF8);
	
	va_start(ap, oflag);
	pmode = va_arg(ap, int);
	va_end(ap);
	
	return _topen(strPath, oflag, pmode);
}
