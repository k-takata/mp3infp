// w2kstub.cpp
//
// WDK 6001.18002 で、Win2k 用にビルドするためのスタブ


#include "stdafx.h"
#include <sys/stat.h>
#include <io.h>

#ifndef _WIN64
void __declspec(noreturn) AFXAPI AfxThrowInvalidArgException()
{
	THROW(0);
}
#endif

#if 0
extern "C"
int __cdecl _fstati64_w2k(int fd, struct _stati64 *buffer)
{
#ifdef _WIN64
	return _fstati64(fd, buffer);
#else
	HANDLE hFile;
	struct _stat buf;
	int ret;
	
	if (buffer == NULL) {
		return -1;
	}
	ret = _fstat(fd, &buf);
	if (ret == 0) {
		buffer->st_dev = buf.st_dev;
		buffer->st_ino = buf.st_ino;
		buffer->st_mode = buf.st_mode;
		buffer->st_nlink = buf.st_nlink;
		buffer->st_uid = buf.st_uid;
		buffer->st_gid = buf.st_gid;
		buffer->st_rdev = buf.st_rdev;
		buffer->st_size = buf.st_size;
		buffer->st_atime = buf.st_atime;
		buffer->st_mtime = buf.st_mtime;
		buffer->st_ctime = buf.st_ctime;
		
		hFile = (HANDLE) _get_osfhandle(fd);
		if (hFile != INVALID_HANDLE_VALUE) {
			BY_HANDLE_FILE_INFORMATION bhfi;
			if (GetFileInformationByHandle(hFile, &bhfi) ) {
				buffer->st_size = ((__int64)(bhfi.nFileSizeHigh)) * (0x100000000i64) +
						(__int64)(bhfi.nFileSizeLow);
			}
		}
	}
	return ret;
#endif
}
#endif

extern "C"
int __cdecl _fseeki64_w2k(FILE *stream, __int64 offset, int whence)
{
#ifdef _WIN64
	return _fseeki64(stream, offset, whence);
#else
	HANDLE hFile;
	LARGE_INTEGER li;
	int ret;
	
	ret = fseek(stream, 0, SEEK_CUR);
	if (ret != 0) {
		return ret;
	}
	hFile = (HANDLE) _get_osfhandle(_fileno(stream));
	if (hFile == INVALID_HANDLE_VALUE) {
		return -1;
	}
	li.QuadPart = offset;
	li.LowPart = SetFilePointer(hFile, li.LowPart, &li.HighPart, whence);
	if ((li.LowPart == INVALID_SET_FILE_POINTER) && (GetLastError() != NO_ERROR)) {
		return -1;
	}
	return 0;
#endif
}
