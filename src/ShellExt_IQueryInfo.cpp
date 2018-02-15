#include "StdAfx.h"
#include "mp3infp.h"

//IQueryInfo methods
STDMETHODIMP CShellExt::GetInfoTip(DWORD dwFlags, WCHAR **ppwszTip)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::GetInfoTip()\n"),APP_NAME);

	CString strInfoTip = _T("");
	*ppwszTip = NULL;

TRACE(_T("[%s]CShellExt::GetInfoTip(10)\n"),APP_NAME);
	switch(m_fileType){
	case MP3:
		if(!m_bMp3InfotipEnable)
		{
			return S_FALSE;
		}
		strInfoTip += GetInfoTip_mp3();
		break;
	case WAVE:
		if(!m_bWaveInfotipEnable)
		{
			return S_FALSE;
		}
		strInfoTip += GetInfoTip_wave();
		break;
	case AVI:
		if(!m_bAviInfotipEnable)
		{
			return S_FALSE;
		}
		strInfoTip += GetInfoTip_avi();
		break;
	case VQF:
		if(!m_bVqfInfotipEnable)
		{
			return S_FALSE;
		}
		strInfoTip += GetInfoTip_vqf();
		break;
	case WMA:
		if(!m_bWmaInfotipEnable)
		{
			return S_FALSE;
		}
		strInfoTip += GetInfoTip_wma();
		break;
	case M3U:
		if(!m_bM3uInfotipEnable)
		{
			return S_FALSE;
		}
		strInfoTip += GetInfoTip_m3u();
		break;
	case OGG:
		if(!m_bOggInfotipEnable)
		{
			return S_FALSE;
		}
		strInfoTip += GetInfoTip_ogg();
		break;
	case APE:
		TRACE(_T("APE\n"));
		if(!m_bApeInfotipEnable)
		{
			return S_FALSE;
		}
		strInfoTip += GetInfoTip_ape();
		break;
	case MP4:
		TRACE(_T("MP4\n"));
		if(!m_bMp4InfotipEnable)
		{
			return S_FALSE;
		}
		strInfoTip += GetInfoTip_mp4();
		break;
	case UNKNOWN:
	default:
		return S_FALSE;
	}

	if(strInfoTip.IsEmpty())
	{
		CString strTmp;
		strInfoTip += m_strSelectFile;
		strInfoTip += _T("\n");
		//「(タグ情報はありません)」
		strTmp.Format(IDS_INFOTIP_NOTINFO);
		strInfoTip += strTmp;
	}
	//終端の改行コードを取る
	if(strInfoTip[strInfoTip.GetLength()-1] == '\n')
	{
		strInfoTip.Delete(strInfoTip.GetLength()-1,1);
	}

	//文字列を格納するメモリを確保
	LPMALLOC pMalloc = NULL;	//IMallocインターフェース
	SHGetMalloc(&pMalloc);
	*ppwszTip = (LPWSTR )pMalloc->Alloc((strInfoTip.GetLength()+1)*sizeof(OLECHAR));
	pMalloc->Release();//アロケータを解放する
	if(!*ppwszTip)
		return E_OUTOFMEMORY;

	TstrToData(strInfoTip, -1, (char *)*ppwszTip, (strInfoTip.GetLength()+1)*sizeof(OLECHAR), DTC_CODE_UTF16LE);

	return S_OK;
}



CString CShellExt::GetInfoTip_mp3()
{
	CString strInfoTip = _T("");
	CString strTmp;
	LPCTSTR pFormatString = m_strMp3InfoTipFormat;

	for(int i=0; i<m_strMp3InfoTipFormat.GetLength(); i++)
	{
		if(IS_LEAD_TBYTE(pFormatString[i]))
		{
			strInfoTip += pFormatString[i];
			i++;
			if(pFormatString[i] == '\0')
			{
				break;
			}
			strInfoTip += pFormatString[i];
		}
		else if(pFormatString[i] == '\\')
		{
			i++;
			if(pFormatString[i] == '\0')
				break;
			if(pFormatString[i] == 'n')
			{
				strInfoTip += '\n';
			}
			else if(pFormatString[i] == '\\')
			{
				strInfoTip += '\\';
			}
			else if(pFormatString[i] == 't')
			{
				strInfoTip += '\t';
			}
			else if(_tcsncmp(&pFormatString[i],_T("FILE"),4) == 0)
			{
				i += 3;
				strInfoTip += getFileName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("FEXT"),4) == 0)
			{
				i += 3;
				strInfoTip += getExtName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("PATH"),4) == 0)
			{
				i += 3;
				strInfoTip += getPathName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZ1"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeString1(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZK"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringK(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZM"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringM(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("AFMT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_mp3info.GetFormatString();
			}
			else if(_tcsncmp(&pFormatString[i],_T("TIME"),4) == 0)
			{
				i += 3;
				strInfoTip += m_mp3info.GetTimeString();
			}
			else if(_tcsncmp(&pFormatString[i],_T("TYPE"),4) == 0)
			{
				i += 3;
				if(m_Id3tagv1.IsEnable())
				{
					if(m_Id3tagv1.GetTrackNo().IsEmpty())
					{
						strInfoTip += _T("ID3v1.0 ");
					}
					else
					{
						strInfoTip += _T("ID3v1.1 ");
					}
				}
				if(m_Id3tagv2.IsEnable())
				{
					WORD wVer = m_Id3tagv2.GetVer();
					CString strVer;
					strVer.Format(_T("ID3v2.%d "),(wVer&0xff00)>>8);
					strInfoTip += strVer;
				}
				if(m_Ape.IsEnable())
				{
					CString strVer;
					strVer.Format(_T("APE Tag v%.0f "),double(m_Ape.GetApeVersion())/1000);
					strInfoTip += strVer;
				}
				strInfoTip += m_Rmp3.IsEnable()?_T("RiffSIF"):_T("");
			}
			//Id3v1/v2/APE/RiffSIF
			else if(_tcsncmp(&pFormatString[i],_T("INAM"),4) == 0)
			{
				i += 3;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += m_Id3tagv2.GetTitle();
				}
				else if(m_Ape.IsEnable())
				{
					m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TITLE,strTmp);
					strInfoTip += strTmp;
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += m_Rmp3.GetNAM();
				}
				else if(m_Id3tagv1.IsEnable())
				{
					strInfoTip += m_Id3tagv1.GetTitle();
				}
				else
				{
					strInfoTip += _T("");
				}
			}
			else if(_tcsncmp(&pFormatString[i],_T("IART"),4) == 0)
			{
				i += 3;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += m_Id3tagv2.GetArtist();
				}
				else if(m_Ape.IsEnable())
				{
					m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,strTmp);
					strInfoTip += strTmp;
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += m_Rmp3.GetART();
				}
				else if(m_Id3tagv1.IsEnable())
				{
					strInfoTip += m_Id3tagv1.GetArtist();
				}
				else
				{
					strInfoTip += _T("");
				}
			}
			else if(_tcsncmp(&pFormatString[i],_T("IPRD"),4) == 0)
			{
				i += 3;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += m_Id3tagv2.GetAlbum();
				}
				else if(m_Ape.IsEnable())
				{
					m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,strTmp);
					strInfoTip += strTmp;
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += m_Rmp3.GetPRD();
				}
				else if(m_Id3tagv1.IsEnable())
				{
					strInfoTip += m_Id3tagv1.GetAlbum();
				}
				else
				{
					strInfoTip += _T("");
				}
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICMT"),4) == 0)
			{
				i += 3;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += m_Id3tagv2.GetComment();
				}
				else if(m_Ape.IsEnable())
				{
					m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,strTmp);
					strInfoTip += strTmp;
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += m_Rmp3.GetCMT();
				}
				else if(m_Id3tagv1.IsEnable())
				{
					strInfoTip += m_Id3tagv1.GetComment();
				}
				else
				{
					strInfoTip += _T("");
				}
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICRD"),4) == 0)
			{
				i += 3;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += m_Id3tagv2.GetYear();
				}
				else if(m_Ape.IsEnable())
				{
					m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_YEAR,strTmp);
					strInfoTip += strTmp;
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += m_Rmp3.GetCRD();
				}
				else if(m_Id3tagv1.IsEnable())
				{
					strInfoTip += m_Id3tagv1.GetYear();
				}
				else
				{
					strInfoTip += _T("");
				}
			}
			else if(_tcsncmp(&pFormatString[i],_T("IGNR"),4) == 0)
			{
				i += 3;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += m_Id3tagv2.GetGenre();
				}
				else if(m_Ape.IsEnable())
				{
					m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_GENRE,strTmp);
					strInfoTip += strTmp;
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += m_Rmp3.GetGNR();
				}
				else if(m_Id3tagv1.IsEnable())
				{
					strInfoTip += m_Id3tagv1.GetGenre();
				}
				else
				{
					strInfoTip += _T("");
				}
			}
			else if(_tcsncmp(&pFormatString[i],_T("TRACK"),5) == 0)
			{
				i += 4;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += m_Id3tagv2.GetTrackNo();
				}
				else if(m_Ape.IsEnable())
				{
					m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TRACK,strTmp);
					strInfoTip += strTmp;
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += _T("");
				}
				else if(m_Id3tagv1.IsEnable())
				{
					strInfoTip += m_Id3tagv1.GetTrackNo();
				}
				else
				{
					strInfoTip += _T("");
				}
			}

			else if(_tcsncmp(&pFormatString[i],_T("ICOP"),4) == 0)
			{
				i += 3;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += m_Id3tagv2.GetCopyright();
				}
				else if(m_Ape.IsEnable())
				{
					strInfoTip += _T("");
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += m_Rmp3.GetCOP();
				}
				else
				{
					strInfoTip += _T("");
				}
			}
			else if(_tcsncmp(&pFormatString[i],_T("ISFT"),4) == 0)
			{
				i += 3;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += m_Id3tagv2.GetEncoder();
				}
				else if(m_Ape.IsEnable())
				{
					strInfoTip += _T("");
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += m_Rmp3.GetSFT();
				}
				else
				{
					strInfoTip += _T("");
				}
			}
			//Riff SIF
			else if(_tcsncmp(&pFormatString[i],_T("ISRC"),4) == 0)
			{
				i += 3;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += _T("");
				}
				else if(m_Ape.IsEnable())
				{
					strInfoTip += _T("");
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += m_Rmp3.GetSRC();
				}
				else
				{
					strInfoTip += _T("");
				}
			}
			else if(_tcsncmp(&pFormatString[i],_T("IENG"),4) == 0)
			{
				i += 3;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += _T("");
				}
				else if(m_Ape.IsEnable())
				{
					strInfoTip += _T("");
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += m_Rmp3.GetENG();
				}
				else
				{
					strInfoTip += _T("");
				}
			}
			else if(_tcsncmp(&pFormatString[i],_T("COMP"),4) == 0)			//id3v2
			{
				i += 3;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += m_Id3tagv2.GetComposer();
				}
				else if(m_Ape.IsEnable())
				{
					strInfoTip += _T("");
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += _T("");
				}
				else
				{
					strInfoTip += _T("");
				}
			}
			else if(_tcsncmp(&pFormatString[i],_T("OART"),4) == 0)
			{
				i += 3;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += m_Id3tagv2.GetOrigArtist();
				}
				else if(m_Ape.IsEnable())
				{
					strInfoTip += _T("");
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += _T("");
				}
				else
				{
					strInfoTip += _T("");
				}
			}
			else if(_tcsncmp(&pFormatString[i],_T("IURL"),4) == 0)
			{
				i += 3;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += m_Id3tagv2.GetUrl();
				}
				else if(m_Ape.IsEnable())
				{
					strInfoTip += _T("");
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += _T("");
				}
				else
				{
					strInfoTip += _T("");
				}
			}
			else if(_tcsncmp(&pFormatString[i],_T("IENC"),4) == 0)
			{
				i += 3;
				if(m_Id3tagv2.IsEnable())
				{
					strInfoTip += m_Id3tagv2.GetEncodedBy();
				}
				else if(m_Ape.IsEnable())
				{
					strInfoTip += _T("");
				}
				else if(m_Rmp3.IsEnable())
				{
					strInfoTip += _T("");
				}
				else
				{
					strInfoTip += _T("");
				}
			}
		}
		else
		{
			strInfoTip += pFormatString[i];
		}
	}
	return strInfoTip;
}

CString CShellExt::GetInfoTip_wave()
{
	CString strFormat;
	CString strTime;
	CString strInfoTip = _T("");
	LPCTSTR pFormatString = m_strWaveInfoTipFormat;

	for(int i=0; i<m_strWaveInfoTipFormat.GetLength(); i++)
	{
		if(IS_LEAD_TBYTE(pFormatString[i]))
		{
			strInfoTip += pFormatString[i];
			i++;
			if(pFormatString[i] == '\0')
			{
				break;
			}
			strInfoTip += pFormatString[i];
		}
		else if(pFormatString[i] == '\\')
		{
			i++;
			if(pFormatString[i] == '\0')
				break;
			if(pFormatString[i] == 'n')
			{
				strInfoTip += '\n';
			}
			else if(pFormatString[i] == '\\')
			{
				strInfoTip += '\\';
			}
			else if(pFormatString[i] == 't')
			{
				strInfoTip += '\t';
			}
			else if(_tcsncmp(&pFormatString[i],_T("FILE"),4) == 0)
			{
				i += 3;
				strInfoTip += getFileName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("FEXT"),4) == 0)
			{
				i += 3;
				strInfoTip += getExtName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("PATH"),4) == 0)
			{
				i += 3;
				strInfoTip += getPathName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZ1"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeString1(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZK"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringK(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZM"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringM(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("AFMT"),4) == 0)
			{
				i += 3;
				if(strFormat.IsEmpty())
				{
					GetWaveAudioFormat(m_strSelectFile,
						m_RiffSIF.GetStreamSize(),
						strFormat,
						strTime,
						m_iWaveCodecFind);
				}
				strInfoTip += strFormat;
			}
			else if(_tcsncmp(&pFormatString[i],_T("TIME"),4) == 0)
			{
				i += 3;
				if(strTime.IsEmpty())
				{
					GetWaveAudioFormat(m_strSelectFile,
						m_RiffSIF.GetStreamSize(),
						strFormat,
						strTime,
						m_iWaveCodecFind);
				}
				strInfoTip += strTime;
			}
			//RiffSIF
			else if(_tcsncmp(&pFormatString[i],_T("INAM"),4) == 0)
			{
//2002-02-10
//INAMを優先、無ければISBJを表示
				CString tmp;
				i += 3;
				tmp = m_RiffSIF.GetField('I','N','A','M');
				if(tmp.IsEmpty())
				{
					tmp = m_RiffSIF.GetField('I','S','B','J');
				}
				strInfoTip += tmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("ISBJ"),4) == 0)
			{
				i += 3;
				strInfoTip += m_RiffSIF.GetField('I','S','B','J');
			}
			else if(_tcsncmp(&pFormatString[i],_T("IART"),4) == 0)
			{
				i += 3;
				strInfoTip += m_RiffSIF.GetField('I','A','R','T');
			}
			else if(_tcsncmp(&pFormatString[i],_T("IPRD"),4) == 0)
			{
				i += 3;
				strInfoTip += m_RiffSIF.GetField('I','P','R','D');
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICMT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_RiffSIF.GetField('I','C','M','T');
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICRD"),4) == 0)
			{
				i += 3;
				strInfoTip += m_RiffSIF.GetField('I','C','R','D');
			}
			else if(_tcsncmp(&pFormatString[i],_T("IGNR"),4) == 0)
			{
				i += 3;
				strInfoTip += m_RiffSIF.GetField('I','G','N','R');
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICOP"),4) == 0)
			{
				i += 3;
				strInfoTip += m_RiffSIF.GetField('I','C','O','P');
			}
			else if(_tcsncmp(&pFormatString[i],_T("ISFT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_RiffSIF.GetField('I','S','F','T');
			}
			else if(_tcsncmp(&pFormatString[i],_T("ISRC"),4) == 0)
			{
				i += 3;
				strInfoTip += m_RiffSIF.GetField('I','S','R','C');
			}
			else if(_tcsncmp(&pFormatString[i],_T("IENG"),4) == 0)
			{
				i += 3;
				strInfoTip += m_RiffSIF.GetField('I','E','N','G');
			}
		}
		else
		{
			strInfoTip += pFormatString[i];
		}
	}
	return strInfoTip;
}

CString CShellExt::GetInfoTip_avi()
{
	CString strAudioFormat;
	CString strVideoFormat;
	CString strStreamFormat;
	CString strTime;
	BOOL bAvi2;

	CString strInfoTip = _T("");
	LPCTSTR pFormatString = m_strAviInfoTipFormat;

	for(int i=0; i<m_strAviInfoTipFormat.GetLength(); i++)
	{
		if(IS_LEAD_TBYTE(pFormatString[i]))
		{
			strInfoTip += pFormatString[i];
			i++;
			if(pFormatString[i] == '\0')
			{
				break;
			}
			strInfoTip += pFormatString[i];
		}
		else if(pFormatString[i] == '\\')
		{
			i++;
			if(pFormatString[i] == '\0')
				break;
			if(pFormatString[i] == 'n')
			{
				strInfoTip += '\n';
			}
			else if(pFormatString[i] == '\\')
			{
				strInfoTip += '\\';
			}
			else if(pFormatString[i] == 't')
			{
				strInfoTip += '\t';
			}
			else if(_tcsncmp(&pFormatString[i],_T("FILE"),4) == 0)
			{
				i += 3;
				strInfoTip += getFileName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("FEXT"),4) == 0)
			{
				i += 3;
				strInfoTip += getExtName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("PATH"),4) == 0)
			{
				i += 3;
				strInfoTip += getPathName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZ1"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeString1(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZK"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringK(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZM"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringM(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("AFMT"),4) == 0)
			{
				i += 3;
				if(strAudioFormat.IsEmpty())
				{
					GetAviFormat(m_strSelectFile,
									strAudioFormat,
									strVideoFormat,
									strStreamFormat,
									strTime,
									bAvi2,
									m_iAviCodecFind);
				}
				strTime = _T("");
				strInfoTip += strAudioFormat;
			}
			else if(_tcsncmp(&pFormatString[i],_T("VFMT"),4) == 0)
			{
				i += 3;
				if(strVideoFormat.IsEmpty())
				{
					GetAviFormat(m_strSelectFile,
									strAudioFormat,
									strVideoFormat,
									strStreamFormat,
									strTime,
									bAvi2,
									m_iAviCodecFind);
				}
				strInfoTip += strVideoFormat;
				strInfoTip += _T(", ");
				strInfoTip += strStreamFormat;
			}
			else if(_tcsncmp(&pFormatString[i],_T("TIME"),4) == 0)
			{
				i += 3;
				if(strTime.IsEmpty())
				{
					GetAviFormat(m_strSelectFile,
									strAudioFormat,
									strVideoFormat,
									strStreamFormat,
									strTime,
									bAvi2,
									m_iAviCodecFind);
				}
				strInfoTip += strTime;
			}
			//RiffSIF
			else if(_tcsncmp(&pFormatString[i],_T("ISBJ"),4) == 0)
			{
//2002-03-17
//INAMを優先、無ければISBJを表示
				CString tmp;
				i += 3;
				tmp = m_OpenDML.GetField('I','N','A','M');
				if(tmp.IsEmpty())
				{
					tmp = m_OpenDML.GetField('I','S','B','J');
				}
				strInfoTip += tmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("IART"),4) == 0)
			{
				i += 3;
				strInfoTip += m_OpenDML.GetField('I','A','R','T');
			}
			else if(_tcsncmp(&pFormatString[i],_T("IPRD"),4) == 0)
			{
				i += 3;
				strInfoTip += m_OpenDML.GetField('I','P','R','D');
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICMT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_OpenDML.GetField('I','C','M','T');
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICRD"),4) == 0)
			{
				i += 3;
				strInfoTip += m_OpenDML.GetField('I','C','R','D');
			}
			else if(_tcsncmp(&pFormatString[i],_T("IGNR"),4) == 0)
			{
				i += 3;
				strInfoTip += m_OpenDML.GetField('I','G','N','R');
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICOP"),4) == 0)
			{
				i += 3;
				strInfoTip += m_OpenDML.GetField('I','C','O','P');
			}
			else if(_tcsncmp(&pFormatString[i],_T("ISFT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_OpenDML.GetField('I','S','F','T');
			}
			else if(_tcsncmp(&pFormatString[i],_T("ISRC"),4) == 0)
			{
				i += 3;
				strInfoTip += m_OpenDML.GetField('I','S','R','C');
			}
			else if(_tcsncmp(&pFormatString[i],_T("IENG"),4) == 0)
			{
				i += 3;
				strInfoTip += m_OpenDML.GetField('I','E','N','G');
			}
			else if(_tcsncmp(&pFormatString[i],_T("AVIV"),4) == 0)
			{
				i += 3;
				if(strTime.IsEmpty())
				{
					GetAviFormat(m_strSelectFile,
									strAudioFormat,
									strVideoFormat,
									strStreamFormat,
									strTime,
									bAvi2,
									m_iAviCodecFind);
				}
				strInfoTip += bAvi2?_T("AVI 2.0 (OpenDML)"):_T("AVI 1.0 (VfW)");
			}
		}
		else
		{
			strInfoTip += pFormatString[i];
		}
	}
	return strInfoTip;
}

CString CShellExt::GetInfoTip_vqf()
{
	CString strInfoTip = _T("");
	LPCTSTR pFormatString = m_strVqfInfoTipFormat;

	for(int i=0; i<m_strVqfInfoTipFormat.GetLength(); i++)
	{
		if(IS_LEAD_TBYTE(pFormatString[i]))
		{
			strInfoTip += pFormatString[i];
			i++;
			if(pFormatString[i] == '\0')
			{
				break;
			}
			strInfoTip += pFormatString[i];
		}
		else if(pFormatString[i] == '\\')
		{
			i++;
			if(pFormatString[i] == '\0')
				break;
			if(pFormatString[i] == 'n')
			{
				strInfoTip += '\n';
			}
			else if(pFormatString[i] == '\\')
			{
				strInfoTip += '\\';
			}
			else if(pFormatString[i] == 't')
			{
				strInfoTip += '\t';
			}
			else if(_tcsncmp(&pFormatString[i],_T("FILE"),4) == 0)
			{
				i += 3;
				strInfoTip += getFileName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("FEXT"),4) == 0)
			{
				i += 3;
				strInfoTip += getExtName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("PATH"),4) == 0)
			{
				i += 3;
				strInfoTip += getPathName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZ1"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeString1(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZK"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringK(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZM"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringM(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("AFMT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Vqf.GetFormatString();
			}
			else if(_tcsncmp(&pFormatString[i],_T("TIME"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Vqf.GetTimeString();
			}
			else if(_tcsncmp(&pFormatString[i],_T("INAM"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Vqf.GetField('N','A','M','E');
			}
			else if(_tcsncmp(&pFormatString[i],_T("IART"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Vqf.GetField('A','U','T','H');
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICMT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Vqf.GetField('C','O','M','T');
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICOP"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Vqf.GetField('(','c',')',' ');
			}
			else if(_tcsncmp(&pFormatString[i],_T("FILE"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Vqf.GetField('F','I','L','E');
			}
		}
		else
		{
			strInfoTip += pFormatString[i];
		}
	}
	return strInfoTip;
}

CString CShellExt::GetInfoTip_wma()
{
	CString strInfoTip = _T("");
	LPCTSTR pFormatString = m_strWmaInfoTipFormat;

	for(int i=0; i<m_strWmaInfoTipFormat.GetLength(); i++)
	{
		if(IS_LEAD_TBYTE(pFormatString[i]))
		{
			strInfoTip += pFormatString[i];
			i++;
			if(pFormatString[i] == '\0')
			{
				break;
			}
			strInfoTip += pFormatString[i];
		}
		else if(pFormatString[i] == '\\')
		{
			i++;
			if(pFormatString[i] == '\0')
				break;
			if(pFormatString[i] == 'n')
			{
				strInfoTip += '\n';
			}
			else if(pFormatString[i] == '\\')
			{
				strInfoTip += '\\';
			}
			else if(pFormatString[i] == 't')
			{
				strInfoTip += '\t';
			}
			else if(_tcsncmp(&pFormatString[i],_T("FILE"),4) == 0)
			{
				i += 3;
				strInfoTip += getFileName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("FEXT"),4) == 0)
			{
				i += 3;
				strInfoTip += getExtName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("PATH"),4) == 0)
			{
				i += 3;
				strInfoTip += getPathName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZ1"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeString1(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZK"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringK(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZM"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringM(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("AFMT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Wma.GetAudioFormatString();
			}
			else if(_tcsncmp(&pFormatString[i],_T("VFMT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Wma.GetVideoFormatString();
			}
			else if(_tcsncmp(&pFormatString[i],_T("TIME"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Wma.GetTimeString();
			}
			else if(_tcsncmp(&pFormatString[i],_T("INAM"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Wma.GetNAM();
			}
			else if(_tcsncmp(&pFormatString[i],_T("TRACK"),5) == 0)
			{
				i += 4;
				strInfoTip += m_Wma.GetTRACK();
			}
			else if(_tcsncmp(&pFormatString[i],_T("IART"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Wma.GetART();
			}
			else if(_tcsncmp(&pFormatString[i],_T("IPRD"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Wma.GetPRD();
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICMT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Wma.GetCMT();
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICRD"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Wma.GetCRD();
			}
			else if(_tcsncmp(&pFormatString[i],_T("IGNR"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Wma.GetGNR();
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICOP"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Wma.GetCOPY();
			}
			else if(_tcsncmp(&pFormatString[i],_T("URL1"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Wma.GetUrl1();
			}
			else if(_tcsncmp(&pFormatString[i],_T("URL2"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Wma.GetUrl2();
			}
		}
		else
		{
			strInfoTip += pFormatString[i];
		}
	}
	return strInfoTip;
}

CString CShellExt::GetInfoTip_m3u()
{
	CString strInfoTip = _T("");
	CString strTmp;
	int line = 0;
	int i = 0;
	while(m_M3u.GetLine(i++,strTmp))
	{
		if(line>=M3U_INFOTIP_MAX_LINE)
		{
			break;
		}
		if(_tcsncmp(strTmp,_T("#"),1) == 0)
		{
			continue;
		}
		if(line)
		{
			strInfoTip += _T("\n");
		}
		strInfoTip += strTmp;
		line++;
	}
	if(line == M3U_INFOTIP_MAX_LINE)
	{
		strTmp.LoadString(IDS_INFOTIP_MAXLINE_OVER);
		strInfoTip += _T("\n");
		strInfoTip += strTmp;
	}
	
	return strInfoTip;
}

CString CShellExt::GetInfoTip_ogg()
{
	CString strTmp;
	CString strInfoTip = _T("");
	LPCTSTR pFormatString = m_strOggInfoTipFormat;

	for(int i=0; i<m_strOggInfoTipFormat.GetLength(); i++)
	{
		if(IS_LEAD_TBYTE(pFormatString[i]))
		{
			strInfoTip += pFormatString[i];
			i++;
			if(pFormatString[i] == '\0')
			{
				break;
			}
			strInfoTip += pFormatString[i];
		}
		else if(pFormatString[i] == '\\')
		{
			i++;
			if(pFormatString[i] == '\0')
				break;
			if(pFormatString[i] == 'n')
			{
				strInfoTip += '\n';
			}
			else if(pFormatString[i] == '\\')
			{
				strInfoTip += '\\';
			}
			else if(pFormatString[i] == 't')
			{
				strInfoTip += '\t';
			}
			else if(_tcsncmp(&pFormatString[i],_T("FILE"),4) == 0)
			{
				i += 3;
				strInfoTip += getFileName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("FEXT"),4) == 0)
			{
				i += 3;
				strInfoTip += getExtName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("PATH"),4) == 0)
			{
				i += 3;
				strInfoTip += getPathName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZ1"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeString1(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZK"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringK(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZM"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringM(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("AFMT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Ogg.GetAudioFormatString();
			}
			else if(_tcsncmp(&pFormatString[i],_T("TIME"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Ogg.GetTimeString();
			}
			else if(_tcsncmp(&pFormatString[i],_T("INAM"),4) == 0)
			{
				i += 3;
				m_Ogg.GetComment(_T("TITLE"),0,strTmp);
				strInfoTip += strTmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("TRACK"),5) == 0)
			{
				i += 4;
				m_Ogg.GetComment(_T("TRACKNUMBER"),0,strTmp);
				strInfoTip += strTmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("IART"),4) == 0)
			{
				i += 3;
				m_Ogg.GetComment(_T("ARTIST"),0,strTmp);
				strInfoTip += strTmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("IPRD"),4) == 0)
			{
				i += 3;
				m_Ogg.GetComment(_T("ALBUM"),0,strTmp);
				strInfoTip += strTmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICMT"),4) == 0)
			{
				i += 3;
				m_Ogg.GetComment(_T("COMMENT"),0,strTmp);
				strInfoTip += strTmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICRD"),4) == 0)
			{
				i += 3;
				m_Ogg.GetComment(_T("DATE"),0,strTmp);
				strInfoTip += strTmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("IGNR"),4) == 0)
			{
				i += 3;
				m_Ogg.GetComment(_T("GENRE"),0,strTmp);
				strInfoTip += strTmp;
			}
		}
		else
		{
			strInfoTip += pFormatString[i];
		}
	}
	return strInfoTip;
}

CString CShellExt::GetInfoTip_ape()
{
	CString strTmp;
	CString strInfoTip = _T("");
	LPCTSTR pFormatString = m_strApeInfoTipFormat;

	for(int i=0; i<m_strApeInfoTipFormat.GetLength(); i++)
	{
		if(IS_LEAD_TBYTE(pFormatString[i]))
		{
			strInfoTip += pFormatString[i];
			i++;
			if(pFormatString[i] == '\0')
			{
				break;
			}
			strInfoTip += pFormatString[i];
		}
		else if(pFormatString[i] == '\\')
		{
			i++;
			if(pFormatString[i] == '\0')
				break;
			if(pFormatString[i] == 'n')
			{
				strInfoTip += '\n';
			}
			else if(pFormatString[i] == '\\')
			{
				strInfoTip += '\\';
			}
			else if(pFormatString[i] == 't')
			{
				strInfoTip += '\t';
			}
			else if(_tcsncmp(&pFormatString[i],_T("FILE"),4) == 0)
			{
				i += 3;
				strInfoTip += getFileName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("FEXT"),4) == 0)
			{
				i += 3;
				strInfoTip += getExtName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("PATH"),4) == 0)
			{
				i += 3;
				strInfoTip += getPathName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZ1"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeString1(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZK"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringK(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZM"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringM(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("AFMT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Monkeys.GetAudioFormatString();
			}
			else if(_tcsncmp(&pFormatString[i],_T("TIME"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Monkeys.GetTimeString();
			}
			else if(_tcsncmp(&pFormatString[i],_T("INAM"),4) == 0)
			{
				i += 3;
				m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TITLE,strTmp);
				strInfoTip += strTmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("TRACK"),5) == 0)
			{
				i += 4;
				m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_TRACK,strTmp);
				strInfoTip += strTmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("IART"),4) == 0)
			{
				i += 3;
				m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ARTIST,strTmp);
				strInfoTip += strTmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("IPRD"),4) == 0)
			{
				i += 3;
				m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_ALBUM,strTmp);
				strInfoTip += strTmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICMT"),4) == 0)
			{
				i += 3;
				m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_COMMENT,strTmp);
				strInfoTip += strTmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICRD"),4) == 0)
			{
				i += 3;
				m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_YEAR,strTmp);
				strInfoTip += strTmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("IGNR"),4) == 0)
			{
				i += 3;
				m_Ape.GetComment(CTag_Ape::APE_TAG_FIELD_GENRE,strTmp);
				strInfoTip += strTmp;
			}
		}
		else
		{
			strInfoTip += pFormatString[i];
		}
	}
	return strInfoTip;
}

CString CShellExt::GetInfoTip_mp4()
{
	CString strTmp;
	CString strInfoTip = _T("");
	LPCTSTR pFormatString = m_strMp4InfoTipFormat;

	for(int i=0; i<m_strMp4InfoTipFormat.GetLength(); i++)
	{
		if(IS_LEAD_TBYTE(pFormatString[i]))
		{
			strInfoTip += pFormatString[i];
			i++;
			if(pFormatString[i] == '\0')
			{
				break;
			}
			strInfoTip += pFormatString[i];
		}
		else if(pFormatString[i] == '\\')
		{
			i++;
			if(pFormatString[i] == '\0')
				break;
			if(pFormatString[i] == 'n')
			{
				strInfoTip += '\n';
			}
			else if(pFormatString[i] == '\\')
			{
				strInfoTip += '\\';
			}
			else if(pFormatString[i] == 't')
			{
				strInfoTip += '\t';
			}
			else if(_tcsncmp(&pFormatString[i],_T("FILE"),4) == 0)
			{
				i += 3;
				strInfoTip += getFileName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("FEXT"),4) == 0)
			{
				i += 3;
				strInfoTip += getExtName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("PATH"),4) == 0)
			{
				i += 3;
				strInfoTip += getPathName(m_strSelectFile);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZ1"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeString1(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZK"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringK(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("SIZM"),4) == 0)
			{
				i += 3;
				strInfoTip += GetSizeStringM(m_i64FileSize);
			}
			else if(_tcsncmp(&pFormatString[i],_T("AFMT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Mp4.GetAudioInfoString();
			}
			else if(_tcsncmp(&pFormatString[i],_T("VFMT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Mp4.GetVideoInfoString();
			}
			else if(_tcsncmp(&pFormatString[i],_T("INAM"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Mp4.GetMetadata_Name();
			}
			else if(_tcsncmp(&pFormatString[i],_T("IART"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Mp4.GetMetadata_Artist();
			}
			else if(_tcsncmp(&pFormatString[i],_T("IPRD"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Mp4.GetMetadata_Album();
			}
			else if(_tcsncmp(&pFormatString[i],_T("IGRP"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Mp4.GetMetadata_Group();
			}
			else if(_tcsncmp(&pFormatString[i],_T("COMPOSER"),8) == 0)
			{
				i += 7;
				strInfoTip += m_Mp4.GetMetadata_Composer();
			}
			else if(_tcsncmp(&pFormatString[i],_T("IGNR"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Mp4.GetMetadata_Genre();
			}
			else if(_tcsncmp(&pFormatString[i],_T("TRACK"),5) == 0)
			{
				i += 4;
				int trk1 = m_Mp4.GetMetadata_Track1();
				int trk2 = m_Mp4.GetMetadata_Track2();
				if( ((trk1 != -1) && (trk1 != 0)) &&
					((trk2 != -1) && (trk2 != 0))
					)
				{
					strTmp.Format(_T("%d / %d"),trk1,trk2);
					strInfoTip += strTmp;
				}
				else if((trk1 != -1) && (trk1 != 0))
				{
					strTmp.Format(_T("%d"),trk1);
					strInfoTip += strTmp;
				}
				else if((trk2 != -1) && (trk2 != 0))
				{
					strTmp.Format(_T(" / %d"),trk2);
					strInfoTip += strTmp;
				}
			}
			else if(_tcsncmp(&pFormatString[i],_T("DISC"),4) == 0)
			{
				i += 3;
				int dsc1 = m_Mp4.GetMetadata_Disc1();
				int dsc2 = m_Mp4.GetMetadata_Disc2();
				if( ((dsc1 != -1) && (dsc1 != 0)) &&
					((dsc2 != -1) && (dsc2 != 0))
					)
				{
					strTmp.Format(_T("%d / %d"),dsc1,dsc2);
					strInfoTip += strTmp;
				}
				else if((dsc1 != -1) && (dsc1 != 0))
				{
					strTmp.Format(_T("%d"),dsc1);
					strInfoTip += strTmp;
				}
				else if((dsc2 != -1) && (dsc2 != 0))
				{
					strTmp.Format(_T(" / %d"),dsc2);
					strInfoTip += strTmp;
				}
			}
			else if(_tcsncmp(&pFormatString[i],_T("BPM"),3) == 0)
			{
				i += 2;
				strTmp.Format(_T("%d"),m_Mp4.GetMetadata_Tempo());
				strInfoTip += strTmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICRD"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Mp4.GetMetadata_Year();
			}
			else if(_tcsncmp(&pFormatString[i],_T("COMPILATION"),11) == 0)
			{
				i += 10;
				int compilation = m_Mp4.GetMetadata_Compilation();
				if((compilation == -1) || (compilation == 0))
				{
					strTmp.Format(_T("No"));
				}
				else
				{
					strTmp.Format(_T("Yes"));
				}
				strInfoTip += strTmp;
			}
			else if(_tcsncmp(&pFormatString[i],_T("ICMT"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Mp4.GetMetadata_Comment();
			}
			else if(_tcsncmp(&pFormatString[i],_T("TOOL"),4) == 0)
			{
				i += 3;
				strInfoTip += m_Mp4.GetMetadata_Tool();
			}
		}
		else
		{
			strInfoTip += pFormatString[i];
		}
	}
	return strInfoTip;
}

STDMETHODIMP CShellExt::GetInfoFlags(LPDWORD pdwFlags)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::GetInfoFlags()\n"),APP_NAME);

	return E_NOTIMPL;
}


