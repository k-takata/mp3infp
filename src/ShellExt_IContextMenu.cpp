#include "StdAfx.h"
#include "mp3infp.h"

//IContextMenu methods
STDMETHODIMP CShellExt::QueryContextMenu(HMENU hMenu, 
								UINT indexMenu, 
								UINT idCmdFirst,
								UINT idCmdLast, 
								UINT uFlags)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::QueryContextMenu()\n"),APP_NAME);

	UINT idCmd = idCmdFirst;

	if(!m_bContextMenu)
	{
		return NOERROR;
	}
	
	if( !((uFlags & CMF_NORMAL) == CMF_NORMAL) )
	{
		return NOERROR;
	}

	switch(m_fileType){
	case MP3:
		if(!m_bMp3PropEnable)
			return NOERROR;
		break;
	case WAVE:
		if(!m_bWavePropEnable)
			return NOERROR;
		break;
	case AVI:
		if(!m_bAviPropEnable)
			return NOERROR;
		break;
	case VQF:
		if(!m_bVqfPropEnable)
			return NOERROR;
		break;
	case WMA:
		if(!m_bWmaPropEnable)
			return NOERROR;
		break;
		break;
	case M3U:
		if(!m_bM3uPropEnable)
			return NOERROR;
		break;
	case OGG:
		if(!m_bOggPropEnable)
			return NOERROR;
		break;
	case APE:
		if(!m_bApePropEnable)
			return NOERROR;
		break;
	case MP4:
		if(!m_bMp4PropEnable)
			return NOERROR;
		break;
	case UNKNOWN:
	default:
		return NOERROR;//対応ファイル形式以外は除外
	}

	InsertMenu(hMenu,
				indexMenu++,
				MF_STRING|MF_BYPOSITION,
				idCmd++,
				_T("mp&3infp"));

	//Must return number of menu items we added.
	return ResultFromShort(idCmd-idCmdFirst); 
}

STDMETHODIMP CShellExt::InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::InvokeCommand()\n"),APP_NAME);

	HRESULT hr = E_INVALIDARG;

	if(HIWORD(lpcmi->lpVerb))
	{
		return hr;
	}
	switch(LOWORD(lpcmi->lpVerb)){
	case 0:
		
		mp3infp_ViewProp(lpcmi->hwnd,m_strSelectFile,0);
//		MessageBox(NULL,_T("mp3infp"),0,0);
		break;
	}

	return NOERROR;
}

STDMETHODIMP CShellExt::GetCommandString(UINT_PTR idCmd,
							 UINT uFlags,
							 UINT FAR *reserved,
							 LPSTR pszName,
							 UINT cchMax)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::GetCommandString()\n"),APP_NAME);

	CString strTmp;

	switch(idCmd){
	case 0:
		// 「プロパティを表示(mp3infp)」
		strTmp.Format(IDS_VIEW_PROP);
		break;
	default:
		break;
	}

	if(uFlags & GCS_UNICODE)
	{
		//unicode
		int len;
		LPWSTR buf = (LPWSTR) TstrToBytesAlloc(strTmp, -1, &len, BTC_CODE_UTF16LE);
		if (buf != NULL) {
			if((UINT)len > cchMax) {
				len = cchMax;
			}
			wcsncpy((LPWSTR)pszName, buf, len-1);
			((LPWSTR)pszName)[len-1] = L'\0';
			free(buf);
		}
	}
	else
	{
		//ansi
		int len;
		LPSTR buf = (LPSTR) TstrToBytesAlloc(strTmp, -1, &len, BTC_CODE_ANSI);
		if (buf != NULL) {
			if((UINT)len > cchMax) {
				len = cchMax;
			}
			strncpy(pszName, buf, len-1);
			pszName[len-1] = '\0';
			free(buf);
		}
	}

	return NOERROR;
}

