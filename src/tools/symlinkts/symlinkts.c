/*
 * symlinkts - Set the timestamp of symlink as same as its target.
 *
 * Note: Administrator privileges are required.
 */

#define UNICODE
#define _UNICODE
#include <windows.h>
#include <tchar.h>
#include <stdio.h>

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES	((DWORD)-1)
#endif

int set_symlink_timestamp(LPCTSTR filename)
{
	HANDLE hFile;
	FILETIME ftModified;
	DWORD dwAttr;
	BOOL ret;

	/* Check if the filename is a reparse point. */
	dwAttr = GetFileAttributes(filename);
	if (dwAttr == INVALID_FILE_ATTRIBUTES) {
		_tprintf(_T("Can't get the attributes of \"%s\".\n"), filename);
		return -1;
	} else if ((dwAttr & FILE_ATTRIBUTE_REPARSE_POINT) == 0) {
		_tprintf(_T("Not a symlink: \"%s\".\n"), filename);
		return -1;
	}

	/* Get the timestamp of the symlink target. */
	hFile = CreateFile(filename, FILE_READ_ATTRIBUTES,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING,
			0, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		_tprintf(_T("Can't open the link target of \"%s\".\n"), filename);
		return -1;
	}
	ret = GetFileTime(hFile, NULL, NULL, &ftModified);
	CloseHandle(hFile);
	if (!ret) {
		_tprintf(_T("Can't get the timestamp.\n"));
		return -1;
	}

	/* Set the timestamp of the symlink itself. */
	hFile = CreateFile(filename, FILE_WRITE_ATTRIBUTES,
			0, NULL, OPEN_EXISTING,
			FILE_FLAG_OPEN_REPARSE_POINT, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		_tprintf(_T("Can't open the link \"%s\" (error: %d).\n"), filename,
				GetLastError());
		return -1;
	}
	ret = SetFileTime(hFile, NULL, NULL, &ftModified);
	if (!ret) {
		_tprintf(_T("Can't set the timestamp.\n"));
	}
	CloseHandle(hFile);
	return (ret) ? 0 : -1;
}

int __cdecl _tmain(int argc, TCHAR **argv)
{
	int i;

	for (i = 1; i < argc; i++) {
		set_symlink_timestamp(argv[i]);
	}
	return 0;
}
