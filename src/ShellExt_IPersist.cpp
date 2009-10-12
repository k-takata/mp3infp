#include "StdAfx.h"
#include "mp3infp.h"

//IPersist Methods
STDMETHODIMP CShellExt::GetClassID(CLSID *pClassID)
{
//	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::GetClassID()\n"),APP_NAME);

	if(!pClassID)
		return E_FAIL;

	*pClassID = CLSID_ShellExt;
	
	return S_OK;
}
