#ifndef _REGACCESS_H
#define _REGACCESS_H

BOOL regDelVal(HKEY keyRoot,char* szSubkey,char* szEntry);
BOOL regSetString(HKEY keyRoot,char *szExName,char *szEntry,const char *szData);
void regGetString(HKEY hRootKey, char *szKey,char *szVal, char *szRetVal,char *szDefault);
#ifdef __AFX_H__
CString regGetStringEx(HKEY hRootKey, char *szKey,char *szVal,char *szDefault);
#endif	//__AFX_H__
BOOL regSetDword(HKEY keyRoot,char *szExName,char *szEntry,DWORD dwData);
void regGetDword(HKEY hRootKey, char *szKey,char *szVal,DWORD *pdwRetVal,DWORD dwDefault);

#endif