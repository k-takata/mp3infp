#include "StdAfx.h"
#include "mp3infp.h"

#include "psapi.h"
#pragma comment(lib,"psapi.lib")

CShellExtClassFactory::CShellExtClassFactory()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#ifdef _DEBUG
	char szProcessName[MAX_PATH];
	HANDLE hProcess = GetCurrentProcess();
	GetModuleBaseName(hProcess,NULL,szProcessName,sizeof(szProcessName));
	TRACE("[%s]CShellExtClassFactory::CShellExtClassFactory(%d->%d) process=%s\n",APP_NAME,g_cRefThisDll,g_cRefThisDll+1,szProcessName);
#endif

	m_cRef = 1L;	//Inside DCOMŽQÆ
//	m_cRef = 0L;
	g_cRefThisDll++;
}

CShellExtClassFactory::~CShellExtClassFactory()				
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#ifdef _DEBUG
	char szProcessName[MAX_PATH];
	HANDLE hProcess = GetCurrentProcess();
	GetModuleBaseName(hProcess,NULL,szProcessName,sizeof(szProcessName));
	TRACE("[%s]CShellExtClassFactory::~CShellExtClassFactory(%d->%d) process=%s\n",APP_NAME,g_cRefThisDll,g_cRefThisDll-1,szProcessName);
#endif

	g_cRefThisDll--;
}

STDMETHODIMP CShellExtClassFactory::QueryInterface(REFIID riid,
												   LPVOID FAR *ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE("[%s]CShellExtClassFactory::QueryInterface()\n",APP_NAME);

	*ppv = NULL;

	// Any interface on this object is the object pointer
	if(IsEqualIID(riid, IID_IUnknown) || IsEqualIID(riid, IID_IClassFactory))
	{
		*ppv = (LPCLASSFACTORY)this;
		AddRef();
		return NOERROR;
	}

	return E_NOINTERFACE;
}	

STDMETHODIMP_(ULONG) CShellExtClassFactory::AddRef()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE("[%s]CShellExtClassFactory::AddRef(%d->%d)\n",APP_NAME,m_cRef,m_cRef+1);

	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CShellExtClassFactory::Release()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE("[%s]CShellExtClassFactory::Release(%d->%d)\n",APP_NAME,m_cRef,m_cRef-1);

	if (--m_cRef)
		return m_cRef;

	delete this;

	return 0L;
}

STDMETHODIMP CShellExtClassFactory::CreateInstance(LPUNKNOWN pUnkOuter,
													  REFIID riid,
													  LPVOID *ppvObj)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE("[%s]CShellExtClassFactory::CreateInstance()\n",APP_NAME);

	*ppvObj = NULL;
	
	// Shell extensions typically don't support aggregation (inheritance)
	if (pUnkOuter)
		return CLASS_E_NOAGGREGATION;

	// Create the main shell extension object.  The shell will then call
	// QueryInterface with IID_IShellExtInit--this is how shell extensions are
	// initialized.
	CShellExt *pShellExt = new CShellExt;  //Create the CShellExt object

    HRESULT hr = pShellExt->QueryInterface(riid, ppvObj);

	pShellExt->Release();

	return hr;
}

STDMETHODIMP CShellExtClassFactory::LockServer(BOOL fLock)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE("[%s]CShellExtClassFactory::LockServer()\n",APP_NAME);

	return NOERROR;
}

