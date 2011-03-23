// w2kstub.cpp
//
// WDK 6001.18002 で、Win2k/XP 用にビルドするためのスタブ


#include "stdafx.h"
#include <sys/stat.h>
#include <io.h>

#ifndef _WIN64
void __declspec(noreturn) AFXAPI AfxThrowInvalidArgException()
{
	THROW(0);
}
#endif

#if 0	// _fstati64 can be used on Win2k
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
#if 0	// #ifdef _WIN64
	return _fseeki64(stream, offset, whence);
#else
	int fd = _fileno(stream);
	
	// TODO: stream should be locked
	
	if (whence == SEEK_CUR) {
		// convert to offset from beginning of file
		offset += _telli64(fd);
		whence = SEEK_SET;
	}
	
	// update some internal information
	fseek(stream, 0, SEEK_CUR);
	
	// do seek
	return (_lseeki64(fd, offset, whence) == -1i64 ? -1 : 0);
#endif
}
