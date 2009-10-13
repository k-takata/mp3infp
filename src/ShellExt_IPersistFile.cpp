#include "StdAfx.h"
#include "mp3infp.h"

//IPersistFile Methods
STDMETHODIMP CShellExt::IsDirty()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::IsDirty()\n"),APP_NAME);

	return S_FALSE;
}

STDMETHODIMP CShellExt::Load(LPCOLESTR pszFileName, DWORD dwMode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::Load(%s)\n"),APP_NAME,pszFileName);

	m_strSelectFile = pszFileName;
	TRACE(_T("    +--Load (%s)\n"),m_strSelectFile);

	Load();	//èâä˙âª

	return S_OK;
}

STDMETHODIMP CShellExt::Save(LPCOLESTR pszFileName, BOOL fMember)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::Save()\n"),APP_NAME);

	return E_NOTIMPL;
}

STDMETHODIMP CShellExt::SaveCompleted(LPCOLESTR)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::SaveCompleted()\n"),APP_NAME);

	return E_NOTIMPL;
}

STDMETHODIMP CShellExt::GetCurFile(LPOLESTR *ppszFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::GetCurFile()\n"),APP_NAME);

	return S_OK;  
}
