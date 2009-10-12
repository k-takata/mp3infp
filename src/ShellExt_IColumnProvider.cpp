#include "StdAfx.h"
#include "mp3infp.h"
#include "GlobalCommand.h"

//IColumnProvider Methods
STDMETHODIMP CShellExt::Initialize(LPCSHCOLUMNINIT psci)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::Initialize(2) fldr=%S\n"),APP_NAME,psci->wszFolder);

	return S_OK;
}

typedef struct _COLUMN_INFO{
//	DWORD		pid;
    VARTYPE     vt;
    DWORD       fmt;
    UINT        cChars;
    DWORD       csFlags; 
    DWORD		dwTitleId;
    DWORD		dwDescriptionId;
}COLUMN_INFO;

#define IDX_TITLE	1
#define IDX_ARTIST	2
#define IDX_ALBUM	3
#define IDX_COMMENT	4
#define IDX_GENRE	5
#define IDX_YEAR	6
#define IDX_AFMT	7
#define IDX_VFMT	8
#define IDX_TIME	9
#define IDX_TRACK	10
#define IDX_MP3TAG	11
#define IDX_AVI_VER	12
#define IDX_NORMAL	12

#define IDX_SIZE_BYTE	13
#define IDX_8_3		14
#define IDX_EXT		15
#define IDX_MAX		15

COLUMN_INFO cf[] = {
	//�u�� �^�C�g���v�u[mp3infp]�^�C�g���v
	{VT_LPSTR,LVCFMT_LEFT,30,SHCOLSTATE_TYPE_STR,IDS_COLUMN_TITLE,IDS_COLUMN_DESC_TITLE},
	//�u�� �A�[�e�B�X�g�v�u[mp3infp]�A�[�e�B�X�g�v
	{VT_LPSTR,LVCFMT_LEFT,30,SHCOLSTATE_TYPE_STR,IDS_COLUMN_ARTIST,IDS_COLUMN_DESC_ARTIST},
	//�u�� �A���o���v�u[mp3infp]�A���o���v
	{VT_LPSTR,LVCFMT_LEFT,30,SHCOLSTATE_TYPE_STR,IDS_COLUMN_ALBUM,IDS_COLUMN_DESC_ALBUM},
	//�u�� �R�����g�v�u[mp3infp]�R�����g�v
	{VT_LPSTR,LVCFMT_LEFT,30,SHCOLSTATE_TYPE_STR,IDS_COLUMN_COMMENT,IDS_COLUMN_DESC_COMMENT},
	//�u�� �W�������v�u[mp3infp]�W�������v
	{VT_LPSTR,LVCFMT_LEFT,15,SHCOLSTATE_TYPE_STR,IDS_COLUMN_GENRE,IDS_COLUMN_DESC_GENRE},
	//�u�� �쐬���v�u[mp3infp]�쐬���v
	{VT_LPSTR,LVCFMT_LEFT,15,SHCOLSTATE_TYPE_STR,IDS_COLUMN_DATE,IDS_COLUMN_DESC_DATE},
	//�u�� �����t�H�[�}�b�g�v�u[mp3infp]�����t�H�[�}�b�g�v
	{VT_LPSTR,LVCFMT_LEFT,50,SHCOLSTATE_TYPE_STR,IDS_COLUMN_AUDIO_FORMAT,IDS_COLUMN_DESC_AUDIO_FORMAT},
	//�u�� �f���t�H�[�}�b�g�v�u[mp3infp]�f���t�H�[�}�b�g�v
	{VT_LPSTR,LVCFMT_LEFT,50,SHCOLSTATE_TYPE_STR,IDS_COLUMN_VIDEO_FORMAT,IDS_COLUMN_DESC_VIDEO_FORMAT},
	//�u�� ���ԁv�u[mp3infp]���ԁv
	{VT_LPSTR,LVCFMT_LEFT,20,SHCOLSTATE_TYPE_INT,IDS_COLUMN_TIME,IDS_COLUMN_DESC_TIME},
	//�u�� �g���b�N�v�u[mp3infp]�g���b�N�v
	{VT_LPSTR,LVCFMT_LEFT,10,SHCOLSTATE_TYPE_INT,IDS_COLUMN_TRACK,IDS_COLUMN_DESC_TRACK},
	//�u�� mp3�^�O��ʁv�u[mp3infp]mp3�^�O��ʁv
	{VT_LPSTR,LVCFMT_LEFT,20,SHCOLSTATE_TYPE_STR,IDS_COLUMN_TAGTYPE,IDS_COLUMN_DESC_TAGTYPE},
	//�u�� AVI�o�[�W�����v�u[mp3infp]AVI�o�[�W�����v
	{VT_LPSTR,LVCFMT_LEFT,20,SHCOLSTATE_TYPE_STR,IDS_COLUMN_AVIVER,IDS_COLUMN_DESC_AVIVER},
	
	//�u�T�C�Y(byte)�v�u[mp3infp]�T�C�Y(�o�C�g�P��)�v
	{VT_LPSTR,LVCFMT_RIGHT,20,SHCOLSTATE_TYPE_STR|SHCOLSTATE_PREFER_VARCMP,IDS_COLUMN_SIZE,IDS_COLUMN_DESC_SIZE},
	//�u8.3�`���v�u[mp3infp]8.3�`���v
	{VT_LPSTR,LVCFMT_LEFT,20,SHCOLSTATE_TYPE_STR,IDS_COLUMN_8_3,IDS_COLUMN_DESC_8_3},
	//�u�g���q�v�u[mp3infp]�g���q�v
	{VT_LPSTR,LVCFMT_LEFT,10,SHCOLSTATE_TYPE_STR,IDS_COLUMN_EXTENSION,IDS_COLUMN_DESC_EXTENSION},
};

STDMETHODIMP CShellExt::GetColumnInfo(DWORD dwIndex,SHCOLUMNINFO *psci)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::GetColumnInfo()\n"),APP_NAME);

	if(!m_bColumn_8_3 && (dwIndex >= IDX_NORMAL))
	{
		TRACE(_T("[%s]CShellExt::GetColumnInfo() - dwIndex=%d,return=S_FALSE\n"),APP_NAME,dwIndex);
		return S_FALSE;
	}
	if(dwIndex >= IDX_MAX)
	{
		TRACE(_T("[%s]CShellExt::GetColumnInfo() - dwIndex=%d,return=S_FALSE\n"),APP_NAME,dwIndex);
		return S_FALSE;
	}

	TCHAR szTitle[100];
	TCHAR szDescription[100];

	lstrcpy(szTitle,_T(""));
	lstrcpy(szDescription,_T(""));

	psci->scid.fmtid = CLSID_ShellExt;

	psci->scid.pid	= dwIndex + 1;
	psci->vt		= cf[dwIndex].vt;
	psci->fmt		= cf[dwIndex].fmt;
	psci->cChars	= cf[dwIndex].cChars;
	psci->csFlags	= cf[dwIndex].csFlags | SHCOLSTATE_SLOW | SHCOLSTATE_SECONDARYUI;
	LoadString(AfxGetResourceHandle(),cf[dwIndex].dwTitleId,szTitle,sizeof_array(szTitle));
	LoadString(AfxGetResourceHandle(),cf[dwIndex].dwDescriptionId,szDescription,sizeof_array(szDescription));
	TstrToData(szTitle, -1, (char *)psci->wszTitle, MAX_COLUMN_NAME_LEN*sizeof(WCHAR), DTC_CODE_UTF16LE);
	TstrToData(szDescription, -1, (char *)psci->wszDescription, MAX_COLUMN_DESC_LEN*sizeof(WCHAR), DTC_CODE_UTF16LE);

	return S_OK;
}

STDMETHODIMP CShellExt::GetItemData(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::GetItemData()\n"),APP_NAME);

	if(pscid->fmtid != CLSID_ShellExt)
	{
		return S_FALSE;
	}

	m_strSelectFile = pscd->wszFile;
	if(GetItemData_all(pscid,pscd,pvarData))
		return S_OK;

	Load();	//������

	switch(m_fileType){
	case MP3:
		if(m_bMp3ColumnEnable && GetItemData_mp3(pscid,pscd,pvarData))
			return S_OK;
		break;
	case WAVE:
		if(m_bWaveColumnEnable && m_RiffSIF.IsEnable() && GetItemData_wave(pscid,pscd,pvarData))
			return S_OK;
		break;
	case AVI:
		if(m_bAviColumnEnable && m_OpenDML.IsEnable() && GetItemData_avi(pscid,pscd,pvarData))
			return S_OK;
		break;
	case VQF:
		if(m_bVqfColumnEnable && m_Vqf.IsEnable() && GetItemData_vqf(pscid,pscd,pvarData))
			return S_OK;
		break;
	case WMA:
		if(m_bWmaColumnEnable && m_Wma.IsEnable() && GetItemData_wma(pscid,pscd,pvarData))
			return S_OK;
		break;
	case OGG:
		if(m_bOggColumnEnable && m_Ogg.IsEnable() && GetItemData_ogg(pscid,pscd,pvarData))
			return S_OK;
		break;
	case APE:
		if(m_bApeColumnEnable && m_Ape.IsEnable() && GetItemData_ape(pscid,pscd,pvarData))
			return S_OK;
		break;
	case MP4:
		if(m_bMp4ColumnEnable && m_Mp4.IsEnable() && GetItemData_mp4(pscid,pscd,pvarData))
			return S_OK;
		break;
	case UNKNOWN:
	default:
		break;
	}

	return S_FALSE;
}

BOOL CShellExt::GetItemData_all(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData)
{
	int iSize;
	CString strVal;
	pvarData->vt = VT_BSTR;
	switch(pscid->pid){
	case IDX_SIZE_BYTE://�T�C�Y(�o�C�g�P��)
		if(pscd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			strVal = _T("<DIR>");
		}
		else
		{
			WIN32_FIND_DATA fd;
			HANDLE hFind = FindFirstFile(m_strSelectFile,&fd);
			if(hFind != INVALID_HANDLE_VALUE)
			{
				FindClose(hFind);
				// 64bit�ŕ\���ł���10�i���͍ő�20��+6(�J���})
				// �\�[�g�\�ɂ��邽�߁A���ׂẴT�C�Y��20���ɂȂ�悤�ɓ��ɃX�y�[�X������
				CString strSize;
				CString strSize2;
				strSize.Format(_T("%I64u"),(((__int64 )fd.nFileSizeHigh)<<32)|(__int64 )fd.nFileSizeLow);
//				strVal.Format(_T("%I64u"),(((__int64 )fd.nFileSizeHigh)<<32)|(__int64 )fd.nFileSizeLow);
				strSize2 = divString((LPTSTR)(LPCTSTR )strSize,',',3);
				strVal = _T("");
				for(int i=0; i<26-strSize2.GetLength(); i++)
				{
					strVal += _T(" ");
				}
				strVal += strSize2/* + _T(" bytes")*/;
			}
		}
		break;
	case IDX_8_3://8.3�`��
		{
			TCHAR szBuff[MAX_PATH];
			if(GetShortPathName(m_strSelectFile,szBuff,sizeof_array(szBuff)))
			{
				strVal = getFileName(szBuff);
			}
		}
		break;
	case IDX_EXT:// �g���q
		{
			strVal = getExtName(m_strSelectFile);
			strVal.MakeUpper();
		}
		break;
	default:
		return FALSE;
	}
	iSize = TstrToData(strVal, -1, NULL, 0, DTC_CODE_UTF16LE) / sizeof(WCHAR);
	pvarData->bstrVal = SysAllocStringLen(NULL,iSize);
	if(!pvarData->bstrVal)
		return FALSE;
	if(!TstrToData(strVal, -1, (char *)pvarData->bstrVal, iSize*sizeof(WCHAR), DTC_CODE_UTF16LE))
	{
		SysFreeString(pvarData->bstrVal);
		return FALSE;
	}

	return TRUE;
}

BOOL CShellExt::GetItemData_mp3(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData)
{
	if(pscd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		return FALSE;
	}

	int iSize;
	CString strVal;
	pvarData->vt = VT_BSTR;
	switch(pscid->pid){
	case IDX_TITLE:	//�^�C�g��
		if(m_Id3tagv2.IsEnable())
			strVal = m_Id3tagv2.GetTitle();
		else if(m_Ape.IsEnable())
			m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TITLE,strVal);
		else if(m_Rmp3.IsEnable())
			strVal = m_Rmp3.GetNAM();
		else
			strVal = m_Id3tagv1.GetTitle();
		break;
	case IDX_ARTIST:	//�A�[�e�B�X�g
		if(m_Id3tagv2.IsEnable())
			strVal = m_Id3tagv2.GetArtist();
		else if(m_Ape.IsEnable())
			m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,strVal);
		else if(m_Rmp3.IsEnable())
			strVal = m_Rmp3.GetART();
		else
			strVal = m_Id3tagv1.GetArtist();
		break;
	case IDX_ALBUM:	//�A���o��
		if(m_Id3tagv2.IsEnable())
			strVal = m_Id3tagv2.GetAlbum();
		else if(m_Ape.IsEnable())
			m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,strVal);
		else if(m_Rmp3.IsEnable())
			strVal = m_Rmp3.GetPRD();
		else
			strVal = m_Id3tagv1.GetAlbum();
		break;
	case IDX_COMMENT:	//�R�����g
		if(m_Id3tagv2.IsEnable())
			strVal = m_Id3tagv2.GetComment();
		else if(m_Ape.IsEnable())
			m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,strVal);
		else if(m_Rmp3.IsEnable())
			strVal = m_Rmp3.GetCMT();
		else
			strVal = m_Id3tagv1.GetComment();
		break;
	case IDX_GENRE:	//�W������
		if(m_Id3tagv2.IsEnable())
			strVal = m_Id3tagv2.GetGenre();
		else if(m_Ape.IsEnable())
			m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_GENRE,strVal);
		else if(m_Rmp3.IsEnable())
			strVal = m_Rmp3.GetGNR();
		else
			strVal = m_Id3tagv1.GetGenre();
		break;
	case IDX_YEAR:	//�쐬��
		if(m_Id3tagv2.IsEnable())
			strVal = m_Id3tagv2.GetYear();
		else if(m_Ape.IsEnable())
			m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_YEAR,strVal);
		else if(m_Rmp3.IsEnable())
			strVal = m_Rmp3.GetCRD();
		else
			strVal = m_Id3tagv1.GetYear();
		break;
	case IDX_AFMT:	//�����t�H�[�}�b�g
		strVal = m_mp3info.GetFormatString();
		break;
//	case IDX_VFMT:	//�f���t�H�[�}�b�g
	case IDX_TIME:	//����
		strVal = m_mp3info.GetTimeString();
		break;
	case IDX_TRACK://�g���b�N
		if(m_Id3tagv2.IsEnable())
			strVal = m_Id3tagv2.GetTrackNo();
		else if(m_Ape.IsEnable())
			m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TRACK,strVal);
		else
			strVal = m_Id3tagv1.GetTrackNo();
		break;
	case IDX_MP3TAG://�^�O���
		strVal = _T("");
		if(m_Id3tagv1.IsEnable())
		{
			if(m_Id3tagv1.GetTrackNo().GetLength())
			{
				strVal += _T("ID3v1.1 ");
			}
			else
			{
				strVal += _T("ID3v1.0 ");
			}
		}
		if(m_Id3tagv2.IsEnable())
		{
			WORD wVer = m_Id3tagv2.GetVer();
			CString strVer;
			strVer.Format(_T("ID3v2.%d "),(wVer&0xff00)>>8);
			strVal += strVer;
		}
		if(m_Ape.IsEnable())
		{
			CString strVer;
			strVer.Format(_T("ApeTagV%.0f "),double(m_Ape.GetApeVersion())/1000);
			strVal += strVer;
		}
		if(m_Rmp3.IsEnable())
			strVal += _T("RiffSIF");
		break;
	default:
		return FALSE;
	}
	iSize = TstrToData(strVal, -1, NULL, 0, DTC_CODE_UTF16LE) / sizeof(WCHAR);
	pvarData->bstrVal = SysAllocStringLen(NULL,iSize);
	if(!pvarData->bstrVal)
		return FALSE;
	if(!TstrToData(strVal, -1, (char *)pvarData->bstrVal, iSize*sizeof(WCHAR), DTC_CODE_UTF16LE))
	{
		SysFreeString(pvarData->bstrVal);
		return FALSE;
	}

	return TRUE;
}

BOOL CShellExt::GetItemData_wave(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData)
{
	CString strFormat;
	CString strTime;

	if(pscd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		return FALSE;
	}

	int iSize;
	CString strVal;
	pvarData->vt = VT_BSTR;
	switch(pscid->pid){
	case IDX_TITLE:	//ISBJ songname
//2002-01-30
//INAM��D��A�������ISBJ��\��
		strVal = m_RiffSIF.GetField('I','N','A','M');
		if(strVal.GetLength() == 0)
		{
			strVal = m_RiffSIF.GetField('I','S','B','J');
		}
		break;
	case IDX_ARTIST:	//IART �A�[�e�B�X�g��
		strVal = m_RiffSIF.GetField('I','A','R','T');
		break;
	case IDX_ALBUM:	//IPRD �A���o����
		strVal = m_RiffSIF.GetField('I','P','R','D');
		break;
	case IDX_COMMENT:	//ICMT �R�����g
		strVal = m_RiffSIF.GetField('I','C','M','T');
		break;
	case IDX_GENRE:	//IGNR �W������
		strVal = m_RiffSIF.GetField('I','G','N','R');
		break;
	case IDX_YEAR:	//ICRD ���t
		strVal = m_RiffSIF.GetField('I','C','R','D');
		break;
	case IDX_AFMT:	//�����t�H�[�}�b�g
		GetWaveAudioFormat((LPCTSTR )m_strSelectFile,
			m_RiffSIF.GetStreamSize(),
			strFormat,
			strTime,
			m_iWaveCodecFind);
		strVal = strFormat;
		break;
//	case IDX_VFMT:	//�f���t�H�[�}�b�g
	case IDX_TIME:	//����
		GetWaveAudioFormat((LPCTSTR )m_strSelectFile,
			m_RiffSIF.GetStreamSize(),
			strFormat,
			strTime,
			m_iWaveCodecFind);
		strVal = strTime;
		break;
//	case IDX_TRACK://�g���b�N
	default:
		//ICOP ���쌠
		//IENG �G���W�j�A	
		//ISRC �\�[�X	
		//ISFT �\�t�g�E�F�A
		return FALSE;
	}
	iSize = TstrToData(strVal, -1, NULL, 0, DTC_CODE_UTF16LE) / sizeof(WCHAR);
	pvarData->bstrVal = SysAllocStringLen(NULL,iSize);
	if(!pvarData->bstrVal)
		return FALSE;
	if(!TstrToData(strVal, -1, (char *)pvarData->bstrVal, iSize*sizeof(WCHAR), DTC_CODE_UTF16LE))
	{
		SysFreeString(pvarData->bstrVal);
		return FALSE;
	}

	return TRUE;
}

BOOL CShellExt::GetItemData_avi(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData)
{
	CString strAudioFormat;
	CString strVideoFormat;
	CString strStreamFormat;
	CString strTime;
	BOOL bAvi2;

	if(pscd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		return FALSE;
	}

	int iSize;
	CString strVal;
	pvarData->vt = VT_BSTR;
	switch(pscid->pid){
	case IDX_TITLE:	//ISBJ songname
//2002-03-17
//INAM��D��A�������ISBJ��\��
		strVal = m_OpenDML.GetField('I','N','A','M');
		if(strVal.GetLength() == 0)
		{
			strVal = m_OpenDML.GetField('I','S','B','J');
		}
		break;
	case IDX_ARTIST:	//IART �A�[�e�B�X�g��
		strVal = m_OpenDML.GetField('I','A','R','T');
		break;
	case IDX_ALBUM:	//IPRD �A���o����
		strVal = m_OpenDML.GetField('I','P','R','D');
		break;
	case IDX_COMMENT:	//ICMT �R�����g
		strVal = m_OpenDML.GetField('I','C','M','T');
		break;
	case IDX_GENRE:	//IGNR �W������
		strVal = m_OpenDML.GetField('I','G','N','R');
		break;
	case IDX_YEAR:	//ICRD ���t
		strVal = m_OpenDML.GetField('I','C','R','D');
		break;
	case IDX_AFMT:	//�����t�H�[�}�b�g
		//�u�Ȃ��v
		if(strAudioFormat.GetLength() == 0)
		{
			GetAviFormat((LPCTSTR )m_strSelectFile,
							strAudioFormat,
							strVideoFormat,
							strStreamFormat,
							strTime,
							bAvi2,
							m_iAviCodecFind);
		}
		strVal = strAudioFormat;
		break;
	case IDX_VFMT:	//�f���t�H�[�}�b�g
		if(strVideoFormat.GetLength() == 0)
		{
			GetAviFormat((LPCTSTR )m_strSelectFile,
							strAudioFormat,
							strVideoFormat,
							strStreamFormat,
							strTime,
							bAvi2,
							m_iAviCodecFind);
		}
		strVal = strVideoFormat;
		strVal += _T(", ");
		strVal += strStreamFormat;
		break;
	case IDX_TIME:	//����
		if(strTime.GetLength() == 0)
		{
			GetAviFormat((LPCTSTR )m_strSelectFile,
							strAudioFormat,
							strVideoFormat,
							strStreamFormat,
							strTime,
							bAvi2,
							m_iAviCodecFind);
		}
		strVal = strTime;
		break;
	case IDX_AVI_VER:	//AVI�o�[�W����
		if(strTime.GetLength() == 0)
		{
			GetAviFormat((LPCTSTR )m_strSelectFile,
							strAudioFormat,
							strVideoFormat,
							strStreamFormat,
							strTime,
							bAvi2,
							m_iAviCodecFind);
		}
		strVal = bAvi2?_T("AVI 2.0 (OpenDML)"):_T("AVI 1.0 (VfW)");
		break;
//	case IDX_TRACK://�g���b�N
	default:
		//ICOP ���쌠
		//IENG �G���W�j�A	
		//ISRC �\�[�X	
		//ISFT �\�t�g�E�F�A
		return FALSE;
	}
	iSize = TstrToData(strVal, -1, NULL, 0, DTC_CODE_UTF16LE) / sizeof(WCHAR);
	pvarData->bstrVal = SysAllocStringLen(NULL,iSize);
	if(!pvarData->bstrVal)
		return FALSE;
	if(!TstrToData(strVal, -1, (char *)pvarData->bstrVal, iSize*sizeof(WCHAR), DTC_CODE_UTF16LE))
	{
		SysFreeString(pvarData->bstrVal);
		return FALSE;
	}

	return TRUE;
}

BOOL CShellExt::GetItemData_vqf(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData)
{
	if(pscd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		return FALSE;
	}

	DWORD dwSize;
	int iSize;
	CString strVal;
	pvarData->vt = VT_BSTR;
	switch(pscid->pid){
	case IDX_TITLE:	//ISBJ songname
		strVal = m_Vqf.GetField('N','A','M','E',&dwSize);
		break;
	case IDX_ARTIST:	//IART �A�[�e�B�X�g��
		strVal = m_Vqf.GetField('A','U','T','H',&dwSize);
		break;
	case IDX_COMMENT:	//ICMT �R�����g
		strVal = m_Vqf.GetField('C','O','M','T',&dwSize);
		break;
	case IDX_AFMT:	//�����t�H�[�}�b�g
		strVal = m_Vqf.GetFormatString();
		break;
//	case IDX_VFMT:	//�f���t�H�[�}�b�g
	case IDX_TIME:	//����
		strVal = m_Vqf.GetTimeString();
		break;
//	case IDX_TRACK://�g���b�N
	default:
		return FALSE;
	}
	iSize = TstrToData(strVal, -1, NULL, 0, DTC_CODE_UTF16LE) / sizeof(WCHAR);
	pvarData->bstrVal = SysAllocStringLen(NULL,iSize);
	if(!pvarData->bstrVal)
		return FALSE;
	if(!TstrToData(strVal, -1, (char *)pvarData->bstrVal, iSize*sizeof(WCHAR), DTC_CODE_UTF16LE))
	{
		SysFreeString(pvarData->bstrVal);
		return FALSE;
	}

	return TRUE;
}

BOOL CShellExt::GetItemData_wma(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData)
{
	CString strAudioFormat;
	CString strVideoFormat;
	CString strTime;
TRACE(_T("CShellExt::GetItemData_wma(\n"));
	if(pscd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		return FALSE;
	}

	int iSize;
	CString strVal;
	pvarData->vt = VT_BSTR;
	switch(pscid->pid){
	case IDX_TITLE:	//�^�C�g��
		strVal = m_Wma.GetNAM();
		break;
	case IDX_ARTIST:	//�A�[�e�B�X�g
		strVal = m_Wma.GetART();
		break;
	case IDX_ALBUM:	//�A���o��
		strVal = m_Wma.GetPRD();
		break;
	case IDX_COMMENT:	//����
		strVal = m_Wma.GetCMT();
		break;
	case IDX_GENRE:	//�W������
		strVal = m_Wma.GetGNR();
		break;
	case IDX_YEAR:	//�쐬��
		strVal = m_Wma.GetCRD();
		break;
	case IDX_AFMT:	//�����t�H�[�}�b�g
		strVal = m_Wma.GetAudioFormatString();
		break;
	case IDX_VFMT:	//�f���t�H�[�}�b�g
		strVal = m_Wma.GetVideoFormatString();
		break;
	case IDX_TIME:	//����
		strVal = m_Wma.GetTimeString();
		break;
	case IDX_TRACK://�g���b�N
		strVal = m_Wma.GetTRACK();
		break;
	default:
		//URL(album)
		//URL(�֘A)	
		//���쌠
		return FALSE;
	}
	iSize = TstrToData(strVal, -1, NULL, 0, DTC_CODE_UTF16LE) / sizeof(WCHAR);
	pvarData->bstrVal = SysAllocStringLen(NULL,iSize);
	if(!pvarData->bstrVal)
		return FALSE;
	if(!TstrToData(strVal, -1, (char *)pvarData->bstrVal, iSize*sizeof(WCHAR), DTC_CODE_UTF16LE))
	{
		SysFreeString(pvarData->bstrVal);
		return FALSE;
	}

	return TRUE;
}

BOOL CShellExt::GetItemData_ogg(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData)
{
	CString strAudioFormat;
	CString strTime;
	if(pscd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		return FALSE;
	}

	int iSize;
	CString strVal;
	pvarData->vt = VT_BSTR;
	switch(pscid->pid){
	case IDX_TITLE:	//�^�C�g��
		m_Ogg.GetComment(_T("TITLE"),0,strVal);
		break;
	case IDX_ARTIST:	//�A�[�e�B�X�g
		m_Ogg.GetComment(_T("ARTIST"),0,strVal);
		break;
	case IDX_ALBUM:	//�A���o��
		m_Ogg.GetComment(_T("ALBUM"),0,strVal);
		break;
	case IDX_COMMENT:	//����
		m_Ogg.GetComment(_T("COMMENT"),0,strVal);
		break;
	case IDX_GENRE:	//�W������
		m_Ogg.GetComment(_T("GENRE"),0,strVal);
		break;
	case IDX_YEAR:	//�쐬��
		m_Ogg.GetComment(_T("DATE"),0,strVal);
		break;
	case IDX_AFMT:	//�����t�H�[�}�b�g
		strVal = m_Ogg.GetAudioFormatString();
		break;
//	case IDX_VFMT:	//�f���t�H�[�}�b�g
	case IDX_TIME:	//����
		strVal = m_Ogg.GetTimeString();
		break;
	case IDX_TRACK://�g���b�N
		m_Ogg.GetComment(_T("TRACKNUMBER"),0,strVal);
		break;
	default:
		//URL(album)
		//URL(�֘A)	
		//���쌠
		return FALSE;
	}
	iSize = TstrToData(strVal, -1, NULL, 0, DTC_CODE_UTF16LE) / sizeof(WCHAR);
	pvarData->bstrVal = SysAllocStringLen(NULL,iSize);
	if(!pvarData->bstrVal)
		return FALSE;
	if(!TstrToData(strVal, -1, (char *)pvarData->bstrVal, iSize*sizeof(WCHAR), DTC_CODE_UTF16LE))
	{
		SysFreeString(pvarData->bstrVal);
		return FALSE;
	}

	return TRUE;
}

BOOL CShellExt::GetItemData_ape(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData)
{
	CString strAudioFormat;
	CString strTime;
	if(pscd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		return FALSE;
	}

	int iSize;
	CString strVal;
	pvarData->vt = VT_BSTR;
	switch(pscid->pid){
	case IDX_TITLE:	//�^�C�g��
		m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TITLE,strVal);
		break;
	case IDX_ARTIST:	//�A�[�e�B�X�g
		m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,strVal);
		break;
	case IDX_ALBUM:	//�A���o��
		m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,strVal);
		break;
	case IDX_COMMENT:	//����
		m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,strVal);
		break;
	case IDX_GENRE:	//�W������
		m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_GENRE,strVal);
		break;
	case IDX_YEAR:	//�쐬��
		m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_YEAR,strVal);
		break;
	case IDX_AFMT:	//�����t�H�[�}�b�g
		strVal = m_Monkeys.GetAudioFormatString();
		break;
//	case IDX_VFMT:	//�f���t�H�[�}�b�g
	case IDX_TIME:	//����
		strVal = m_Monkeys.GetTimeString();
		break;
	case IDX_TRACK://�g���b�N
		m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TRACK,strVal);
		break;
	default:
		//URL(album)
		//URL(�֘A)	
		//���쌠
		return FALSE;
	}
	iSize = TstrToData(strVal, -1, NULL, 0, DTC_CODE_UTF16LE) / sizeof(WCHAR);
	pvarData->bstrVal = SysAllocStringLen(NULL,iSize);
	if(!pvarData->bstrVal)
		return FALSE;
	if(!TstrToData(strVal, -1, (char *)pvarData->bstrVal, iSize*sizeof(WCHAR), DTC_CODE_UTF16LE))
	{
		SysFreeString(pvarData->bstrVal);
		return FALSE;
	}

	return TRUE;
}

BOOL CShellExt::GetItemData_mp4(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData)
{
	CString strAudioFormat;
	CString strTime;
	if(pscd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		return FALSE;
	}

	int iSize;
	CString strVal;
	pvarData->vt = VT_BSTR;
	switch(pscid->pid){
	case IDX_TITLE:	//�^�C�g��
		strVal = m_Mp4.GetMetadata_Name();
		break;
	case IDX_ARTIST:	//�A�[�e�B�X�g
		strVal = m_Mp4.GetMetadata_Artist();
		break;
	case IDX_ALBUM:	//�A���o��
		strVal = m_Mp4.GetMetadata_Album();
		break;
	case IDX_COMMENT:	//����
		strVal = m_Mp4.GetMetadata_Comment();
		break;
	case IDX_GENRE:	//�W������
		strVal = m_Mp4.GetMetadata_Genre();
		break;
	case IDX_YEAR:	//�쐬��
		strVal = m_Mp4.GetMetadata_Year();
		break;
	case IDX_AFMT:	//�����t�H�[�}�b�g
		strVal = m_Mp4.GetAudioInfoString();
		break;
	case IDX_VFMT:	//�f���t�H�[�}�b�g
		strVal = m_Mp4.GetVideoInfoString();
		break;
//	case IDX_TIME:	//����
	case IDX_TRACK://�g���b�N
		{
			int trk1 = m_Mp4.GetMetadata_Track1();
			int trk2 = m_Mp4.GetMetadata_Track2();
			if( ((trk1 != -1) && (trk1 != 0)) &&
				((trk2 != -1) && (trk2 != 0))
				)
			{
				strVal.Format(_T("%d / %d"),trk1,trk2);
			}
			else if((trk1 != -1) && (trk1 != 0))
			{
				strVal.Format(_T("%d"),trk1);
			}
			else if((trk2 != -1) && (trk2 != 0))
			{
				strVal.Format(_T(" / %d"),trk2);
			}
			else
			{
				return FALSE;
			}
		}
		break;
	default:
		//�O���[�v
		//���
		//�f�B�X�N�ԍ�
		//�e���|
		//�R���s���[�V����
		//�c�[��
		return FALSE;
	}
	iSize = TstrToData(strVal, -1, NULL, 0, DTC_CODE_UTF16LE) / sizeof(WCHAR);
	pvarData->bstrVal = SysAllocStringLen(NULL,iSize);
	if(!pvarData->bstrVal)
		return FALSE;
	if(!TstrToData(strVal, -1, (char *)pvarData->bstrVal, iSize*sizeof(WCHAR), DTC_CODE_UTF16LE))
	{
		SysFreeString(pvarData->bstrVal);
		return FALSE;
	}

	return TRUE;
}

