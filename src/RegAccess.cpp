#include "StdAfx.h"

//値の削除
BOOL regDelVal(HKEY keyRoot,char* szSubkey,char* szEntry)
{
	HKEY	hKey=NULL;
	BOOL	bReturn=FALSE;
	
	if(RegOpenKeyEx(keyRoot,szSubkey,0,KEY_WRITE|KEY_READ,&hKey) == ERROR_SUCCESS)
	{
		if(RegDeleteValue(hKey,szEntry) == ERROR_SUCCESS)
		{
			bReturn=TRUE;
		}
		RegCloseKey(hKey);
	}
	return bReturn;
}

//エントリの追加
BOOL regSetString(HKEY keyRoot,char *szExName,char *szEntry,const char *szData)
{
	HKEY		hKeyResult=0;
	DWORD		dwDisposition=0;
	LONG		lResult;
//	char		szRetVal[255];

	if(RegCreateKeyEx(keyRoot,
		szExName,
		0,
		"",
		REG_OPTION_NON_VOLATILE,
		KEY_WRITE|KEY_READ,
		NULL,
		&hKeyResult,
		&dwDisposition) == ERROR_SUCCESS)
	{
		lResult=RegSetValueEx(hKeyResult,
					szEntry,
					0,
					REG_SZ,
					(const unsigned char *)szData,
					lstrlen(szData)+1);
		RegCloseKey(hKeyResult);
		return TRUE;
	}
	return FALSE;
}

void regGetString(HKEY hRootKey, char *szKey,char *szVal, char *szRetVal,char *szDefault)
{
	HKEY	hKey=NULL;
	DWORD	dwSize=255;
	DWORD	dwType;

	strcpy(szRetVal,szDefault);
	if(RegOpenKeyEx(hRootKey,szKey,0,KEY_READ,&hKey) == ERROR_SUCCESS)
	{
		dwSize = 255;
		if(RegQueryValueEx(hKey,szVal,NULL,&dwType,(LPBYTE)szRetVal,&dwSize) == ERROR_SUCCESS)
		{
			if(dwSize == 0)
			{
				*szRetVal = 0;
			}
		}
		RegCloseKey(hKey);
	}
}

#ifdef __AFX_H__
CString regGetStringEx(HKEY hRootKey, char *szKey,char *szVal,char *szDefault)
{
	HKEY	hKey=NULL;
	CString strRet = szDefault;
	DWORD	dwSize=255;
	DWORD	dwType;

	if(RegOpenKeyEx(hRootKey,szKey,0,KEY_READ,&hKey) == ERROR_SUCCESS)
	{
		dwSize = 0;
		//サイズを取得
		if(RegQueryValueEx(hKey,szVal,NULL,&dwType,(LPBYTE)NULL,&dwSize) == ERROR_SUCCESS)
		{
			//実際に文字列を取得
			if(RegQueryValueEx(hKey,szVal,NULL,&dwType,(LPBYTE)strRet.GetBuffer(dwSize-1),&dwSize) == ERROR_SUCCESS)
			{
				if(dwSize == 0)
				{
					strRet = "";
				}
			}
			strRet.ReleaseBuffer();
		}
		RegCloseKey(hKey);
	}
	return strRet;
}
#endif //__AFX_H__

BOOL regSetDword(HKEY hRootKey,char *szSubkey,char *szEntry,DWORD dwVal)
{
	HKEY	hKey=NULL;
	DWORD	dwDisposition=0;
	BOOL	bReturn=FALSE;
	
	if(RegCreateKeyEx(hRootKey,
			szSubkey,
			0,
			"",
			REG_OPTION_NON_VOLATILE,
			KEY_WRITE|KEY_READ,
			NULL,
			&hKey,
			&dwDisposition) == ERROR_SUCCESS)
	{
		if(RegSetValueEx(hKey,szEntry,0,REG_DWORD,(CONST BYTE*)&dwVal,4) == 0)
		{
			bReturn=TRUE;
		}
		RegCloseKey(hKey);
	}
	return bReturn;
}


void regGetDword(HKEY hRootKey,char* szSubkey,char* szEntry,DWORD* pdwVal,DWORD dwDefault)
{
	HKEY	hKey=NULL;
	DWORD	dwSize=0;
	DWORD	dwType;
	*pdwVal = dwDefault;
	
	if(RegOpenKeyEx(HKEY_CURRENT_USER,szSubkey,0,KEY_READ,&hKey) == ERROR_SUCCESS)
	{
		dwSize = 4;
		if(RegQueryValueEx(hKey,szEntry,NULL,&dwType,(LPBYTE)pdwVal,&dwSize) == ERROR_SUCCESS)
		{
		}
		RegCloseKey(hKey);
	}
}

