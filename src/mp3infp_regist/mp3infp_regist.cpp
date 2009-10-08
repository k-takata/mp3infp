// mp3infp_regist.cpp : �A�v���P�[�V�����p�̃G���g�� �|�C���g�̒�`
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
	// System�f�B���N�g���擾
	char szSystemDirectory[MAX_PATH];
	GetSystemDirectory(szSystemDirectory,sizeof(szSystemDirectory));
	strcat(szSystemDirectory,"\\mp3infp.dll");

	//���ȓo�^DLL��o�^
	OutputDebugString(szSystemDirectory);
	HMODULE m_hMp3infpDll = LoadLibrary(szSystemDirectory);
	if(m_hMp3infpDll == NULL)
	{
		return;
	}
	//DllRegisterServer�A�h���X�擾
	LPDLLREGISTERSERVER m_lpDllRegisterServer = (LPDLLREGISTERSERVER )GetProcAddress(m_hMp3infpDll,"DllRegisterServer");
	if(m_lpDllRegisterServer == NULL)
	{
		return;
	}
	//�o�^
	m_lpDllRegisterServer();
	FreeLibrary(m_hMp3infpDll);
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow )
{
	// mp3infp�t�H���_��mp3infp.dll���폜
//	DeleteFile("mp3infp.dll");
//	MessageBox(NULL,"mp3infp_regist.exe",NULL,0);

	regDll();
	// v2.32�ȑO��mp3infp�΍�(���g�p�t�@�C�����폜����)
//	MessageBox(NULL,"test",NULL,0);

	//�V�X�e���f�B���N�g�����擾
	char szSystemPath[MAX_PATH];
	char szDeleteFile[MAX_PATH];
	GetSystemDirectory(szSystemPath,sizeof(szSystemPath));
	strcat(szSystemPath,"\\");
	
	//System�t�H���_��mp3infp.dll���폜
//	strcpy(szDeleteFile,szSystemPath);
//	strcat(szDeleteFile,"mp3infp.dll");
//	DeleteFile(szDeleteFile);
	//System�t�H���_��mp3infpr.dll���폜
	strcpy(szDeleteFile,szSystemPath);
	strcat(szDeleteFile,"mp3infpr.dll");
	DeleteFile(szDeleteFile);
#if 1	//���o�[�W�����΍�
	//System�t�H���_��mp3infpw.dll���폜
	strcpy(szDeleteFile,szSystemPath);
	strcat(szDeleteFile,"mp3infpw.dll");
	DeleteFile(szDeleteFile);
#endif

	return 0;
}

