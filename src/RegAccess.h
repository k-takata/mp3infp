#ifndef _REGACCESS_H
#define _REGACCESS_H

BOOL regDelVal(HKEY keyRoot,LPTSTR szSubkey,LPTSTR szEntry);
BOOL regSetString(HKEY keyRoot,LPTSTR szExName,LPTSTR szEntry,LPCTSTR szData);
void regGetString(HKEY hRootKey, LPTSTR szKey,LPTSTR szVal, LPTSTR szRetVal,LPCTSTR szDefault);
#ifdef __AFX_H__
CString regGetStringEx(HKEY hRootKey, LPTSTR szKey,LPTSTR szVal,LPCTSTR szDefault);
#endif	//__AFX_H__
BOOL regSetDword(HKEY keyRoot,LPTSTR szExName,LPTSTR szEntry,DWORD dwData);
void regGetDword(HKEY hRootKey, LPTSTR szKey,LPTSTR szVal,DWORD *pdwRetVal,DWORD dwDefault);

#endif