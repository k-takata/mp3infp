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
	//「☆ タイトル」「[mp3infp]タイトル」
	{VT_LPSTR,LVCFMT_LEFT,30,SHCOLSTATE_TYPE_STR,IDS_COLUMN_TITLE,IDS_COLUMN_DESC_TITLE},
	//「☆ アーティスト」「[mp3infp]アーティスト」
	{VT_LPSTR,LVCFMT_LEFT,30,SHCOLSTATE_TYPE_STR,IDS_COLUMN_ARTIST,IDS_COLUMN_DESC_ARTIST},
	//「☆ アルバム」「[mp3infp]アルバム」
	{VT_LPSTR,LVCFMT_LEFT,30,SHCOLSTATE_TYPE_STR,IDS_COLUMN_ALBUM,IDS_COLUMN_DESC_ALBUM},
	//「☆ コメント」「[mp3infp]コメント」
	{VT_LPSTR,LVCFMT_LEFT,30,SHCOLSTATE_TYPE_STR,IDS_COLUMN_COMMENT,IDS_COLUMN_DESC_COMMENT},
	//「☆ ジャンル」「[mp3infp]ジャンル」
	{VT_LPSTR,LVCFMT_LEFT,15,SHCOLSTATE_TYPE_STR,IDS_COLUMN_GENRE,IDS_COLUMN_DESC_GENRE},
	//「☆ 作成日」「[mp3infp]作成日」
	{VT_LPSTR,LVCFMT_LEFT,15,SHCOLSTATE_TYPE_STR,IDS_COLUMN_DATE,IDS_COLUMN_DESC_DATE},
	//「☆ 音声フォーマット」「[mp3infp]音声フォーマット」
	{VT_LPSTR,LVCFMT_LEFT,50,SHCOLSTATE_TYPE_STR,IDS_COLUMN_AUDIO_FORMAT,IDS_COLUMN_DESC_AUDIO_FORMAT},
	//「☆ 映像フォーマット」「[mp3infp]映像フォーマット」
	{VT_LPSTR,LVCFMT_LEFT,50,SHCOLSTATE_TYPE_STR,IDS_COLUMN_VIDEO_FORMAT,IDS_COLUMN_DESC_VIDEO_FORMAT},
	//「☆ 時間」「[mp3infp]時間」
	{VT_LPSTR,LVCFMT_LEFT,20,SHCOLSTATE_TYPE_INT,IDS_COLUMN_TIME,IDS_COLUMN_DESC_TIME},
	//「☆ トラック」「[mp3infp]トラック」
	{VT_LPSTR,LVCFMT_LEFT,10,SHCOLSTATE_TYPE_INT,IDS_COLUMN_TRACK,IDS_COLUMN_DESC_TRACK},
	//「☆ mp3タグ種別」「[mp3infp]mp3タグ種別」
	{VT_LPSTR,LVCFMT_LEFT,20,SHCOLSTATE_TYPE_STR,IDS_COLUMN_TAGTYPE,IDS_COLUMN_DESC_TAGTYPE},
	//「☆ AVIバージョン」「[mp3infp]AVIバージョン」
	{VT_LPSTR,LVCFMT_LEFT,20,SHCOLSTATE_TYPE_STR,IDS_COLUMN_AVIVER,IDS_COLUMN_DESC_AVIVER},
	
	//「サイズ(byte)」「[mp3infp]サイズ(バイト単位)」
	{VT_LPSTR,LVCFMT_RIGHT,20,SHCOLSTATE_TYPE_STR|SHCOLSTATE_PREFER_VARCMP,IDS_COLUMN_SIZE,IDS_COLUMN_DESC_SIZE},
	//「8.3形式」「[mp3infp]8.3形式」
	{VT_LPSTR,LVCFMT_LEFT,20,SHCOLSTATE_TYPE_STR,IDS_COLUMN_8_3,IDS_COLUMN_DESC_8_3},
	//「拡張子」「[mp3infp]拡張子」
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

	Load();	//初期化

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
	case IDX_SIZE_BYTE://サイズ(バイト単位)
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
				// 64bitで表現できる10進数は最大20桁+6(カンマ)
				// ソート可能にするため、すべてのサイズを20桁になるように頭にスペースを入れる
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
	case IDX_8_3://8.3形式
		{
			TCHAR szBuff[MAX_PATH];
			if(GetShortPathName(m_strSelectFile,szBuff,sizeof_array(szBuff)))
			{
				strVal = getFileName(szBuff);
			}
		}
		break;
	case IDX_EXT:// 拡張子
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
	case IDX_TITLE:	//タイトル
		if(m_Id3tagv2.IsEnable())
			strVal = m_Id3tagv2.GetTitle();
		else if(m_Ape.IsEnable())
			m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TITLE,strVal);
		else if(m_Rmp3.IsEnable())
			strVal = m_Rmp3.GetNAM();
		else
			strVal = m_Id3tagv1.GetTitle();
		break;
	case IDX_ARTIST:	//アーティスト
		if(m_Id3tagv2.IsEnable())
			strVal = m_Id3tagv2.GetArtist();
		else if(m_Ape.IsEnable())
			m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,strVal);
		else if(m_Rmp3.IsEnable())
			strVal = m_Rmp3.GetART();
		else
			strVal = m_Id3tagv1.GetArtist();
		break;
	case IDX_ALBUM:	//アルバム
		if(m_Id3tagv2.IsEnable())
			strVal = m_Id3tagv2.GetAlbum();
		else if(m_Ape.IsEnable())
			m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,strVal);
		else if(m_Rmp3.IsEnable())
			strVal = m_Rmp3.GetPRD();
		else
			strVal = m_Id3tagv1.GetAlbum();
		break;
	case IDX_COMMENT:	//コメント
		if(m_Id3tagv2.IsEnable())
			strVal = m_Id3tagv2.GetComment();
		else if(m_Ape.IsEnable())
			m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,strVal);
		else if(m_Rmp3.IsEnable())
			strVal = m_Rmp3.GetCMT();
		else
			strVal = m_Id3tagv1.GetComment();
		break;
	case IDX_GENRE:	//ジャンル
		if(m_Id3tagv2.IsEnable())
			strVal = m_Id3tagv2.GetGenre();
		else if(m_Ape.IsEnable())
			m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_GENRE,strVal);
		else if(m_Rmp3.IsEnable())
			strVal = m_Rmp3.GetGNR();
		else
			strVal = m_Id3tagv1.GetGenre();
		break;
	case IDX_YEAR:	//作成日
		if(m_Id3tagv2.IsEnable())
			strVal = m_Id3tagv2.GetYear();
		else if(m_Ape.IsEnable())
			m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_YEAR,strVal);
		else if(m_Rmp3.IsEnable())
			strVal = m_Rmp3.GetCRD();
		else
			strVal = m_Id3tagv1.GetYear();
		break;
	case IDX_AFMT:	//音声フォーマット
		strVal = m_mp3info.GetFormatString();
		break;
//	case IDX_VFMT:	//映像フォーマット
	case IDX_TIME:	//時間
		strVal = m_mp3info.GetTimeString();
		break;
	case IDX_TRACK://トラック
		if(m_Id3tagv2.IsEnable())
			strVal = m_Id3tagv2.GetTrackNo();
		else if(m_Ape.IsEnable())
			m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TRACK,strVal);
		else
			strVal = m_Id3tagv1.GetTrackNo();
		break;
	case IDX_MP3TAG://タグ種別
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
//INAMを優先、無ければISBJを表示
		strVal = m_RiffSIF.GetField('I','N','A','M');
		if(strVal.GetLength() == 0)
		{
			strVal = m_RiffSIF.GetField('I','S','B','J');
		}
		break;
	case IDX_ARTIST:	//IART アーティスト名
		strVal = m_RiffSIF.GetField('I','A','R','T');
		break;
	case IDX_ALBUM:	//IPRD アルバム名
		strVal = m_RiffSIF.GetField('I','P','R','D');
		break;
	case IDX_COMMENT:	//ICMT コメント
		strVal = m_RiffSIF.GetField('I','C','M','T');
		break;
	case IDX_GENRE:	//IGNR ジャンル
		strVal = m_RiffSIF.GetField('I','G','N','R');
		break;
	case IDX_YEAR:	//ICRD 日付
		strVal = m_RiffSIF.GetField('I','C','R','D');
		break;
	case IDX_AFMT:	//音声フォーマット
		GetWaveAudioFormat((LPCTSTR )m_strSelectFile,
			m_RiffSIF.GetStreamSize(),
			strFormat,
			strTime,
			m_iWaveCodecFind);
		strVal = strFormat;
		break;
//	case IDX_VFMT:	//映像フォーマット
	case IDX_TIME:	//時間
		GetWaveAudioFormat((LPCTSTR )m_strSelectFile,
			m_RiffSIF.GetStreamSize(),
			strFormat,
			strTime,
			m_iWaveCodecFind);
		strVal = strTime;
		break;
//	case IDX_TRACK://トラック
	default:
		//ICOP 著作権
		//IENG エンジニア	
		//ISRC ソース	
		//ISFT ソフトウェア
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
//INAMを優先、無ければISBJを表示
		strVal = m_OpenDML.GetField('I','N','A','M');
		if(strVal.GetLength() == 0)
		{
			strVal = m_OpenDML.GetField('I','S','B','J');
		}
		break;
	case IDX_ARTIST:	//IART アーティスト名
		strVal = m_OpenDML.GetField('I','A','R','T');
		break;
	case IDX_ALBUM:	//IPRD アルバム名
		strVal = m_OpenDML.GetField('I','P','R','D');
		break;
	case IDX_COMMENT:	//ICMT コメント
		strVal = m_OpenDML.GetField('I','C','M','T');
		break;
	case IDX_GENRE:	//IGNR ジャンル
		strVal = m_OpenDML.GetField('I','G','N','R');
		break;
	case IDX_YEAR:	//ICRD 日付
		strVal = m_OpenDML.GetField('I','C','R','D');
		break;
	case IDX_AFMT:	//音声フォーマット
		//「なし」
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
	case IDX_VFMT:	//映像フォーマット
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
	case IDX_TIME:	//時間
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
	case IDX_AVI_VER:	//AVIバージョン
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
//	case IDX_TRACK://トラック
	default:
		//ICOP 著作権
		//IENG エンジニア	
		//ISRC ソース	
		//ISFT ソフトウェア
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
	case IDX_ARTIST:	//IART アーティスト名
		strVal = m_Vqf.GetField('A','U','T','H',&dwSize);
		break;
	case IDX_COMMENT:	//ICMT コメント
		strVal = m_Vqf.GetField('C','O','M','T',&dwSize);
		break;
	case IDX_AFMT:	//音声フォーマット
		strVal = m_Vqf.GetFormatString();
		break;
//	case IDX_VFMT:	//映像フォーマット
	case IDX_TIME:	//時間
		strVal = m_Vqf.GetTimeString();
		break;
//	case IDX_TRACK://トラック
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
	case IDX_TITLE:	//タイトル
		strVal = m_Wma.GetNAM();
		break;
	case IDX_ARTIST:	//アーティスト
		strVal = m_Wma.GetART();
		break;
	case IDX_ALBUM:	//アルバム
		strVal = m_Wma.GetPRD();
		break;
	case IDX_COMMENT:	//説明
		strVal = m_Wma.GetCMT();
		break;
	case IDX_GENRE:	//ジャンル
		strVal = m_Wma.GetGNR();
		break;
	case IDX_YEAR:	//作成日
		strVal = m_Wma.GetCRD();
		break;
	case IDX_AFMT:	//音声フォーマット
		strVal = m_Wma.GetAudioFormatString();
		break;
	case IDX_VFMT:	//映像フォーマット
		strVal = m_Wma.GetVideoFormatString();
		break;
	case IDX_TIME:	//時間
		strVal = m_Wma.GetTimeString();
		break;
	case IDX_TRACK://トラック
		strVal = m_Wma.GetTRACK();
		break;
	default:
		//URL(album)
		//URL(関連)	
		//著作権
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
	case IDX_TITLE:	//タイトル
		m_Ogg.GetComment(_T("TITLE"),0,strVal);
		break;
	case IDX_ARTIST:	//アーティスト
		m_Ogg.GetComment(_T("ARTIST"),0,strVal);
		break;
	case IDX_ALBUM:	//アルバム
		m_Ogg.GetComment(_T("ALBUM"),0,strVal);
		break;
	case IDX_COMMENT:	//説明
		m_Ogg.GetComment(_T("COMMENT"),0,strVal);
		break;
	case IDX_GENRE:	//ジャンル
		m_Ogg.GetComment(_T("GENRE"),0,strVal);
		break;
	case IDX_YEAR:	//作成日
		m_Ogg.GetComment(_T("DATE"),0,strVal);
		break;
	case IDX_AFMT:	//音声フォーマット
		strVal = m_Ogg.GetAudioFormatString();
		break;
//	case IDX_VFMT:	//映像フォーマット
	case IDX_TIME:	//時間
		strVal = m_Ogg.GetTimeString();
		break;
	case IDX_TRACK://トラック
		m_Ogg.GetComment(_T("TRACKNUMBER"),0,strVal);
		break;
	default:
		//URL(album)
		//URL(関連)	
		//著作権
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
	case IDX_TITLE:	//タイトル
		m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TITLE,strVal);
		break;
	case IDX_ARTIST:	//アーティスト
		m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,strVal);
		break;
	case IDX_ALBUM:	//アルバム
		m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,strVal);
		break;
	case IDX_COMMENT:	//説明
		m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,strVal);
		break;
	case IDX_GENRE:	//ジャンル
		m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_GENRE,strVal);
		break;
	case IDX_YEAR:	//作成日
		m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_YEAR,strVal);
		break;
	case IDX_AFMT:	//音声フォーマット
		strVal = m_Monkeys.GetAudioFormatString();
		break;
//	case IDX_VFMT:	//映像フォーマット
	case IDX_TIME:	//時間
		strVal = m_Monkeys.GetTimeString();
		break;
	case IDX_TRACK://トラック
		m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TRACK,strVal);
		break;
	default:
		//URL(album)
		//URL(関連)	
		//著作権
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
	case IDX_TITLE:	//タイトル
		strVal = m_Mp4.GetMetadata_Name();
		break;
	case IDX_ARTIST:	//アーティスト
		strVal = m_Mp4.GetMetadata_Artist();
		break;
	case IDX_ALBUM:	//アルバム
		strVal = m_Mp4.GetMetadata_Album();
		break;
	case IDX_COMMENT:	//説明
		strVal = m_Mp4.GetMetadata_Comment();
		break;
	case IDX_GENRE:	//ジャンル
		strVal = m_Mp4.GetMetadata_Genre();
		break;
	case IDX_YEAR:	//作成日
		strVal = m_Mp4.GetMetadata_Year();
		break;
	case IDX_AFMT:	//音声フォーマット
		strVal = m_Mp4.GetAudioInfoString();
		break;
	case IDX_VFMT:	//映像フォーマット
		strVal = m_Mp4.GetVideoInfoString();
		break;
//	case IDX_TIME:	//時間
	case IDX_TRACK://トラック
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
		//グループ
		//作曲
		//ディスク番号
		//テンポ
		//コンピレーション
		//ツール
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

