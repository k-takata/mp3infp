// M3u.cpp: CM3u �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "M3u.h"
#include "GlobalCommand.h"

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

CM3u::CM3u()
{
	Release();

}

CM3u::~CM3u()
{

}

void CM3u::Release()
{
	m_strLines.RemoveAll();
}

DWORD CM3u::Load(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	Release();

	//�t�@�C�����I�[�v��
	HANDLE hFile = CreateFile(
				szFileName,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,	//�t�@�C�����I�[�v�����܂��B�w��t�@�C�������݂��Ă��Ȃ��ꍇ�A�֐��͎��s���܂��B
				FILE_ATTRIBUTE_NORMAL,
				NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}

	char buf[1024*64];
	DWORD ptr;
	DWORD dwReadSize;
	CString str = _T("");
	while(ReadFile(hFile,buf,sizeof(buf),&dwReadSize,NULL) && (dwReadSize != 0))
	{
		ptr = 0;
		while(ptr < dwReadSize)
		{
			if((buf[ptr] == '\r') || (buf[ptr] == '\n'))
			{
				if((ptr > 0) && (buf[ptr-1] == '\r') && (buf[ptr] == '\n'))
				{
				}
				else
				{
					m_strLines.Add(str);	//1�s�ǉ�
					str = _T("");
				}
				ptr++;
				continue;
			}
			else
			{
				str += buf[ptr];
			}
			ptr++;
		}
	}
	CloseHandle(hFile);
	if(str.GetLength())
	{
		m_strLines.Add(str);
	}

	return dwWin32errorCode;
}

DWORD CM3u::Save(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;

	//�t�@�C�����I�[�v��
	HANDLE hFile = CreateFile(
							szFileName,
							GENERIC_READ|GENERIC_WRITE,
							FILE_SHARE_READ,
							NULL,
							CREATE_ALWAYS,	//�V�����t�@�C�����쐬���܂��B�w�肵���t�@�C�������ɑ��݂��Ă���ꍇ�A���̃t�@�C�����㏑�����A�����̑������������܂��B
							FILE_ATTRIBUTE_NORMAL,
							NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}

	//���X�g����̎���0�o�C�g�̃t�@�C�������
	if(m_strLines.GetUpperBound() == -1)
	{
		CloseHandle(hFile);
		return dwWin32errorCode;
	}

	CString str;
	DWORD dwWritten;
	for(int i=0; i<=m_strLines.GetUpperBound(); i++)
	{
		str = m_strLines.GetAt(i);
		str += _T("\r\n");
		int size;
		char *buf = TstrToDataAlloc(str, str.GetLength(), &size, DTC_CODE_ANSI);
		if (buf != NULL) {
			int ret = WriteFile(hFile,buf,size,&dwWritten,NULL);
			free(buf);
			if (!ret) {
				dwWin32errorCode = GetLastError();
				break;
			}
		}
	}
	CloseHandle(hFile);
	
	return dwWin32errorCode;
}

BOOL CM3u::Add(LPCTSTR szLine)
{
	m_strLines.Add(CString(szLine));

	return TRUE;
}

BOOL CM3u::GetLine(int iLine,CString &str)
{
	if((m_strLines.GetUpperBound() == -1) ||
		(m_strLines.GetUpperBound() < iLine) )
	{
		return FALSE;
	}
	str = m_strLines.GetAt(iLine);

	return TRUE;
}