#include "stdafx.h"
#include "GlobalCommand.h"

#pragma comment(lib,"Version.lib")

//欠けている可能性のある２バイト文字対応
void mbsncpy2(unsigned char *dst,const unsigned char *src,int c)
{
	for(int i=0; i<c; i++)
	{
		if(IsDBCSLeadByte(src[i]))
		{
			//欠けているリードバイトはコピーしない
			if((i+1) == c)
			{
				dst[i] = '\0';
				break;
			}
			dst[i] = src[i];
			i++;
		}
		dst[i] = src[i];
		if(src[i] == '\0')
		{
			break;
		}
	}
}

long check2ByteLength(const char *szTag,long lLimit)
{
	long i=0;
	while(szTag[i])
	{
		BOOL bIsLb = IsDBCSLeadByte(szTag[i]);
		if(bIsLb)
			i++;
		if(i >= lLimit)
		{
			if(bIsLb)
				return (i-1);
			return i;
		}
		i++;
	}
	return i;
}

BOOL GetDLLVersion(IN LPCTSTR szDLLFileName,
				   IN DWORD *pdwMajor,
				   IN DWORD *pdwMinor,
				   IN DWORD *pdwBuildNumber1,
				   IN DWORD *pdwBuildNumber2)
{
	*pdwMajor = 0;
	*pdwMinor = 0;
	*pdwBuildNumber1 = 0;
	*pdwBuildNumber2 = 0;
	TCHAR fileVersion[256];

	UINT len;
	DWORD dwHandle;
	DWORD dwSize = GetFileVersionInfoSize(szDLLFileName,&dwHandle);
	if(dwSize)
	{
		char *versionInfo = (char *)malloc(dwSize+sizeof(TCHAR));
		if(GetFileVersionInfo(szDLLFileName,dwHandle,dwSize,versionInfo))
		{
			wsprintf(fileVersion,_T("\\VarFileInfo\\Translation"));
			void *version = NULL;
			BOOL bRet = VerQueryValue(versionInfo,fileVersion,&version,(UINT *)&len);
			if(bRet && len == sizeof(DWORD))
			{
				DWORD dwLangD;
				memcpy(&dwLangD,version,sizeof(DWORD));
				wsprintf(fileVersion,_T("\\StringFileInfo\\%02X%02X%02X%02X\\FileVersion"),
					(dwLangD & 0xff00)>>8,dwLangD & 0xff,(dwLangD & 0xff000000)>>24,(dwLangD & 0xff0000)>>16);
			}
			else
			{
				wsprintf(fileVersion,_T("\\StringFileInfo\\%04X04B0\\FileVersion"),GetUserDefaultLangID());
			}
			bRet = VerQueryValue(versionInfo,fileVersion,&version,(UINT *)&len);
			if(!bRet)
			{
				free(versionInfo);
				return FALSE;
			}
			lstrcpyn(fileVersion,(LPCTSTR)version,sizeof_array(fileVersion));
		//	fileVersion[255] = '\0';
			free(versionInfo);
		}
		else
		{
			free(versionInfo);
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
	

	//バージョン情報を数字に分解
	LPTSTR ptr = _tcstok(fileVersion,_T(",. "));
	if(ptr == NULL)
		return TRUE;
	*pdwMajor = _ttoi(ptr);
	
	ptr = _tcstok(NULL,_T(",. "));
	if(ptr == NULL)
		return TRUE;
	*pdwMinor = _ttoi(ptr);
	
	ptr = _tcstok(NULL,_T(",. "));
	if(ptr == NULL)
		return TRUE;
	*pdwBuildNumber1 = _ttoi(ptr);
	
	ptr = _tcstok(NULL,_T(",. "));
	if(ptr == NULL)
		return TRUE;
	*pdwBuildNumber2 = _ttoi(ptr);

	return TRUE;
}

//文末がYenのときTRUE
BOOL IsTailYenSign(LPCTSTR szStr)
{
	LPCTSTR yen = _tcsrchr(szStr,_T('\\'));
	if(yen && (&szStr[lstrlen(szStr)-1] == yen))
	{
		return TRUE;
	}
	return FALSE;
}

//文末に'\\'を追加(既に'\\'のときは何もしない)
void AddTailYenSign(LPTSTR szStr)
{
	if(!IsTailYenSign(szStr))
	{
		lstrcat(szStr,_T("\\"));
	}
}

void AddTailYenSign(CString &str)
{
	if(!IsTailYenSign(str))
	{
		str += _T("\\");
	}
}

#ifdef _UNICODE
#define CHAR_FUDGE 1    // one TCHAR unused is good enough
#else
#define CHAR_FUDGE 2    // two BYTES unused for case of DBC last char
#endif

//LoadStringのバグ対策(MSKB Q140452)
CString LoadResString(HINSTANCE hInstance,UINT uID)
{
	TCHAR szTmp[256*2];	//UNICODEで256文字
	CString strRet;

	//必要なサイズを計算
	int iLength = LoadString(hInstance,uID,szTmp,256);
	if(256 - iLength > CHAR_FUDGE)
	{
		return szTmp;	//256で足りた場合
	}
	
	// try buffer size of 512, then larger size until entire string is retrieved
	int nSize = 256;
	do
	{
		nSize += 256;
		iLength = LoadString(hInstance,uID,strRet.GetBuffer(nSize*2-1),nSize);
	}while(nSize - iLength <= CHAR_FUDGE);
	strRet.ReleaseBuffer();

	return strRet;
}

//---------------------------------------------------------------------
LPCTSTR getFileNameExtName(LPCTSTR szPath)
{
	LPCTSTR szPtr = szPath;

	while(*szPtr != '\0')
	{
		//２バイト文字の先頭はスキップ
		if(IS_LEAD_TBYTE(*szPtr) == 0)
		{
			//[\],[/],[:]を見つけたら現在地+1のポインタを保存
			if((*szPtr == '\\') || (*szPtr == '/') || (*szPtr == ':'))
			{
				szPath=szPtr+1;
			}
		}
		//次の文字へ
		szPtr=CharNext(szPtr);
	}
	return szPath;
}

CString getFileNameExtName(const CString &path)
{
	int i = 0;
	int pathOffset = 0;
	while(path.GetLength() > i)
	{
		//２バイト文字の先頭はスキップ
		if(IS_LEAD_TBYTE(path[i]) == 0)
		{
			//[\],[/],[:]を見つけたら現在地+1のポインタを保存
			if((path[i] == '\\') || (path[i] == '/') || (path[i] == ':'))
			{
				pathOffset = i+1;
			}
			i++;
		}
		else
		{
			i += 2;
		}
	}

	return path.Right(path.GetLength() - pathOffset);
}

LPCTSTR getExtName(LPCTSTR szPath)
{
	LPCTSTR szPtr = szPath;

	//ファイル名だけを分離
	szPtr=getFileNameExtName(szPath);
	//拡張子を含まないときは""へのポインタ
	szPath+=lstrlen(szPath);
	while(*szPtr != '\0')
	{
		//２バイト文字の先頭はスキップ
		if(IS_LEAD_TBYTE(*szPtr) == 0)
		{
			//[.]を見つけたら現在地のポインタを保存
			if(*szPtr == '.')
			{
				szPath=szPtr;
			}
		}
		//次の文字へ
		szPtr=CharNext(szPtr);
	}
	return szPath;
}

CString getExtName(const CString &path)
{
	//ファイル名だけを分離
	CString fName = getFileNameExtName(path);

	//拡張子を含まないときは""へのポインタ
	int i = 0;
	int pathOffset = -1;
	while(fName.GetLength() > i)
	{
		//２バイト文字の先頭はスキップ
		if(IS_LEAD_TBYTE(fName[i]) == 0)
		{
			//[.]を見つけたら現在地のポインタを保存
			if(fName[i] == '.')
			{
				pathOffset = i;
			}
			i++;
		}
		else
		{
			i += 2;
		}
	}

	if(pathOffset == -1)
	{
		return _T("");
	}
	else
	{
		return fName.Right(fName.GetLength() - pathOffset);
	}
}

void sysError(HWND hWnd,LPCTSTR mes)
{
	errMessageBox(hWnd, GetLastError(), mes);
}

void errMessageBox(HWND hWnd,DWORD dwErrorCode,LPCTSTR mes)
{
	LPTSTR lpBuffer;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			dwErrorCode,
			0,
			(LPTSTR )&lpBuffer,
			0,
			NULL );
	MessageBox(hWnd,lpBuffer,mes,MB_APPLMODAL | MB_ICONSTOP);
	LocalFree(lpBuffer);
	return;
}

void cutFileName(LPTSTR szPath)
{
	LPTSTR szEnd=szPath;
	
	while(*szPath != '\0')
	{
		//２バイト文字の先頭はスキップ
		if(IS_LEAD_TBYTE(*szPath) == 0)
		{
			if((*szPath == '\\') || (*szPath == '/') || (*szPath == ':'))
			{
				szEnd=szPath+1;
			}
		}
		szPath=CharNext(szPath);
	}
	//パス名にファイルを含まなかった場合何もしない
	if(szEnd == szPath)
	{
		return;
	}
	//パス名からファイル名を切り離す
	*szEnd='\0';
	return;
}

CString getPathName(const CString &path)
{
	int i = 0;
	int pathOffset = 0;
	while(path.GetLength() > i)
	{
		//２バイト文字の先頭はスキップ
		if(IS_LEAD_TBYTE(path[i]) == 0)
		{
			if((path[i] == '\\') || (path[i] == '/') || (path[i] == ':'))
			{
				pathOffset = i + 1;
			}
			i++;
		}
		else
		{
			i += 2;
		}
	}

	return path.Left(pathOffset);
}

void cutExtName(LPTSTR szFileName)
{
	LPTSTR pEnd = szFileName;
	LPTSTR pStart=szFileName;
	
	while(*pEnd != '\0')
	{
		if(IS_LEAD_TBYTE(*pEnd) != 0)
		{
			pEnd++;
		}else
		{
			if((*pEnd == '\\') || (*pEnd == '/') || (*pEnd == ':'))
			{
				pStart=pEnd+1;
			}
		}
		pEnd++;
	}
	pEnd=szFileName;
	while(*pStart != '\0')
	{
		if(IS_LEAD_TBYTE(*pStart) != 0)
		{
			pStart++;
		}else
		{
			if(*pStart == '.')
			{
				pEnd=pStart;
			}
		}
		pStart++;
	}
	//ファイル名に拡張子を含まなかった場合何もしない
	if(pEnd == szFileName)
	{
		return;
	}
	//ファイル名から拡張子を切り離す
	*pEnd='\0';
}

CString getFileName(const CString &path)
{
	CString fName = getFileNameExtName(path);
	
	int i = 0;
	int pathOffset = 0;
	while(fName.GetLength() > i)
	{
		if(IS_LEAD_TBYTE(fName[i]) == 0)
		{
			if(fName[i] == '.')
			{
				pathOffset = i;
			}
			i++;
		}
		else
		{
			i += 2;
		}
	}

	return fName.Left(pathOffset);
}

CString divString(LPCTSTR src,char c,int n)
{
	CString ret;
	int cnt = (int )((n - lstrlen(src)%n)%n);
	while(*src)
	{
		ret += *src;
		src++;
		cnt++;
		if(*src && (cnt == n))
		{
			cnt = 0;
			ret += c;
		}
	}
	return ret;
}

CString DataToCString(const char *data, int size, DTC_CODE code)
{
	CString ret;
	
	switch (code) {
	case DTC_CODE_ANSI:
		return CString(data, size);
		
	case DTC_CODE_UTF16LE:
		return CString((LPCWSTR)data, size/sizeof(WCHAR));
		
	case DTC_CODE_UTF16BE:
		{
			LPWSTR wstr = (LPWSTR) malloc(size + sizeof(WCHAR));
			if (wstr != NULL) {
				int len = size / sizeof(WCHAR);
				wcsncpy(wstr, (LPCWSTR)data, len);
				wstr[len] = L'\0';
				// UTF-16BE -> UTF-16LE
				ConvertUTF16Endian(wstr, len);
				ret = wstr;		// UNICODE -> TCHAR
				free(wstr);
			}
		}
		break;
		
	case DTC_CODE_UTF8:
		{
			// Win95ではCP_UTF8が使えないが考慮しない
			int wsize = MultiByteToWideChar(CP_UTF8, 0, data, size, NULL, 0);
			LPWSTR wstr = (LPWSTR) malloc((wsize+1)*sizeof(WCHAR));
			if (wstr != NULL) {
				MultiByteToWideChar(CP_UTF8, 0, data, size, wstr, wsize);
				wstr[wsize] = L'\0';
				ret = wstr;		// UNICODE -> TCHAR
				free(wstr);
			}
		}
		break;
	}
	return ret;
}

int TstrToData(LPCTSTR tstr, int tlen, char *data, int dsize, DTC_CODE code)
{
	int ret_size = 0;
	
	if ((data == NULL) && (dsize != 0)) {
		return 0;	// error
	}
	if (dsize == 0) {
		data = NULL;
	}
	if (tlen == -1) {
		tlen = lstrlen(tstr) + 1;
	}
	
	switch (code) {
	case DTC_CODE_ANSI:
		{
#ifdef UNICODE
			ret_size = WideCharToMultiByte(CP_ACP, 0, tstr, tlen, data, dsize, NULL, NULL);
#else
			ret_size = tlen * sizeof(char);
			if (data != NULL) {
				if (dsize < ret_size) {
					return 0;
				}
				memcpy(data, tstr, ret_size);
			}
#endif
		}
		break;
		
	case DTC_CODE_UTF16LE:
	case DTC_CODE_UTF16BE:
		{
#ifdef UNICODE
			ret_size = tlen * sizeof(WCHAR);
			if (data != NULL) {
				if (dsize < ret_size) {
					return 0;
				}
				memcpy(data, tstr, ret_size);
			}
#else
			ret_size = MultiByteToWideChar(CP_ACP, 0, tstr, tlen, (LPWSTR)data, dsize/sizeof(WCHAR)) * sizeof(WCHAR);
#endif
			if ((data != NULL) && (code == DTC_CODE_UTF16BE)) {
				// UTF-16LE -> UTF-16BE
				ConvertUTF16Endian((LPWSTR)data, ret_size/sizeof(WCHAR));
			}
		}
		break;
		
	case DTC_CODE_UTF8:
		{
			LPWSTR buf;
			int buf_len;
#ifdef UNICODE
			buf = (LPWSTR)tstr;
			buf_len = tlen;
#else
			buf_len = MultiByteToWideChar(CP_ACP, 0, tstr, tlen, NULL, 0);
			buf = (LPWSTR)malloc(buf_len * sizeof(WCHAR));
			if (buf == NULL) {
				return 0;
			}
			MultiByteToWideChar(CP_ACP, 0, tstr, tlen, buf, buf_len);
#endif
			// Win95ではCP_UTF8が使えないが考慮しない
			ret_size = WideCharToMultiByte(CP_UTF8, 0, buf, buf_len, data, dsize, NULL, NULL);
#ifndef UNICODE
			free(buf);
#endif
		}
		break;
	}
	return ret_size;
}

char *TstrToDataAlloc(LPCTSTR tstr, int tlen, int *dsize, DTC_CODE code, int offset)
{
	int size = TstrToData(tstr, tlen, NULL, 0, code);
	if (size == 0) {
		return NULL;
	}
	char *buf = (char *)malloc(size + offset);
	if (buf == NULL) {
		return NULL;
	}
	if (dsize != NULL) {
		*dsize = size + offset;
	}
	TstrToData(tstr, tlen, buf + offset, size, code);
	return buf;
}

__int64 GetFileSize64(HANDLE hFile)
{
	LARGE_INTEGER liSize;
	liSize.LowPart = GetFileSize(hFile,(LPDWORD)&liSize.HighPart);
	if ((liSize.LowPart == (DWORD) -1) && (GetLastError() != NO_ERROR)) {
		return -1;
	}
	return liSize.QuadPart;
}


__int64 SetFilePointer64(HANDLE hFile,__int64 llDistanceToMove,DWORD dwMoveMethod)
{
	LARGE_INTEGER liDist;
	liDist.QuadPart = llDistanceToMove;
	
	liDist.LowPart = SetFilePointer(hFile,liDist.LowPart,&liDist.HighPart,dwMoveMethod);
	if ((liDist.LowPart == INVALID_SET_FILE_POINTER) && (GetLastError() != NO_ERROR)) {
		return -1;
	}
	return liDist.QuadPart;
}


BOOL CTimeStampSaver::Push(LPCTSTR szFile)
{
	BOOL ret = FALSE;

	//タイムスタンプを保存
	HANDLE hFile = CreateFile(
						szFile,
						GENERIC_READ,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		ret = GetFileTime(hFile,&m_createTime,NULL,&m_fileTime);
		CloseHandle(hFile);
	}
	m_bTimeStampPushed = ret;
	m_strFile = szFile;
	return ret;
}

BOOL CTimeStampSaver::Pop(LPCTSTR szFile)
{
	if(!m_bTimeStampPushed)
	{
		//直前のPushが失敗した場合はタイムスタンプを復元しない
		return FALSE;
	}
	if(szFile == NULL)
	{
		szFile = m_strFile;
	}
	m_bTimeStampPushed = FALSE;

	//タイムスタンプを復元
	HANDLE hFile = CreateFile(
						szFile,
						GENERIC_READ|GENERIC_WRITE,
						FILE_SHARE_READ|FILE_SHARE_WRITE,
						NULL,
						OPEN_EXISTING,	//ファイルをオープンします。指定ファイルが存在していない場合、関数は失敗します。
						FILE_ATTRIBUTE_NORMAL,
						NULL);
	if(hFile != INVALID_HANDLE_VALUE)
	{
		SetFileTime(hFile,&m_createTime,NULL,&m_fileTime);
		CloseHandle(hFile);
		return TRUE;
	}
	return FALSE;
}
