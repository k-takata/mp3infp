#ifndef _SHELLEXTCLASSFACTORY_H
#define _SHELLEXTCLASSFACTORY_H
//CShellExtClassFactory


/*===========================================================
Windows95�̃R���e�L�X�g���j���[�n���h�����쐬����ClassFactory	
===========================================================*/
class CShellExtClassFactory : public IClassFactory
{
protected:
	ULONG	m_cRef;

public:
	CShellExtClassFactory();
	~CShellExtClassFactory();

	//IUnknown members
	STDMETHODIMP			QueryInterface(REFIID, LPVOID FAR *);
	STDMETHODIMP_(ULONG)	AddRef();
	STDMETHODIMP_(ULONG)	Release();

	//IClassFactory members
	STDMETHODIMP			CreateInstance(LPUNKNOWN, REFIID, LPVOID FAR *);
	STDMETHODIMP			LockServer(BOOL);
};
typedef CShellExtClassFactory *LPCSHELLEXTCLASSFACTORY;

#endif