// mp3infp_regist.cpp : アプリケーション用のエントリ ポイントの定義
//

#include "stdafx.h"
#include <stdlib.h>
#include <shellapi.h>
#include <mbstring.h>
#include "../RegAccess.h"

typedef DWORD (__cdecl *LPDLLREGISTERSERVER)(void);
typedef DWORD (__cdecl *LPDLLUNREGISTERSERVER)(void);

void regDll()
{
	// Systemディレクトリ取得
	char szSystemDirectory[MAX_PATH];
	GetSystemDirectory(szSystemDirectory,sizeof(szSystemDirectory));
	strcat(szSystemDirectory,"\\mp3infp.dll");

	//自己登録DLLを登録
	OutputDebugString(szSystemDirectory);
	HMODULE m_hMp3infpDll = LoadLibrary(szSystemDirectory);
	if(m_hMp3infpDll == NULL)
	{
		return;
	}
	//DllRegisterServerアドレス取得
	LPDLLREGISTERSERVER m_lpDllRegisterServer = (LPDLLREGISTERSERVER )GetProcAddress(m_hMp3infpDll,"DllRegisterServer");
	if(m_lpDllRegisterServer == NULL)
	{
		return;
	}
	//登録
	m_lpDllRegisterServer();
	FreeLibrary(m_hMp3infpDll);
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow )
{
	// mp3infpフォルダのmp3infp.dllを削除
//	DeleteFile("mp3infp.dll");
//	MessageBox(NULL,"mp3infp_regist.exe",NULL,0);

	regDll();
	// v2.32以前のmp3infp対策(未使用ファイルを削除する)
//	MessageBox(NULL,"test",NULL,0);

	//システムディレクトリを取得
	char szSystemPath[MAX_PATH];
	char szDeleteFile[MAX_PATH];
	GetSystemDirectory(szSystemPath,sizeof(szSystemPath));
	strcat(szSystemPath,"\\");
	
	//Systemフォルダのmp3infp.dllを削除
//	strcpy(szDeleteFile,szSystemPath);
//	strcat(szDeleteFile,"mp3infp.dll");
//	DeleteFile(szDeleteFile);
	//Systemフォルダのmp3infpr.dllを削除
	strcpy(szDeleteFile,szSystemPath);
	strcat(szDeleteFile,"mp3infpr.dll");
	DeleteFile(szDeleteFile);
#if 1	//旧バージョン対策
	//Systemフォルダのmp3infpw.dllを削除
	strcpy(szDeleteFile,szSystemPath);
	strcat(szDeleteFile,"mp3infpw.dll");
	DeleteFile(szDeleteFile);
#endif

	return 0;
}

