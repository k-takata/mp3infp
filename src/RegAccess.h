#ifndef _REGACCESS_H
#define _REGACCESS_H

BOOL regDelVal(HKEY keyRoot,LPCTSTR szSubkey,LPCTSTR szEntry);
BOOL regSetString(HKEY keyRoot,LPCTSTR szExName,LPCTSTR szEntry,LPCTSTR szData);
void regGetString(HKEY hRootKey, LPCTSTR szKey,LPCTSTR szVal, LPTSTR szRetVal,LPCTSTR szDefault);
#ifdef __AFX_H__
CString regGetStringEx(HKEY hRootKey, LPCTSTR szKey,LPCTSTR szVal,LPCTSTR szDefault);
#endif	//__AFX_H__
BOOL regSetDword(HKEY keyRoot,LPCTSTR szExName,LPCTSTR szEntry,DWORD dwData);
void regGetDword(HKEY hRootKey, LPCTSTR szKey,LPCTSTR szVal,DWORD *pdwRetVal,DWORD dwDefault);

#endif
