// Vqf.cpp: CVqf �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"		// ���C�� �V���{��
#include "GlobalCommand.h"
#include "Vqf.h"


//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

CVqf::CVqf()
{
	Release();
}

CVqf::~CVqf()
{

}

void CVqf::Release()
{
	m_bEnable = FALSE;
	m_fields.clear();
	m_dwStreamSize = 0;
	m_dwStereo = 0;
	m_dwCompRate = 0;
	m_dwSamplingFrequency = 0;
}

BOOL CVqf::SetField(char id1,char id2,char id3,char id4,const unsigned char *szData,DWORD dwSize)
{
	DWORD id = MakeKey(id1,id2,id3,id4);
	m_fields.erase(id);

	if(dwSize == 0)
	{
		return TRUE;
	}

	//map�ɒǉ�
	m_fields.insert(pair<DWORD,CVqfTag>(id,CVqfTag()));
	map<DWORD,CVqfTag>::iterator p = m_fields.find(id);
	if(p != m_fields.end())
	{
		p->second.SetData(szData,dwSize);
		unsigned char *data = p->second.GetData();
		if(!data)
		{
			//��̃t�B�[���h�͍��Ȃ�
			m_fields.erase(id);
		}
	}
	else
	{
		return FALSE;
	}
	return TRUE;
}

BOOL CVqf::SetField(char id1,char id2,char id3,char id4,LPCTSTR szStr)
{
	int size;
	char *buf = TstrToDataAlloc(szStr, lstrlen(szStr), &size, DTC_CODE_ANSI);
	if (buf == NULL) {
		return FALSE;
	}
	BOOL ret = SetField(id1, id2, id3, id4, (unsigned char *)buf, size);
	free(buf);
	return ret;
}

unsigned char *CVqf::GetField(char id1,char id2,char id3,char id4,DWORD *pdwSize)
{
	map<DWORD,CVqfTag>::iterator p = m_fields.find(MakeKey(id1,id2,id3,id4));
	if(p == m_fields.end())
	{
		return NULL;
	}
	*pdwSize = p->second.GetSize();
	return p->second.GetData();
}

CString CVqf::GetField(char id1,char id2,char id3,char id4)
{
	DWORD dwSize;
	unsigned char *data = GetField(id1, id2, id3, id4, &dwSize);
	if (data == NULL) {
		return _T("");
	}
	return CString((LPSTR)data, dwSize);
}

DWORD CVqf::GetTotalFieldSize()
{
	DWORD dwSize = 0;
	map<DWORD,CVqfTag>::iterator p;

	p = m_fields.begin();
	while(p != m_fields.end())
	{
		DWORD len = p->second.GetSize();
		dwSize += len + 4 + 4;/* + (len&0x1)?1:0;	//WORD���E���킹*/
		
		p++;
	}
	return dwSize;
}

DWORD CVqf::Load(LPCTSTR szFileName)
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

	DWORD dwRet;
	char szTmp[8];
	//VQF�w�b�_��ǂ݂Ƃ�
	char head[12];
	if(!ReadFile(hFile,&head,sizeof(head),&dwRet,NULL) || (dwRet != sizeof(head)))
	{
		CloseHandle(hFile);
		return -1;
	}
	
	//TWIN VQ�t�@�C���ł��邱�Ƃ��m�F
	if(strncmp(head,"TWIN97012000",sizeof(head)) != 0)
	{
		if(strncmp(head,"TWIN00052200",sizeof(head)) != 0)
		{
			CloseHandle(hFile);
			return -1;
		}
		else
		{
			m_strVer = _T("v2+");
		}
	}
	else
	{
		m_strVer = _T("v2");
	}
	
	m_bEnable = TRUE;

	//�w�b�_�T�C�Y���擾
	if(!ReadFile(hFile,&szTmp,4,&dwRet,NULL) || (dwRet != 4))
	{
		CloseHandle(hFile);
		return -1;
	}
	long lHeadSize = ((unsigned char )szTmp[0] << 24) | ((unsigned char )szTmp[1] << 16) |
						((unsigned char )szTmp[2] << 8) | (unsigned char )szTmp[3];

	char *HeadBuf = (char *)malloc(lHeadSize);
	if(!HeadBuf)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	m_dwStreamSize = GetFileSize(hFile,NULL);
	m_dwStreamSize -= lHeadSize + 12;


	//�w�b�_����荞��
	if(!ReadFile(hFile,HeadBuf,lHeadSize,&dwRet,NULL) || (dwRet != lHeadSize))
	{
		CloseHandle(hFile);
		return -1;
	}
	CloseHandle(hFile);

	long lRemainSize = lHeadSize;
	while(lRemainSize > 0)
	{
		if(8 > lRemainSize)
		{
			break;	//�p�S
		}
		char id1 = HeadBuf[lHeadSize-lRemainSize];
		char id2 = HeadBuf[lHeadSize-lRemainSize+1];
		char id3 = HeadBuf[lHeadSize-lRemainSize+2];
		char id4 = HeadBuf[lHeadSize-lRemainSize+3];
		lRemainSize -= 4;
		long lSize = ((unsigned char )HeadBuf[lHeadSize-lRemainSize+0] << 24) |
					((unsigned char )HeadBuf[lHeadSize-lRemainSize+1] << 16) |
					((unsigned char )HeadBuf[lHeadSize-lRemainSize+2] << 8) |
					(unsigned char )HeadBuf[lHeadSize-lRemainSize+3];
		lRemainSize -= 4;
		if(lSize > lRemainSize)
		{
			break;	//�p�S
		}

		//map�ɒǉ�
		SetField(id1,id2,id3,id4,(const unsigned char *)&HeadBuf[lHeadSize-lRemainSize],lSize);

		lRemainSize -= lSize;
	}
	free(HeadBuf);

	//�T���v�����O���[�g�Ȃǂ̏����擾����
	map<DWORD,CVqfTag>::iterator p = m_fields.find(MakeKey('C','O','M','M'));
	if(p != m_fields.end())
	{
		unsigned char *data = p->second.GetData();
		DWORD size = p->second.GetSize();
		if(size >= 12)
		{
			m_dwStereo =	((DWORD )data[0] << 24) |
							((DWORD )data[1] << 16) |
							((DWORD )data[2] << 8) |
							(DWORD )data[3];
			m_dwCompRate =	((DWORD )data[4] << 24) |
							((DWORD )data[5] << 16) |
							((DWORD )data[6] << 8) |
							(DWORD )data[7];
//			m_dwCompRate /= m_dwStereo + 1;
			m_dwSamplingFrequency = ((DWORD )data[8] << 24) |
									((DWORD )data[9] << 16) |
									((DWORD )data[10] << 8) |
									(DWORD )data[11];
		}
	}
	p = m_fields.find(MakeKey('D','S','I','Z'));
	if(p != m_fields.end())
	{
		unsigned char *data = p->second.GetData();
		DWORD size = p->second.GetSize();
		if(size >= 4)
		{
			m_dwStreamSize =((DWORD )data[0] << 24) |
							((DWORD )data[1] << 16) |
							((DWORD )data[2] << 8) |
							(DWORD )data[3];
		}
	}

	return dwWin32errorCode;
}

DWORD CVqf::Save(HWND hWnd,LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	if(!m_bEnable)
	{
		return FALSE;
	}

	DWORD dwTotalFrameSize = GetTotalFieldSize();

	//==================���t�@�C�����������ɕۑ�==================
	HANDLE hFile = CreateFile(
				szFileName,
				GENERIC_READ,
				FILE_SHARE_READ,
				NULL,
				OPEN_EXISTING,	//�w��t�@�C�������݂��Ă��Ȃ��ꍇ�A�֐��͎��s���܂��B
				FILE_ATTRIBUTE_NORMAL,
				NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}

	DWORD dwRet;
	char szTmp[8];
	//VQF�w�b�_��ǂ݂Ƃ�
	char head[12];
	if(!ReadFile(hFile,&head,sizeof(head),&dwRet,NULL) || (dwRet != sizeof(head)))
	{
		CloseHandle(hFile);
		return -1;
	}
	
	//TWIN VQ�t�@�C���ł��邱�Ƃ��m�F
	if( (strncmp(head,"TWIN97012000",sizeof(head)) != 0) &&
		(strncmp(head,"TWIN00052200",sizeof(head)) != 0) )
	{
		CloseHandle(hFile);
		return -1;
	}
	
	//�w�b�_�T�C�Y���擾
	if(!ReadFile(hFile,&szTmp,4,&dwRet,NULL) || (dwRet != 4))
	{
		CloseHandle(hFile);
		return -1;
	}
	long lHeadSize = ((unsigned char )szTmp[0] << 24) | ((unsigned char )szTmp[1] << 16) |
						((unsigned char )szTmp[2] << 8) | (unsigned char )szTmp[3];

	DWORD dwDataSize = GetFileSize(hFile,NULL);
	dwDataSize -= lHeadSize + 16;

	//�o�b�t�@�������̊m��
	char *pRawData = (char *)malloc(dwDataSize);
	if(!pRawData)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	//raw data�̓ǂݏo��
	DWORD dwWritten;
	SetFilePointer(hFile,lHeadSize+16,NULL,FILE_BEGIN);
	if(!ReadFile(hFile,pRawData,dwDataSize,&dwWritten,NULL))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		return dwWin32errorCode;
	}
	CloseHandle(hFile);

	//==================�e���|�������쐬==================
	//�e���|���������擾
	TCHAR szTempPath[MAX_PATH];
	TCHAR szTempFile[MAX_PATH];
	lstrcpy(szTempPath,szFileName);
	cutFileName(szTempPath);
	if(!GetTempFileName(szTempPath,_T("tms"),0,szTempFile))
	{
		dwWin32errorCode = GetLastError();
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	//�t�@�C���I�[�v��(�w��t�@�C�������łɑ��݂��Ă���ꍇ�A���̃t�@�C���͏㏑������܂��B)
	hFile = CreateFile(szTempFile,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	//���܂��Ȃ�
	if(SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	//�w�b�_�̈���X�L�b�v
	if(SetFilePointer(hFile,GetTotalFieldSize() + 16,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	//�ړ���ɃR�s�[
	if(WriteFile(hFile,pRawData,dwDataSize,&dwWritten,NULL) == 0)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

	free(pRawData);

	//�w�b�_�̈���쐬
	if(SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER )
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	if(WriteFile(hFile,head,12,&dwWritten,NULL) == 0)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	//(�w�b�_�T�C�Y)
	MakeBeSize(GetTotalFieldSize(),szTmp);
	if(WriteFile(hFile,szTmp,4,&dwWritten,NULL) == 0)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	//(�^�O������������)
	map<DWORD,CVqfTag>::iterator p;
	p = m_fields.begin();
	while(p != m_fields.end())
	{
		DWORD id = p->first;
		DWORD len = p->second.GetSize();
		unsigned char *data = p->second.GetData();
		MakeBeSize(id,szTmp);
		if(WriteFile(hFile,szTmp,4,&dwWritten,NULL) == 0)
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			DeleteFile(szTempFile);
			return dwWin32errorCode;
		}
		MakeBeSize(len,szTmp);
		if(WriteFile(hFile,szTmp,4,&dwWritten,NULL) == 0)
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			DeleteFile(szTempFile);
			return dwWin32errorCode;
		}
		if(WriteFile(hFile,data,len,&dwWritten,NULL) == 0)
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			DeleteFile(szTempFile);
			return dwWin32errorCode;
		}
		
		p++;
	}
	CloseHandle(hFile);
	
	//�I���W�i���t�@�C����ޔ�(���l�[��)
	TCHAR szPreFile[MAX_PATH];
	if(!GetTempFileName(szTempPath,_T("tms"),0,szPreFile))
	{
		dwWin32errorCode = GetLastError();
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	DeleteFile(szPreFile);//�蔲��(^^;
	if(!MoveFile(szFileName,szPreFile))
	{
		dwWin32errorCode = GetLastError();
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

	//�����i�����l�[��
	if(!MoveFile(szTempFile,szFileName))
	{
		dwWin32errorCode = GetLastError();
		MoveFile(szPreFile,szFileName);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

	//�I���W�i�����폜
	DeleteFile(szPreFile);

	return dwWin32errorCode;
}

CString CVqf::GetFormatString()
{
	CString strFormat;
	if(!m_bEnable)
	{
		//�u�s���v
		strFormat.LoadString(IDS_UNKNOWN);
		return strFormat;
	}

	strFormat.Format(_T("TwinVQ %s, %dkHz, %dkbps, %s"),
					GetVer(),
					GetSamplFreq(),
					GetRate(),
					(GetStereo()&0x03)?_T("Stereo"):_T("Mono")
					);
	return strFormat;
}

CString CVqf::GetTimeString()
{
	CString strTime;
	//�u�s���v
	strTime.LoadString(IDS_UNKNOWN);
	if(!m_bEnable)
	{
		return strTime;
	}
	DWORD dwTime = GetRate()*1000/8;
	if(dwTime)
	{
		dwTime = GetStreamSize()/dwTime;
		strTime.Format(_T("%ld:%02ld (%ldsec)"),
						dwTime/60,
						dwTime%60,
						dwTime);
	}
	return strTime;
}
