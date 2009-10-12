#include "StdAfx.h"
#include "mp3infp.h"

BOOL CALLBACK CShellExt::AddPropSheetPageProc(HPROPSHEETPAGE hpage,LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::AddPropSheetPageProc()\n"),APP_NAME);
	
	CShellExt *mod = (CShellExt *)lParam;

	mod->m_tmpProp[mod->m_tmpPropCount++] = hpage;

	return TRUE;
}

void CShellExt::Initialize(LPCTSTR szFileName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::Initialize(%s)\n"),APP_NAME,szFileName);
	
	m_tmpPropCount = 0;
	m_strSelectFile = szFileName;
	Load();	//������
}

//IShellExtInit methods
STDMETHODIMP CShellExt::Initialize(LPCITEMIDLIST pIDFolder, LPDATAOBJECT pDataObj, HKEY hKeyID)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE(_T("[%s]CShellExt::Initialize(1)\n"),APP_NAME);

	//Initialize can be called more than once
	if(m_pDataObj)
	{
		m_pDataObj->Release();
	}

	//duplicate the object pointer and registry handle
	if(pDataObj)
	{
		m_pDataObj = pDataObj;
		pDataObj->AddRef();
	}
	else	//�f�[�^�I�u�W�F�N�g�����݂��Ȃ�
	{
		return E_FAIL;
	}

	//�^����ꂽIDataObject���g�p���āA�t�@�C�����̃��X�g���擾����
	FORMATETC fmte = {CF_HDROP,NULL,DVASPECT_CONTENT,-1,TYMED_HGLOBAL};
	STGMEDIUM medium;
	HRESULT hres = pDataObj->GetData(&fmte,&medium);
	if(FAILED(hres))
	{
		return E_FAIL;
	}

	//�I���t�@�C�����̎擾
	if(!medium.hGlobal)
		return E_FAIL;


	UINT cbFiles = DragQueryFile((HDROP )medium.hGlobal,(UINT )-1,0,0);
	if(cbFiles != 1)
		return E_FAIL;//�����I�����͋p��

	TCHAR szFileName[MAX_PATH];
	DragQueryFile((HDROP)medium.hGlobal, 
				0, 
				szFileName,
				MAX_PATH);
	m_strSelectFile = szFileName;
	Load();	//������

	GlobalUnlock(medium.hGlobal);
    ReleaseStgMedium(&medium);

	return NOERROR;
}
