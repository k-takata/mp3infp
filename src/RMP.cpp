// RMP.cpp: CRMP �N���X�̃C���v�������e�[�V����
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"		// ���C�� �V���{��
#include "GlobalCommand.h"
#include "Id3tagv1.h"
#include "RMP.h"
#include <io.h>

static const unsigned char SCMPX_GENRE_NULL = 247;
static const unsigned char WINAMP_GENRE_NULL = 255;

//#pragma comment(lib,"winmm.lib")

//////////////////////////////////////////////////////////////////////
// �\�z/����
//////////////////////////////////////////////////////////////////////

CRMP::CRMP(BOOL bScmpxGenre)
{
	m_bScmpxGenre = bScmpxGenre;
	Release();
}

CRMP::~CRMP()
{

}

void CRMP::Release()
{
	m_bEnable = FALSE;
	m_bHasId3tag = FALSE;
	m_strNAM = _T("");		//INAM songname
	m_strART = _T("");		//IART �A�[�e�B�X�g��
	m_strPRD = _T("");		//IPRD �A���o����
	m_strCMT = _T("");		//ICMT �R�����g
	m_strCRD = _T("");		//ICRD ���t
	m_strGNR = _T("");		//IGNR �W������
	m_strCOP = _T("");		//ICOP ���쌠
	m_strENG = _T("");		//IENG �G���W�j�A
	m_strSRC = _T("");		//ISRC �\�[�X
	m_strSFT = _T("");		//ISFT �\�t�g�E�F�A
	m_strKEY = _T("");		//IKEY �L�[���[�h
	m_strTCH = _T("");		//ITCH �Z�p��
	m_strLYC = _T("");		//ILYC �̎�
	m_strCMS = _T("");		//ICMS �R�~�b�V����
	m_strMED = _T("");		//IMED ����
	m_strSBJ = _T("");		//ISBJ subject
	m_strMP3 = _T("");		//IMP3 mp3 info
}

void CRMP::SetScmpxGenre(BOOL bSwitch)
{
	m_bScmpxGenre = bSwitch;
}

void CRMP::SetSftDefault(LPCTSTR szDefaultSoft)
{
	m_strDefaultSft = szDefaultSoft;
}

CString CRMP::ReadChunk(HMMIO hmmio,MMCKINFO mmckinfo,FOURCC id)
{
	CString retString;
	MMCKINFO	mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.ckid = id;
	if(mmioDescend(hmmio,&mmckOutinfoSubchunk,&mmckinfo,MMIO_FINDCHUNK) == MMSYSERR_NOERROR)
	{
		char *buf = (char *)malloc(mmckOutinfoSubchunk.cksize);
		if (buf != NULL) {
			mmioRead(hmmio,buf,mmckOutinfoSubchunk.cksize);
			//2001-05-12 ILYC�΍�
			if( (mmckOutinfoSubchunk.cksize > 11) && 
				(mmioFOURCC('I','L','Y','C') == id) &&
				(strncmp(buf+1,"LYRICSBEGIN",11) == 0) )
			{
				buf[0] = '_';	//�Ƃ肠����'_'�����Ă���(���\0�ɖ߂�)
			}
			retString = buf;
		}
		mmioAscend(hmmio,&mmckOutinfoSubchunk,0);
	}
	return retString;
}

DWORD CRMP::Load(LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	Release();

	// �o�b�t�@�t��I/O���g���ăt�@�C�����J��
	HMMIO hmmio = mmioOpen(const_cast<LPTSTR>(szFileName),NULL,MMIO_COMPAT);
	if(!hmmio)
	{
		return -1;
	}

	//RMP3�t�@�C���̊m�F
	char hdr[4];
	LONG ret = mmioRead(hmmio,hdr,sizeof(hdr));
	if((ret != 4) || (hdr[0] != 'R') || (hdr[1] != 'I') || (hdr[2] != 'F') || (hdr[3] != 'F'))
	{
		mmioClose(hmmio,0);
		return ERROR_SUCCESS;
	}

	mmioSeek(hmmio,0,SEEK_SET);
	//RMP3�`�����N�ֈړ�
	MMCKINFO	mmckOutinfoParent;
	memset(&mmckOutinfoParent,0,sizeof(mmckOutinfoParent));
	mmckOutinfoParent.fccType = mmioFOURCC('R','M','P','3');
	if(mmioDescend(hmmio,&mmckOutinfoParent,NULL,MMIO_FINDRIFF) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return ERROR_SUCCESS;
	}
	m_bEnable = TRUE;
	//INFO�`�����N�ֈړ�
	MMCKINFO	mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.fccType = mmioFOURCC('I','N','F','O');
	if(mmioDescend(hmmio,&mmckOutinfoSubchunk,&mmckOutinfoParent,MMIO_FINDLIST) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return ERROR_SUCCESS;
	}
	long lListTop = mmioSeek(hmmio,0,SEEK_CUR);

	//INAM songname
	m_strNAM = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','N','A','M'));
	mmioSeek(hmmio,lListTop,SEEK_SET);
	
	//IART �A�[�e�B�X�g��
	m_strART = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','A','R','T'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//IPRD �A���o����
	m_strPRD = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','P','R','D'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ICMT �R�����g
	m_strCMT = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','M','T'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ICRD ���t
	m_strCRD = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','R','D'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//IGNR �W������
	m_strGNR = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','G','N','R'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ICOP ���쌠
	m_strCOP = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','O','P'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//IENG �G���W�j�A
	m_strENG = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','E','N','G'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ISRC �\�[�X
	m_strSRC = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','R','C'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ISFT �\�t�g�E�F�A
	m_strSFT = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','F','T'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//IKEY �L�[���[�h
	m_strKEY = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','K','E','Y'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ITCH �Z�p��
	m_strTCH = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','T','C','H'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ILYC �̎�
	m_strLYC = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','L','Y','C'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ICMS �R�~�b�V����
	m_strCMS = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','M','S'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//IMED ����
	m_strMED = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','M','E','D'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//ISBJ subject
	m_strSBJ = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','B','J'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//IMP3 mp3 info
	m_strMP3 = ReadChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','M','P','3'));
	mmioSeek(hmmio,lListTop,SEEK_SET);

	//IID3
	MMCKINFO	mmckOutinfoSubSubchunk;
	memset(&mmckOutinfoSubSubchunk,0,sizeof(mmckOutinfoSubSubchunk));
	mmckOutinfoSubSubchunk.ckid = mmioFOURCC('I','I','D','3');
	if(mmioDescend(hmmio,&mmckOutinfoSubSubchunk,&mmckOutinfoSubchunk,MMIO_FINDCHUNK) == MMSYSERR_NOERROR)
	{
		mmioAscend(hmmio,&mmckOutinfoSubSubchunk,0);
		m_bHasId3tag = TRUE;
	}
	mmioSeek(hmmio,lListTop,SEEK_SET);
	
	mmioClose(hmmio,0);

	return dwWin32errorCode;
}

DWORD CRMP::Save(HWND hWnd,LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	DWORD		mp3Size;
	DWORD		listSize;
	DWORD		newSize;
	DWORD		dwNewSize;	//�\�z����鑝���T�C�Y
	DWORD		dwOldSize;	//
	FILE		*fp;
	HANDLE		hFile;
	DWORD		dwWritten;

	//�\�z����鑝���T�C�Y
	dwNewSize = strlen("LIST----INFO");
	dwNewSize += TstrToBytes(m_strNAM,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;//+1�̓��[�h�A���C�������g���l�����Ă̐��l
	dwNewSize += TstrToBytes(m_strART,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strPRD,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strCMT,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strCRD,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strGNR,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strCOP,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strENG,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strSRC,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strSFT,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strKEY,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strTCH,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strLYC,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strCMS,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strMED,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strSBJ,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += TstrToBytes(m_strMP3,-1,NULL,0,BTC_CODE_ANSI)+1+4+4;
	dwNewSize += 128+2+4+4;	//szId3dummy

	hFile = CreateFile(szFileName,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	//ID3TAGv1���̕ۑ�
	CId3tagv1::ID3_TAG id3tag;
	if(SetFilePointer(hFile,-128,NULL,FILE_END) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	if(	!ReadFile(hFile,&id3tag,sizeof(id3tag),&dwWritten,NULL) ||
		!CId3tagv1::IsTagValid(&id3tag) ||
		(dwWritten != sizeof(id3tag)) )
	{
		if(m_bHasId3tag)
		{
			CId3tagv1::GetId3tag(&id3tag,m_bScmpxGenre);
			char *buf = TstrToBytesAlloc(getFileName(szFileName), -1, NULL, BTC_CODE_ANSI);
			if (buf) {
				mbsncpy2((unsigned char *)id3tag.Title,(const unsigned char *)buf,30);
				free(buf);
			}
		}
	}
	//�����Ƀt�@�C����傫�����Ă݂�
	dwOldSize = GetFileSize(hFile,NULL);
	if(SetFilePointer(hFile,dwNewSize,NULL,FILE_END) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	if(SetEndOfFile(hFile) == 0)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	//���ɖ߂�
	if(SetFilePointer(hFile,dwOldSize,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	if(SetEndOfFile(hFile) == 0)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	CloseHandle(hFile);

	// �o�b�t�@�t��I/O���g���ăt�@�C�����J��
	HMMIO hmmio = mmioOpen(const_cast<LPTSTR>(szFileName),NULL,MMIO_COMPAT);
	if(!hmmio)
	{
		return -1;
	}
	//RMP3�t�@�C���̊m�F
	char hdr[4];
	LONG ret = mmioRead(hmmio,hdr,sizeof(hdr));
	if((ret != 4) || (hdr[0] != 'R') || (hdr[1] != 'I') || (hdr[2] != 'F') || (hdr[3] != 'F'))
	{
		mmioClose(hmmio,0);
		return -1;
	}
	mmioSeek(hmmio,0,SEEK_SET);

	//RMP3�`�����N�ֈړ�
	MMCKINFO mmckOutinfoParent;
	memset(&mmckOutinfoParent,0,sizeof(mmckOutinfoParent));
	mmckOutinfoParent.fccType = mmioFOURCC('R','M','P','3');
	if(mmioDescend(hmmio,&mmckOutinfoParent,NULL,MMIO_FINDRIFF) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	mp3Size = mmckOutinfoParent.cksize;

	MMCKINFO mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	//INFO�`�����N�ֈړ�
	mmckOutinfoSubchunk.fccType = mmioFOURCC('I','N','F','O');
	if(mmioDescend(hmmio,&mmckOutinfoSubchunk,&mmckOutinfoParent,MMIO_FINDLIST) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return -1;
	}
//	listSize=mmckOutinfoSubchunk.cksize;//INFO�`�����N�T�C�Y���������Ȃ��f�[�^�ɑΉ�
	listSize = mmioSeek(hmmio,0,SEEK_CUR);
	if(listSize == -1)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	listSize = dwOldSize - listSize + 4;
	mmioClose(hmmio,0);

	//����؂���
	if((fp=_tfopen(szFileName,_T("r+b"))) == NULL)
	{
		dwWin32errorCode = GetLastError();
		return dwWin32errorCode;
	}
	if(fseek(fp,4,SEEK_SET) != 0)
	{
		dwWin32errorCode = GetLastError();
		fclose(fp);
		return dwWin32errorCode;
	}
	newSize = mp3Size-listSize-8;
	fwrite(&newSize,1,4,fp);
	if(fseek(fp,-(long )(listSize+8),SEEK_END) != 0)
	{
		dwWin32errorCode = GetLastError();
		fclose(fp);
		return dwWin32errorCode;
	}
	//�؂��
	_chsize(fileno(fp),ftell(fp));
	fclose(fp);

	// �o�b�t�@�t��I/O���g���ăt�@�C�����J��
	if((hmmio = mmioOpen(const_cast<LPTSTR>(szFileName),NULL,MMIO_COMPAT | MMIO_READWRITE)) == NULL)
	{
		return -1;
	}
	//RMP3�t�@�C���̊m�F
	ret = mmioRead(hmmio,hdr,sizeof(hdr));
	if((ret != 4) || (hdr[0] != 'R') || (hdr[1] != 'I') || (hdr[2] != 'F') || (hdr[3] != 'F'))
	{
		mmioClose(hmmio,0);
		return -1;
	}
	mmioSeek(hmmio,0,SEEK_SET);

	//RMP3�`�����N�ֈړ�
	memset(&mmckOutinfoParent,0,sizeof(mmckOutinfoParent));
	mmckOutinfoParent.fccType = mmioFOURCC('R','M','P','3');
	if(mmioDescend(hmmio,&mmckOutinfoParent,NULL,MMIO_FINDRIFF) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	mmckOutinfoParent.dwFlags = MMIO_DIRTY;//�T�C�Y�ύX�ɐݒ�

	//�t�@�C���I�[�ɔ��
	mmioSeek(hmmio,0,SEEK_END);

	//INFO�`�����N�쐬�E�ړ�
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.fccType = mmioFOURCC('I','N','F','O');
	mmckOutinfoSubchunk.cksize = 0L;
	if(mmioCreateChunk(hmmio,&mmckOutinfoSubchunk,MMIO_CREATELIST) != 0)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	mmckOutinfoSubchunk.dwFlags = MMIO_DIRTY;//�T�C�Y�ύX�ɐݒ�
	//INAM songname
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','N','A','M'),m_strNAM,m_strNAM.GetLength()+1);
	//IART �A�[�e�B�X�g��
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','A','R','T'),m_strART,m_strART.GetLength()+1);
	//IPRD �A���o����
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','P','R','D'),m_strPRD,m_strPRD.GetLength()+1);
	//ICMT �R�����g
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','M','T'),m_strCMT,m_strCMT.GetLength()+1);
	//ICRD ���t
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','R','D'),m_strCRD,m_strCRD.GetLength()+1);
	//IGNR �W������
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','G','N','R'),m_strGNR,m_strGNR.GetLength()+1);
	//ICOP ���쌠
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','O','P'),m_strCOP,m_strCOP.GetLength()+1);
	//IENG �G���W�j�A
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','E','N','G'),m_strENG,m_strENG.GetLength()+1);
	//ISRC �\�[�X
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','R','C'),m_strSRC,m_strSRC.GetLength()+1);
	//ISFT �\�t�g�E�F�A
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','F','T'),m_strSFT,m_strSFT.GetLength()+1);
	//IKEY �L�[���[�h
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','K','E','Y'),m_strKEY,m_strKEY.GetLength()+1);
	//ITCH �Z�p��
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','T','C','H'),m_strTCH,m_strTCH.GetLength()+1);
	//ILYC �̎�
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','L','Y','C'),m_strLYC,m_strLYC.GetLength()+1);
	//ICMS �R�~�b�V����
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','M','S'),m_strCMS,m_strCMS.GetLength()+1);
	//IMED ����
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','M','E','D'),m_strMED,m_strMED.GetLength()+1);
	//ISBJ subject
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','B','J'),m_strSBJ,m_strSBJ.GetLength()+1);
	//IMP3 mp3 info
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','M','P','3'),m_strMP3,m_strMP3.GetLength()+1);
	//IID3 ID3V1 TAG
	if(m_bHasId3tag)
		WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','I','D','3'),(const char *)&id3tag,sizeof(id3tag));

	//�T�C�Y�̊m��
	if(mmioAscend(hmmio,&mmckOutinfoSubchunk,0) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	if(mmioAscend(hmmio,&mmckOutinfoParent,0) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	mmioClose(hmmio,0);

	return dwWin32errorCode;
}

DWORD CRMP::DelTag(HWND hWnd,LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	// �o�b�t�@�t��I/O���g���ăt�@�C�����J��
	HMMIO hmmio = mmioOpen(const_cast<LPTSTR>(szFileName),NULL,MMIO_COMPAT);
	if(hmmio == NULL)
	{
		return -1;
	}
	//RMP3�t�@�C���̊m�F
	char hdr[4];
	LONG ret = mmioRead(hmmio,hdr,sizeof(hdr));
	if((ret != 4) || (hdr[0] != 'R') || (hdr[1] != 'I') || (hdr[2] != 'F') || (hdr[3] != 'F'))
	{
		mmioClose(hmmio,0);
		return -1;
	}
	mmioSeek(hmmio,0,SEEK_SET);

	//RMP3�`�����N�ֈړ�
	MMCKINFO	mmckOutinfoParent;
	memset(&mmckOutinfoParent,0,sizeof(mmckOutinfoParent));
	mmckOutinfoParent.fccType = mmioFOURCC('R','M','P','3');
	if(mmioDescend(hmmio,&mmckOutinfoParent,NULL,MMIO_FINDRIFF) == MMIOERR_CHUNKNOTFOUND)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	DWORD mp3Size = mmckOutinfoParent.cksize;
	//data�`�����N�ֈړ�
	MMCKINFO	mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.ckid = mmioFOURCC('d','a','t','a');
	if(mmioDescend(hmmio,&mmckOutinfoSubchunk,&mmckOutinfoParent,MMIO_FINDCHUNK) == MMIOERR_CHUNKNOTFOUND)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	DWORD dataSize = mmckOutinfoSubchunk.cksize;
	//�o�b�t�@�������̊m��
	char *pRawData = (char *)malloc(dataSize);
	if(!pRawData)
	{
		mmioClose(hmmio,0);
		return -1;
	}
	//raw data�̓ǂݏo��
	if(mmioRead(hmmio,pRawData,dataSize) < 1)
	{
		mmioClose(hmmio,0);
		free(pRawData);
		return -1;
	}
	mmioClose(hmmio,0);

	//ID3TAGv1���̕ۑ�
	HANDLE hFile = CreateFile(szFileName,GENERIC_WRITE|GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
	{
		dwWin32errorCode = GetLastError();
		free(pRawData);
		return dwWin32errorCode;
	}
	CId3tagv1::ID3_TAG id3tag;
	if(SetFilePointer(hFile,-128,NULL,FILE_END) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		return dwWin32errorCode;
	}
	DWORD dwWritten;
	if(	ReadFile(hFile,&id3tag,sizeof(id3tag),&dwWritten,NULL) &&
		CId3tagv1::IsTagValid(&id3tag) &&
		(dwWritten == sizeof(id3tag)) )
	{
		m_bHasId3tag = TRUE;
	}
	else
	{
		m_bHasId3tag = FALSE;
	}
	CloseHandle(hFile);

	//�e���|���������擾
	TCHAR szTempPath[MAX_PATH];
	lstrcpy(szTempPath,szFileName);
	cutFileName(szTempPath);
	TCHAR szTempFile[MAX_PATH];
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
	if(SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	
	//�����o��(data)
	if(WriteFile(hFile,pRawData,dataSize,&dwWritten,NULL) == FALSE)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	free(pRawData);
	//�����o��(id3tag)
	if(m_bHasId3tag)
	{
		if(WriteFile(hFile,&id3tag,sizeof(id3tag),&dwWritten,NULL) == FALSE)
		{
			dwWin32errorCode = GetLastError();
			CloseHandle(hFile);
			DeleteFile(szTempFile);
			return dwWin32errorCode;
		}
	}
	if(CloseHandle(hFile) == 0)
	{
		dwWin32errorCode = GetLastError();
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}

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

	Release();

	return dwWin32errorCode;
}

BOOL CRMP::WriteStringChunk(HMMIO hmmio,MMCKINFO mmckinfo,FOURCC id,LPCTSTR pStr,DWORD dwLen)
{
#ifdef UNICODE
	int size;
	char *buf = TstrToBytesAlloc(pStr, dwLen, &size, BTC_CODE_ANSI);
	if (buf == NULL) {
		return FALSE;
	}
	BOOL ret = WriteChunk(hmmio, mmckinfo, id, buf, size);
	free(buf);
	return ret;
#else
	return WriteChunk(hmmio, mmckinfo, id, pStr, dwLen);
#endif
}

BOOL CRMP::WriteChunk(HMMIO hmmio,MMCKINFO mmckinfo,FOURCC id,const char *pData,DWORD dwSize)
{
	if(dwSize < 2)
		return TRUE;	//�T�C�Y��0�̃f�[�^�͋L�^���Ȃ�

	MMCKINFO	mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.ckid = id;
	mmckOutinfoSubchunk.cksize = 0L;
	if(mmioCreateChunk(hmmio,&mmckOutinfoSubchunk,0) != MMSYSERR_NOERROR)
	{
		//���s
		mmioClose(hmmio,0);
		return FALSE;
	}
	else
	{
		mmckOutinfoSubchunk.dwFlags = MMIO_DIRTY;//�T�C�Y�ύX�ɐݒ�
		//2001-05-12 ILYC�΍�
		if( (dwSize > 11) && 
			(mmioFOURCC('I','L','Y','C') == id) &&
			(strncmp(pData,"_LYRICSBEGIN",12) == 0) )
		{
			if(mmioWrite(hmmio,"\0",1) == -1)
			{
				mmioClose(hmmio,0);
				return FALSE;
			}
			if(mmioWrite(hmmio,pData+1,dwSize-1) == -1)//�擪��'_'��������
			{
				mmioClose(hmmio,0);
				return FALSE;
			}
		}
		else
		{
			if(mmioWrite(hmmio,pData,dwSize) == -1)
			{
				mmioClose(hmmio,0);
				return FALSE;
			}
		}
		if(mmioAscend(hmmio,&mmckOutinfoSubchunk,0) != MMSYSERR_NOERROR)
		{
			mmioClose(hmmio,0);
			return FALSE;
		}
	}
	return TRUE;
}

DWORD CRMP::MakeTag(HWND hWnd,LPCTSTR szFileName)
{
	DWORD	dwWin32errorCode = ERROR_SUCCESS;
	HANDLE	hFile;
	DWORD	dwWritten;

	Release();
	
	hFile = CreateFile(
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

	DWORD dwDataSize = GetFileSize(hFile,NULL);
	//�o�b�t�@�������̊m��
	char *pRawData = (char *)malloc(dwDataSize);
	if(!pRawData)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		return dwWin32errorCode;
	}
	//raw data�̓ǂݏo��
	if(!ReadFile(hFile,pRawData,dwDataSize,&dwWritten,NULL))
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		return dwWin32errorCode;
	}
	//ID3TAGv1���̕ۑ�
	CId3tagv1::ID3_TAG id3tag;
	if(SetFilePointer(hFile,-128,NULL,FILE_END) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		return dwWin32errorCode;
	}
	if(	ReadFile(hFile,&id3tag,sizeof(id3tag),&dwWritten,NULL) &&
		CId3tagv1::IsTagValid(&id3tag) &&
		(dwWritten == sizeof(id3tag)) )
	{
		m_bHasId3tag = TRUE;
		dwDataSize -= sizeof(id3tag);
	}
	else
	{
		m_bHasId3tag = FALSE;
	}

	CloseHandle(hFile);

	//�e���|���������擾
	TCHAR szTempPath[MAX_PATH];
	lstrcpy(szTempPath,szFileName);
	cutFileName(szTempPath);
	TCHAR szTempFile[MAX_PATH];
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
	if(SetFilePointer(hFile,0,NULL,FILE_BEGIN) == INVALID_SET_FILE_POINTER)
	{
		dwWin32errorCode = GetLastError();
		CloseHandle(hFile);
		free(pRawData);
		DeleteFile(szTempFile);
		return dwWin32errorCode;
	}
	CloseHandle(hFile);

	// �o�b�t�@�t��I/O���g���ăe���|�����t�@�C�����J��(���̃v���Z�X����̏����֎~)
	//(�e���|������Riff�`����mp3�t�@�C��������)
	HMMIO hmmio = mmioOpen(szTempFile,NULL,MMIO_CREATE | MMIO_WRITE | MMIO_ALLOCBUF | MMIO_DENYWRITE);
	if(!hmmio)
	{
		free(pRawData);
		DeleteFile(szTempFile);
		return -1;
	}

	//RMP3�`�����N���쐬
	MMCKINFO	mmckOutinfoParent;
	memset(&mmckOutinfoParent,0,sizeof(mmckOutinfoParent));
	mmckOutinfoParent.fccType = mmioFOURCC('R','M','P','3');
	mmckOutinfoParent.cksize = 0L;
	if(mmioCreateChunk(hmmio,&mmckOutinfoParent,MMIO_CREATERIFF) != MMSYSERR_NOERROR)
	{
		free(pRawData);
		mmioClose(hmmio,0);
		DeleteFile(szTempFile);
		return -1;
	}
	mmckOutinfoParent.dwFlags = MMIO_DIRTY;//�T�C�Y�ύX�ɐݒ�
	//data�`�����N���쐬
	MMCKINFO	mmckOutinfoSubchunk;
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.ckid = mmioFOURCC('d','a','t','a');
	mmckOutinfoSubchunk.cksize = 0L;
	if(mmioCreateChunk(hmmio,&mmckOutinfoSubchunk,0) != MMSYSERR_NOERROR)
	{
		free(pRawData);
		mmioClose(hmmio,0);
		DeleteFile(szTempFile);
		return -1;
	}
	mmckOutinfoSubchunk.dwFlags = MMIO_DIRTY;//�T�C�Y�ύX�ɐݒ�
	//data�`�����N�֏�������
	if(mmioWrite(hmmio,pRawData,dwDataSize) == -1)
	{
		free(pRawData);
		mmioClose(hmmio,0);
		DeleteFile(szTempFile);
		return -1;
	}
	free(pRawData);
	//data�`�����N���m��
	if(mmioAscend(hmmio,&mmckOutinfoSubchunk,0) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		DeleteFile(szTempFile);
		return -1;
	}
	//INFO�`�����N�쐬�E�ړ�
	memset(&mmckOutinfoSubchunk,0,sizeof(mmckOutinfoSubchunk));
	mmckOutinfoSubchunk.fccType = mmioFOURCC('I','N','F','O');
	mmckOutinfoSubchunk.cksize = 0L;
	if(mmioCreateChunk(hmmio,&mmckOutinfoSubchunk,MMIO_CREATELIST) != 0)
	{
		mmioClose(hmmio,0);
		DeleteFile(szTempFile);
		return -1;
	}

	m_strSFT = m_strDefaultSft;
	//INAM songname
	if(m_strNAM.IsEmpty())
	{
		//�f�t�H���g�Ȗ�(�t�@�C����)
		m_strNAM = getFileName((CString)szFileName);
	}
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','N','A','M'),m_strNAM,m_strNAM.GetLength()+1);
	//IART �A�[�e�B�X�g��
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','A','R','T'),m_strART,m_strART.GetLength()+1);
	//IPRD �A���o����
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','P','R','D'),m_strPRD,m_strPRD.GetLength()+1);
	//ICMT �R�����g
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','M','T'),m_strCMT,m_strCMT.GetLength()+1);
	//ICRD ���t
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','R','D'),m_strCRD,m_strCRD.GetLength()+1);
	//IGNR �W������
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','G','N','R'),m_strGNR,m_strGNR.GetLength()+1);
	//ICOP ���쌠
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','O','P'),m_strCOP,m_strCOP.GetLength()+1);
	//IENG �G���W�j�A
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','E','N','G'),m_strENG,m_strENG.GetLength()+1);
	//ISRC �\�[�X
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','R','C'),m_strSRC,m_strSRC.GetLength()+1);
	//ISFT �\�t�g�E�F�A
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','F','T'),m_strSFT,m_strSFT.GetLength()+1);
	//IKEY �L�[���[�h
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','K','E','Y'),m_strKEY,m_strKEY.GetLength()+1);
	//ITCH �Z�p��
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','T','C','H'),m_strTCH,m_strTCH.GetLength()+1);
	//ILYC �̎�
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','L','Y','C'),m_strLYC,m_strLYC.GetLength()+1);
	//ICMS �R�~�b�V����
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','C','M','S'),m_strCMS,m_strCMS.GetLength()+1);
	//IMED ����
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','M','E','D'),m_strMED,m_strMED.GetLength()+1);
	//ISBJ subject
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','S','B','J'),m_strSBJ,m_strSBJ.GetLength()+1);
	//IMP3 mp3 info
	WriteStringChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','M','P','3'),m_strMP3,m_strMP3.GetLength()+1);
	//IID3 ID3V1 TAG
	if(m_bHasId3tag)
		WriteChunk(hmmio,mmckOutinfoSubchunk,mmioFOURCC('I','I','D','3'),(const char *)&id3tag,sizeof(id3tag));

	//�T�C�Y�̏�������
	if(mmioAscend(hmmio,&mmckOutinfoSubchunk,0) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		DeleteFile(szTempFile);
		return -1;
	}
	if(mmioAscend(hmmio,&mmckOutinfoParent,0) != MMSYSERR_NOERROR)
	{
		mmioClose(hmmio,0);
		DeleteFile(szTempFile);
		return -1;
	}

	if(mmioClose(hmmio,0))
	{	//�쐻���s�����Ƃ�
		DeleteFile(szTempFile);
		return -1;
	}

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

	m_bEnable = TRUE;

	return dwWin32errorCode;
}
