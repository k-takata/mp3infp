#include "stdafx.h"

#pragma comment(lib,"Version.lib")

//欠けている可能性のある２バイト文字対応
void mbsncpy2(unsigned char *dst,unsigned char *src,int c)
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

BOOL GetDLLVersion(IN LPSTR szDLLFileName,
				   IN DWORD *pdwMajor,
				   IN DWORD *pdwMinor,
				   IN DWORD *pdwBuildNumber1,
				   IN DWORD *pdwBuildNumber2)
{
	*pdwMajor = 0;
	*pdwMinor = 0;
	*pdwBuildNumber1 = 0;
	*pdwBuildNumber2 = 0;
	char fileVersion[256];

	UINT len;
	DWORD dwHandle;
	DWORD dwSize = GetFileVersionInfoSize(szDLLFileName,&dwHandle);
	if(dwSize)
	{
		char *versionInfo = (char *)malloc(dwSize+1);
		if(GetFileVersionInfo(szDLLFileName,dwHandle,dwSize,versionInfo))
		{
			sprintf(fileVersion,"\\VarFileInfo\\Translation");
			void *version = NULL;
			BOOL bRet = VerQueryValue(versionInfo,fileVersion,&version,(UINT *)&len);
			if(bRet && len == 4)
			{
				DWORD dwLangD;
				memcpy(&dwLangD,version,4);
				sprintf(fileVersion,"\\StringFileInfo\\%02X%02X%02X%02X\\FileVersion",
					(dwLangD & 0xff00)>>8,dwLangD & 0xff,(dwLangD & 0xff000000)>>24,(dwLangD & 0xff0000)>>16);
			}
			else
			{
				sprintf(fileVersion,"\\StringFileInfo\\%04X04B0\\FileVersion",GetUserDefaultLangID());
			}
			bRet = VerQueryValue(versionInfo,fileVersion,&version,(UINT *)&len);
			if(!bRet)
			{
				free(versionInfo);
				return FALSE;
			}
			strncpy(fileVersion,(char *)version,255);
			fileVersion[255] = '\0';
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
	char *ptr = strtok(fileVersion,",. ");
	if(ptr == NULL)
		return TRUE;
	*pdwMajor = atoi(ptr);
	
	ptr = strtok(NULL,",. ");
	if(ptr == NULL)
		return TRUE;
	*pdwMinor = atoi(ptr);
	
	ptr = strtok(NULL,",. ");
	if(ptr == NULL)
		return TRUE;
	*pdwBuildNumber1 = atoi(ptr);
	
	ptr = strtok(NULL,",. ");
	if(ptr == NULL)
		return TRUE;
	*pdwBuildNumber2 = atoi(ptr);

	return TRUE;
}

//文末がYenのときTRUE
BOOL IsTailYenSign(char *szStr)
{
	unsigned char *yen = _mbsrchr((unsigned char *)szStr,'\\');
	if(yen && ((unsigned char *)&szStr[strlen(szStr)-1] == yen))
	{
		return TRUE;
	}
	return FALSE;
}

//文末に'\\'を追加(既に'\\'のときは何もしない)
void AddTAilYenSigne(char *szStr)
{
	if(!IsTailYenSign(szStr))
	{
		strcat(szStr,"\\");
	}
}

void AddTAilYenSigne(CString &str)
{
	if(!IsTailYenSign((char *)(LPCSTR )str))
	{
		str += "\\";
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
	char szTmp[256*2];	//UNICODEで256文字
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
//★ビッグエンディアン->リトルエンディアンの変換
DWORD ExtractI4(unsigned char buf[4])
{
	DWORD x;
	// big endian extract
	x = buf[0];
	x <<= 8;
	x |= buf[1];
	x <<= 8;
	x |= buf[2];
	x <<= 8;
	x |= buf[3];
	return x;
}

//---------------------------------------------------------------------
//★ビッグエンディアン->リトルエンディアンの変換
WORD ExtractI2(unsigned char buf[2])
{
	WORD x;
	// big endian extract
	x = buf[0];
	x <<= 8;
	x |= buf[1];
	return x;
}

const char *getFileName(const char *szPath)
{
	const char *szPtr = szPath;

	while(*szPtr != '\0')
	{
		//２バイト文字の先頭はスキップ
		if(IsDBCSLeadByte(*szPtr) == 0)
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

CString getFileNameExtName(CString &path)
{
	int i = 0;
	int pathOffset = 0;
	while(path.GetLength() > i)
	{
		//２バイト文字の先頭はスキップ
		if(IsDBCSLeadByte(path[i]) == 0)
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

const char *getExtName(const char *szPath)
{
	const char *szPtr = szPath;

	//ファイル名だけを分離
	szPtr=getFileName(szPath);
	//拡張子を含まないときは""へのポインタ
	szPath+=strlen(szPath);
	while(*szPtr != '\0')
	{
		//２バイト文字の先頭はスキップ
		if(IsDBCSLeadByte(*szPtr) == 0)
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

CString getExtName(CString &path)
{
	//ファイル名だけを分離
	CString fName = getFileName(path);

	//拡張子を含まないときは""へのポインタ
	int i = 0;
	int pathOffset = -1;
	while(fName.GetLength() > i)
	{
		//２バイト文字の先頭はスキップ
		if(IsDBCSLeadByte(fName[i]) == 0)
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
		return "";
	}
	else
	{
		return fName.Right(fName.GetLength() - pathOffset);
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

void sysError(HWND hWnd,char *mes)
{
	LPSTR lpBuffer;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
			NULL,
			GetLastError(),
			0,
			(LPTSTR )&lpBuffer,
			0,
			NULL );
	MessageBox(hWnd,lpBuffer,mes,MB_APPLMODAL | MB_ICONSTOP);
	LocalFree(lpBuffer);
	return;
}

void errMessageBox(HWND hWnd,DWORD dwErrorCode,char *mes)
{
	LPSTR lpBuffer;

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

void cutFileName(char *szPath)
{
	char	*szEnd=szPath;
	
	while(*szPath != '\0')
	{
		//２バイト文字の先頭はスキップ
		if(IsDBCSLeadByte(*szPath) == 0)
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

CString getPathName(CString &path)
{
	int i = 0;
	int pathOffset = 0;
	while(path.GetLength() > i)
	{
		//２バイト文字の先頭はスキップ
		if(IsDBCSLeadByte(path[i]) == 0)
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

void cutExtName(char *szFileName)
{
	char *pEnd = szFileName;
	char *pStart=szFileName;
	
	while(*pEnd != '\0')
	{
		if(IsDBCSLeadByte(*pEnd) != 0)
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
		if(IsDBCSLeadByte(*pStart) != 0)
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

CString getFileName(CString &path)
{
	CString fName = getFileNameExtName(path);
	
	int i = 0;
	int pathOffset = 0;
	while(fName.GetLength() > i)
	{
		if(IsDBCSLeadByte(fName[i]) == 0)
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

CString divString(char *src,char c,int n)
{
	CString ret;
	int cnt = (int )((n - strlen(src)%n)%n);
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
