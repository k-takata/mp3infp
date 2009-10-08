#include "StdAfx.h"
#include "mp3infp.h"

//IShellPropSheetExt methods
STDMETHODIMP CShellExt::AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE("[%s]CShellExt::AddPages()\n",APP_NAME);

	DWORD dwMajor,dwMinor,dwBuildNumber1,dwBuildNumber2;

	//comctl32.dllのバージョンチェック
	if(
		!GetDLLVersion("comctl32.dll",&dwMajor,&dwMinor,&dwBuildNumber1,&dwBuildNumber2) ||
		(dwMajor < 5) ||
		((dwMajor == 5) && (dwMinor < 80))
		)
	{
		//「mp3infpの実行にはComctl32.dll Ver.5.80以上のインストールが必要です。」
		//「http://www.microsoft.com/msdownload/ieplatform/ie/comctrlx86.asp」
		CString tmp;
		tmp.LoadString(IDS_COMCTL32_INFO);
		CString ver;
		ver.Format("\nfound comctl32 version -> %d.%d.%d.%d",dwMajor,dwMinor,dwBuildNumber1,dwBuildNumber2);
		tmp += ver;
		AfxMessageBox(tmp);
		return NOERROR;
	}
	
	m_bApply = FALSE;
	switch(m_fileType){
	case MP3:
		m_bId3v1Apply = FALSE;
		m_bId3v2Apply = FALSE;
		m_bRiffApply = FALSE;
		if(!m_bMp3PropEnable)
			break;
		AddPages_mp3(lpfnAddPage,lParam);
		break;
	case WAVE:
		if(!m_bWavePropEnable)
			break;
		AddPages_wave(lpfnAddPage,lParam);
		break;
	case AVI:
		if(!m_bAviPropEnable)
			break;
		AddPages_avi(lpfnAddPage,lParam);
		break;
	case VQF:
		if(!m_bVqfPropEnable)
			break;
		AddPages_vqf(lpfnAddPage,lParam);
		break;
	case WMA:
		if(!m_bWmaPropEnable)
			break;
		AddPages_wma(lpfnAddPage,lParam);
		break;
	case M3U:
		if(!m_bM3uPropEnable)
			break;
		AddPages_m3u(lpfnAddPage,lParam);
		break;
	case OGG:
		if(!m_bOggPropEnable)
			break;
		AddPages_ogg(lpfnAddPage,lParam);
		break;
	case APE:
		if(!m_bApePropEnable)
			break;
		AddPages_ape(lpfnAddPage,lParam);
		break;
	case MP4:
		if(!m_bMp4PropEnable)
			break;
		AddPages_mp4(lpfnAddPage,lParam);
		break;
	case UNKNOWN:
	default:
		return NOERROR;
	}

	return NOERROR;
}

void CShellExt::AddPages_mp3(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
	PROPSHEETPAGE psp;
	HPROPSHEETPAGE hpage;

	m_hpageId3v1 = NULL;
	m_hpageId3v2 = NULL;
	m_hpageRiffSIF = NULL;
	m_hpageMp3Ape = NULL;

	//=== ID3V1 ===
	memset(&psp,0,sizeof(PROPSHEETPAGE));
	psp.hIcon		= 0;
	psp.dwSize		= sizeof(psp);
	psp.dwFlags		= PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
	psp.hInstance	= theApp.m_hResource;
	psp.pcRefParent	= (unsigned int *)&g_cRefThisDll;
	psp.lParam		= (LPARAM )this;
	if(m_Id3tagv1.IsEnable())
		psp.pszTitle	= "ID3v1";
	else
		psp.pszTitle	= "ID3v1(*)";
	psp.pszTemplate	= MAKEINTRESOURCE(IDD_PAGE_MP3_ID3V1);
	psp.pfnDlgProc	= PageDlgProc_mp3_ID3V1;
	psp.pfnCallback	= PageCallback_mp3_ID3V1;
	hpage=CreatePropertySheetPage(&psp); 
	if(!hpage) 
	{
		goto error;
	}
	m_hpageId3v1 = hpage;
	if(!lpfnAddPage(hpage,lParam))
	{
		goto error;
	}
	
	//=== ID3V2 ===
	memset(&psp,0,sizeof(PROPSHEETPAGE));
	psp.hIcon		= 0;
	psp.dwSize		= sizeof(psp);
	psp.dwFlags		= PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
	psp.hInstance	= theApp.m_hResource;
	psp.pcRefParent	= (unsigned int *)&g_cRefThisDll;
	psp.lParam		= (LPARAM )this;
	
	if(m_Id3tagv2.IsEnable())
		psp.pszTitle	= "ID3v2";
	else
		psp.pszTitle	= "ID3v2(*)";
	psp.pszTemplate	= MAKEINTRESOURCE(IDD_PAGE_MP3_ID3V2);
	psp.pfnDlgProc	= PageDlgProc_mp3_ID3V2;
	psp.pfnCallback	= PageCallback_mp3_ID3V2;
	hpage=CreatePropertySheetPage(&psp);
	if(!hpage) 
	{
		goto error;
	}
	m_hpageId3v2 = hpage;
	if(!lpfnAddPage(hpage,lParam))
	{
		goto error;
	}

	//=== RMP ===
	if(!m_bRiffSifDisable || m_Rmp3.IsEnable())
	{
		memset(&psp,0,sizeof(PROPSHEETPAGE));
		psp.hIcon		= 0;
		psp.dwSize		= sizeof(psp);
		psp.dwFlags		= PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
		psp.hInstance	= theApp.m_hResource;
		psp.pcRefParent	= (unsigned int *)&g_cRefThisDll;
		psp.lParam		= (LPARAM )this;
		if(m_Rmp3.IsEnable())
			psp.pszTitle	= "Riff SIF";
		else
			psp.pszTitle	= "Riff SIF(*)";
		psp.pszTemplate	= MAKEINTRESOURCE(IDD_PAGE_MP3_RMP);
		psp.pfnDlgProc	= PageDlgProc_mp3_RMP;
		psp.pfnCallback	= PageCallback_mp3_RMP;
		hpage=CreatePropertySheetPage(&psp);
		if(!hpage) 
		{
			goto error;
		}
		m_hpageRiffSIF = hpage;
		if(!lpfnAddPage(hpage,lParam))
		{
			goto error;
		}
	}

	//=== LAME ===

	//=== APE Tag ===
	if(!m_bMp3ApeTagDisable || m_Rmp3.IsEnable())
	{
		memset(&psp,0,sizeof(PROPSHEETPAGE));
		psp.hIcon		= 0;
		psp.dwSize		= sizeof(psp);
		psp.dwFlags		= PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
		psp.hInstance	= theApp.m_hResource;
		psp.pcRefParent	= (unsigned int *)&g_cRefThisDll;
		psp.lParam		= (LPARAM )this;
		if(m_Ape.IsEnable())
		{
			psp.pszTitle	= "APE";
		}
		else
		{
			psp.pszTitle	= "APE(*)";
		}
		psp.pszTemplate	= MAKEINTRESOURCE(IDD_PAGE_MP3_APE);
		psp.pfnDlgProc	= PageDlgProc_mp3_APE;
		psp.pfnCallback	= PageCallback_mp3_APE;
		hpage=CreatePropertySheetPage(&psp);
		if(!hpage) 
		{
			goto error;
		}
		m_hpageMp3Ape = hpage;
		if(!lpfnAddPage(hpage,lParam))
		{
			goto error;
		}
	}

	if(m_hpageId3v1)
	{
		AddRef();	// id3v1
	}
	if(m_hpageId3v2)
	{
		AddRef();	// id3v2
	}
	if(m_hpageRiffSIF)
	{
		AddRef();	// riffsif
	}
	if(m_hpageMp3Ape)
	{
		AddRef();	// ape
	}
	return;

error:

	if(m_hpageId3v1)
	{
		DestroyPropertySheetPage(m_hpageId3v1);
		m_hpageId3v1 = NULL;
	}
	if(m_hpageId3v2)
	{
		DestroyPropertySheetPage(m_hpageId3v2);
		m_hpageId3v2 = NULL;
	}
	if(m_hpageRiffSIF)
	{
		DestroyPropertySheetPage(m_hpageRiffSIF);
		m_hpageRiffSIF = NULL;
	}
	if(m_hpageMp3Ape)
	{
		DestroyPropertySheetPage(m_hpageMp3Ape);
		m_hpageMp3Ape = NULL;
	}
	return;
}

void CShellExt::AddPages_wave(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
	PROPSHEETPAGE psp;
	HPROPSHEETPAGE hpage;

	//=== WAVE ===
	memset(&psp,0,sizeof(PROPSHEETPAGE));
	psp.hIcon		= 0;
	psp.dwSize		= sizeof(psp);
	psp.dwFlags		= PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
	psp.hInstance	= theApp.m_hResource;
	psp.pcRefParent	= (unsigned int *)&g_cRefThisDll;
	psp.lParam		= (LPARAM )this;
	psp.pszTitle	= "mp3infp";
	psp.pszTemplate	= MAKEINTRESOURCE(IDD_PAGE_WAVE);
	psp.pfnDlgProc	= PageDlgProc_wave;
	psp.pfnCallback	= PageCallback_wave;
	hpage=CreatePropertySheetPage(&psp);
	if(!hpage) 
	{
		return;
	}
	if(!lpfnAddPage(hpage,lParam))
	{
		DestroyPropertySheetPage(hpage);
		return;
	}

	AddRef();
	return;
}

void CShellExt::AddPages_avi(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
	PROPSHEETPAGE psp;
	HPROPSHEETPAGE hpage;

	//=== AVI ===
	memset(&psp,0,sizeof(PROPSHEETPAGE));
	psp.hIcon		= 0;
	psp.dwSize		= sizeof(psp);
	psp.dwFlags		= PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
	psp.hInstance	= theApp.m_hResource;
	psp.pcRefParent	= (unsigned int *)&g_cRefThisDll;
	psp.lParam		= (LPARAM )this;
	psp.pszTitle	= "mp3infp";
	psp.pszTemplate	= MAKEINTRESOURCE(IDD_PAGE_AVI);
	psp.pfnDlgProc	= PageDlgProc_avi;
	psp.pfnCallback	= PageCallback_avi;
	hpage=CreatePropertySheetPage(&psp);
	if(!hpage) 
	{
		return;
	}
	if(!lpfnAddPage(hpage,lParam))
	{
		DestroyPropertySheetPage(hpage);
		return;
	}

	AddRef();
	return;
}

void CShellExt::AddPages_vqf(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
	PROPSHEETPAGE psp;
	HPROPSHEETPAGE hpage;

	//=== VQF ===
	memset(&psp,0,sizeof(PROPSHEETPAGE));
	psp.hIcon		= 0;
	psp.dwSize		= sizeof(psp);
	psp.dwFlags		= PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
	psp.hInstance	= theApp.m_hResource;
	psp.pcRefParent	= (unsigned int *)&g_cRefThisDll;
	psp.lParam		= (LPARAM )this;
	psp.pszTitle	= "mp3infp";
	psp.pszTemplate	= MAKEINTRESOURCE(IDD_PAGE_VQF);
	psp.pfnDlgProc	= PageDlgProc_vqf;
	psp.pfnCallback	= PageCallback_vqf;
	hpage=CreatePropertySheetPage(&psp);
	if(!hpage) 
	{
		return;
	}
	if(!lpfnAddPage(hpage,lParam))
	{
		DestroyPropertySheetPage(hpage);
		return;
	}

	AddRef();
	return;
}

void CShellExt::AddPages_wma(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
	PROPSHEETPAGE psp;
	HPROPSHEETPAGE hpage;

	//=== WMA ===
	memset(&psp,0,sizeof(PROPSHEETPAGE));
	psp.hIcon		= 0;
	psp.dwSize		= sizeof(psp);
	psp.dwFlags		= PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
	psp.hInstance	= theApp.m_hResource;
	psp.pcRefParent	= (unsigned int *)&g_cRefThisDll;
	psp.lParam		= (LPARAM )this;
	psp.pszTitle	= "mp3infp";
	psp.pszTemplate	= MAKEINTRESOURCE(IDD_PAGE_WMA);
	psp.pfnDlgProc	= PageDlgProc_wma;
	psp.pfnCallback	= PageCallback_wma;
	hpage=CreatePropertySheetPage(&psp);
	if(!hpage) 
	{
		return;
	}
	if(!lpfnAddPage(hpage,lParam))
	{
		DestroyPropertySheetPage(hpage);
		return;
	}

	AddRef();
	return;
}

void CShellExt::AddPages_m3u(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
	PROPSHEETPAGE psp;
	HPROPSHEETPAGE hpage;

	//=== M3U ===
	memset(&psp,0,sizeof(PROPSHEETPAGE));
	psp.hIcon		= 0;
	psp.dwSize		= sizeof(psp);
	psp.dwFlags		= PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
	psp.hInstance	= theApp.m_hResource;
	psp.pcRefParent	= (unsigned int *)&g_cRefThisDll;
	psp.lParam		= (LPARAM )this;
	psp.pszTitle	= "mp3infp";
	psp.pszTemplate	= MAKEINTRESOURCE(IDD_PAGE_M3U);
	psp.pfnDlgProc	= PageDlgProc_m3u;
	psp.pfnCallback	= PageCallback_m3u;
	hpage=CreatePropertySheetPage(&psp);
	if(!hpage) 
	{
		return;
	}
	if(!lpfnAddPage(hpage,lParam))
	{
		DestroyPropertySheetPage(hpage);
		return;
	}

	AddRef();
	return;
}

void CShellExt::AddPages_ogg(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
	PROPSHEETPAGE psp;
	HPROPSHEETPAGE hpage;

	//=== OGG ===
	memset(&psp,0,sizeof(PROPSHEETPAGE));
	psp.hIcon		= 0;
	psp.dwSize		= sizeof(psp);
	psp.dwFlags		= PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
	psp.hInstance	= theApp.m_hResource;
	psp.pcRefParent	= (unsigned int *)&g_cRefThisDll;
	psp.lParam		= (LPARAM )this;
	psp.pszTitle	= "mp3infp";
	psp.pszTemplate	= MAKEINTRESOURCE(IDD_PAGE_OGG);
	psp.pfnDlgProc	= PageDlgProc_ogg;
	psp.pfnCallback	= PageCallback_ogg;
	hpage=CreatePropertySheetPage(&psp);
	if(!hpage) 
	{
		return;
	}
	if(!lpfnAddPage(hpage,lParam))
	{
		DestroyPropertySheetPage(hpage);
		return;
	}

	AddRef();
	return;
}

void CShellExt::AddPages_ape(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
	PROPSHEETPAGE psp;
	HPROPSHEETPAGE hpage;

	//=== APE ===
	memset(&psp,0,sizeof(PROPSHEETPAGE));
	psp.hIcon		= 0;
	psp.dwSize		= sizeof(psp);
	psp.dwFlags		= PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
	psp.hInstance	= theApp.m_hResource;
	psp.pcRefParent	= (unsigned int *)&g_cRefThisDll;
	psp.lParam		= (LPARAM )this;
	psp.pszTitle	= "mp3infp";
	psp.pszTemplate	= MAKEINTRESOURCE(IDD_PAGE_APE);
	psp.pfnDlgProc	= PageDlgProc_ape;
	psp.pfnCallback	= PageCallback_ape;
	hpage=CreatePropertySheetPage(&psp);
	if(!hpage) 
	{
		return;
	}
	if(!lpfnAddPage(hpage,lParam))
	{
		DestroyPropertySheetPage(hpage);
		return;
	}

	AddRef();
	return;
}

void CShellExt::AddPages_mp4(LPFNADDPROPSHEETPAGE lpfnAddPage, LPARAM lParam)
{
	PROPSHEETPAGE psp;
	HPROPSHEETPAGE hpage;

	//=== MP4 ===
	memset(&psp,0,sizeof(PROPSHEETPAGE));
	psp.hIcon		= 0;
	psp.dwSize		= sizeof(psp);
	psp.dwFlags		= PSP_USEREFPARENT | PSP_USETITLE | PSP_USECALLBACK;
	psp.hInstance	= theApp.m_hResource;
	psp.pcRefParent	= (unsigned int *)&g_cRefThisDll;
	psp.lParam		= (LPARAM )this;
	psp.pszTitle	= "mp3infp";
	psp.pszTemplate	= MAKEINTRESOURCE(IDD_PAGE_MP4);
	psp.pfnDlgProc	= PageDlgProc_mp4;
	psp.pfnCallback	= PageCallback_mp4;
	hpage=CreatePropertySheetPage(&psp);
	if(!hpage) 
	{
		return;
	}
	if(!lpfnAddPage(hpage,lParam))
	{
		DestroyPropertySheetPage(hpage);
		return;
	}

	AddRef();
	return;
}

STDMETHODIMP CShellExt::ReplacePage(UINT uPageID, 
									LPFNADDPROPSHEETPAGE lpfnReplaceWith, 
									LPARAM lParam)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TRACE("[%s]CShellExt::ReplacePage()\n",APP_NAME);

	return NOERROR;
}

