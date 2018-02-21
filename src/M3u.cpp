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
	m_encoding = ENC_ANSI;

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

	if (lstrcmpi(getExtName(szFileName), _T(".m3u8")) == 0) {
		if ((m_encoding != ENC_UTF8N) && (m_encoding != ENC_UTF8B)) {
			m_encoding = ENC_UTF8N;
		}
	}

	FILE *fp = _tfopen(szFileName, _T("r"));
	if (fp == NULL) {
		return ERROR_INVALID_FUNCTION;	// QQQ �����G���[��Ԃ��Ă���
	}
	BTC_CODE input_code = BTC_CODE_ANSI;
	BOOL firstline = TRUE;
	char buf[1024*64];
	char *ptr, *eptr;
	while (fgets(buf, sizeof(buf), fp)) {
		ptr = buf;
		if (firstline) {
			if (memcmp(buf, "\xEF\xBB\xBF", 3) == 0) {	// UTF-8 BOM
				m_encoding = ENC_UTF8B;
				ptr += 3;
			}
			if ((m_encoding == ENC_UTF8N) || (m_encoding == ENC_UTF8B)) {
				input_code = BTC_CODE_UTF8;
			}
			firstline = FALSE;
		}
		eptr = ptr + strlen(ptr);
		if ((eptr > ptr) && (eptr[-1] == '\n')) {
			--eptr;
		}
		m_strLines.Add(BytesToCString(ptr, (int)(eptr - ptr), input_code));	//1�s�ǉ�
	}
	fclose(fp);

	return dwWin32errorCode;
}

DWORD CM3u::Save(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;

	if (lstrcmpi(getExtName(szFileName), _T(".m3u8")) == 0) {
		if ((m_encoding != ENC_UTF8N) && (m_encoding != ENC_UTF8B)) {
			m_encoding = ENC_UTF8N;
		}
	}
	BTC_CODE output_code = BTC_CODE_ANSI;
	if ((m_encoding == ENC_UTF8N) || (m_encoding == ENC_UTF8B)) {
		output_code = BTC_CODE_UTF8;
	}

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

	DWORD dwWritten;
	if (m_encoding == ENC_UTF8B) {
		WriteFile(hFile, "\xEF\xBB\xBF", 3, &dwWritten, NULL);
	}

	//���X�g����̎���0�o�C�g�̃t�@�C�������
	if(m_strLines.GetUpperBound() == -1)
	{
		CloseHandle(hFile);
		return dwWin32errorCode;
	}

	CString str;
	for(int i=0; i<=m_strLines.GetUpperBound(); i++)
	{
		str = m_strLines.GetAt(i);
		str += _T("\r\n");
		int size;
		char *buf = TstrToBytesAlloc(str, str.GetLength(), &size, output_code);
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
