#ifndef _SHELLEXT_H
#define _SHELLEXT_H
//ShellExtクラス定義

#include <shlobj.h>

void DlgContextMenu(HWND hDlg,LPARAM lParam,BOOL isEnable);
void HandleMenuCopy(HWND hDlg,const int *idArray,const int *staticWndArray,const int *editWndArray,const CString &strFile);
void SetDlgOutlineTextSp(HWND hDlg,const int *idArray,const int *editWndArray);
HGLOBAL GetDlgOutlineTextSp(HWND hDlg,const int *idArray,const int *editWndArray);
HGLOBAL GetDlgOutlineText(HWND hDlg,const int *staticWndArray,const int *editWndArray,const CString &strFileName);
void OpenConfigPage(HWND hwnd, int pagenum);
void OpenAboutDlg(HWND hwnd);
#define ResultFromShort(i)  ResultFromScode(MAKE_SCODE(SEVERITY_SUCCESS, 0, (USHORT)(i)))
#define CF_MP3INFP	_T("CF_MP3INFP")
#define CLP_NAM	1	// タイトル
#define CLP_ART	2	// アーティスト
#define CLP_PRD	3	// アルバム
#define CLP_CRD	4	// 西暦
#define CLP_GNR	5	// ジャンル
#define CLP_CMT	6	// コメント
#define CLP_COP	7	// 著作権
#define CLP_ENG	8	// エンジニア
#define CLP_SRC	9	// ソース
#define CLP_SFT	10	// ソフトウェア
#define CLP_ORGART	11	// orig.アーティスト
#define CLP_URL1	12	// URL1
#define CLP_URL2	13	// URL2
#define CLP_ENC		14	// エンコーダ
#define CLP_ENC2	15	// エンコードした人または組織
#define CLP_TRACK	16	// トラック
#define CLP_FILE	17	// 保存名
#define CLP_COMPOSE	20	// 作曲者
#define CLP_TRACK2	21	// トラック2
#define CLP_DISC	22	// ディスク
#define CLP_DISC2	23	// ディスク2
#define CLP_BPM		24	// テンポ
#define CLP_COMPILATION	25
#define CLP_GROUP	26
#define CLP_AART	27	// アルバムアーティスト

#ifdef UNICODE
#define CF_TTEXT	CF_UNICODETEXT
#else
#define CF_TTEXT	CF_TEXT
#endif

#define DLG_CLIPBOARD_MACRO(strFile)	\
	case IDCANCEL:	\
	{				\
		/* Forward this message to the parent window */ \
		/* so that the PPT sheet is dismissed. */ \
		SendMessage(GetParent(hDlg),uMessage,wParam,lParam);	\
		break;	\
	}	\
	case ID_MENU_COPY:	\
	{	\
		HandleMenuCopy(hDlg,ids,staticWnd,editWnd,strFile);	\
		break;	\
	}	\
	case ID_MENU_PASTE:	\
	{	\
		SetDlgOutlineTextSp(hDlg,ids,editWnd);	\
		break;	\
	}

/*===========================================================
OLEコンテキストメニューハンドラ	
===========================================================*/
class CShellExt : public	
							IShellPropSheetExt,
							IShellExtInit,
							IContextMenu,
							IPersistFile,
							IColumnProvider,
							IQueryInfo
							
{
public:
	enum EN_FILETYPE
	{
		UNKNOWN,MP3,WAVE,AVI,VQF,WMA,M3U,OGG,APE,MP4
	};
protected:
	ULONG			m_cRef;
	LPDATAOBJECT	m_pDataObj;

	EN_FILETYPE		m_fileType;
	BOOL			m_bMultiSelect;
	CString			m_strSelectFile;
	CTimeStampSaver	m_timestamp;	//タイムスタンプ保存用

	BOOL			m_bSaveTimeStamp;	//タイムスタンプを保存
	BOOL			m_bContextMenu;		//コンテキストメニューに"mp3infp"を追加
	BOOL			m_bSelectDrive;		//mp3infpを使用しないドライブの種類を指定
	BOOL			m_bDisable_FDD;		//フロッピーディスクドライブ
	BOOL			m_bDisable_REMOVABLE;//リムーバブルドライブ
	BOOL			m_bDisable_CDROM;	//CDROMドライブ
	BOOL			m_bDisable_NETWORK;	//ネットワークドライブ

	BOOL			m_bPropAOT;			//プロパティ拡張 - 常に手前表示
	BOOL			m_bColumn_8_3;		//カラム拡張 - 8.3形式/ファイルサイズ

	__int64			m_i64FileSize;

	//MP3=====================================================
	BOOL			m_bMp3PropEnable;
	BOOL			m_bMp3InfotipEnable;
	BOOL			m_bMp3ColumnEnable;
	CString			m_strMp3InfoTipFormat;//Infotipの書式

	//===mp3info
	CMp3Info		m_mp3info;

	//===ID3v1(.1)
	CId3tagv1		m_Id3tagv1;
	BOOL			m_bId3v1ScmpxGenre;
	HWND			m_hwndId3v1;
	HPROPSHEETPAGE	m_hpageId3v1;
	BOOL			m_bId3v1Apply;	//保存が必要なときにTRUE

	//===ID3v2
	CId3tagv2		m_Id3tagv2;
	HWND			m_hwndId3v2;
	HPROPSHEETPAGE	m_hpageId3v2;
	BOOL			m_bId3v2Apply;	//保存が必要なときにTRUE
	
	//===Riff SIF
	CRMP			m_Rmp3;
	CString			m_strRmpSoft;
	HWND			m_hwndRiffSIF;
	HPROPSHEETPAGE	m_hpageRiffSIF;
	BOOL			m_bRiffApply;	//保存が必要なときにTRUE
	BOOL			m_bRiffSifDisable;

	//===APE Tag
//	CTag_Ape		m_Ape;
	HWND			m_hwndMp3Ape;
	HPROPSHEETPAGE	m_hpageMp3Ape;
	BOOL			m_bMp3ApeApply;	//保存が必要なときにTRUE
	BOOL			m_bMp3ApeTagDisable;

	//WAVE=====================================================
	CRiffSIF		m_RiffSIF;
	BOOL			m_bWavePropEnable;
	BOOL			m_bWaveInfotipEnable;
	BOOL			m_bWaveColumnEnable;
	CString			m_strWaveInfoTipFormat;//Infotipの書式
	int				m_iWaveCodecFind;	// コーデック名称取得方法

	//AVI=====================================================
	CTag_OpenDML	m_OpenDML;
	BOOL			m_bAviPropEnable;
	BOOL			m_bAviInfotipEnable;
	BOOL			m_bAviColumnEnable;
	CString			m_strAviInfoTipFormat;//Infotipの書式
	int				m_iAviCodecFind;	// コーデック名称取得方法
	
	//VQF=====================================================
	CVqf			m_Vqf;
	BOOL			m_bVqfPropEnable;
	BOOL			m_bVqfInfotipEnable;
	BOOL			m_bVqfColumnEnable;
	CString			m_strVqfInfoTipFormat;//Infotipの書式
	
	//WMA=====================================================
	CTag_Wma		m_Wma;
	BOOL			m_bWmaPropEnable;
	BOOL			m_bWmaInfotipEnable;
	BOOL			m_bWmaColumnEnable;
	CString			m_strWmaInfoTipFormat;//Infotipの書式
	
	//M3U=====================================================
	CM3u			m_M3u;
	BOOL			m_bM3uPropEnable;
	BOOL			m_bM3uInfotipEnable;
//	BOOL			m_bM3uColumnEnable;
	BOOL			m_bM3uProp_Fullpath;

	//OGG=====================================================
	CTag_Ogg		m_Ogg;
	BOOL			m_bOggPropEnable;
	BOOL			m_bOggInfotipEnable;
	BOOL			m_bOggColumnEnable;
	BOOL			m_bOggProp_Fullpath;
	CString			m_strOggInfoTipFormat;//Infotipの書式

	//APE=====================================================
	CInfo_MonkeysAudio m_Monkeys;
	CTag_Ape		m_Ape;
	BOOL			m_bApePropEnable;
	BOOL			m_bApeInfotipEnable;
	BOOL			m_bApeColumnEnable;
	BOOL			m_bApeProp_Fullpath;
	CString			m_strApeInfoTipFormat;//Infotipの書式
	
	//MP4=====================================================
	CTag_Mp4		m_Mp4;
	BOOL			m_bMp4PropEnable;
	BOOL			m_bMp4InfotipEnable;
	BOOL			m_bMp4ColumnEnable;
	CString			m_strMp4InfoTipFormat;//Infotipの書式

	HWND			m_hwndPage;
	BOOL			m_bApply;	//保存が必要なときにTRUE

public:
	CShellExt();
	~CShellExt();

	// mp3infp固有
	int m_tmpPropCount;
	HPROPSHEETPAGE	m_tmpProp[100];	// viewProp用のテンポラリ
	void Initialize(LPCTSTR szFileName);
	static BOOL CALLBACK AddPropSheetPageProc(HPROPSHEETPAGE hpage,LPARAM lParam);

	//IUnknown members
	STDMETHODIMP			QueryInterface(REFIID, LPVOID FAR *);
	STDMETHODIMP_(ULONG)	AddRef();
	STDMETHODIMP_(ULONG)	Release();

	//IShellExtInit methods
	STDMETHODIMP			Initialize(LPCITEMIDLIST pIDFolder, 
									   LPDATAOBJECT pDataObj, 
									   HKEY hKeyID);

	//IContextMenu methods
	STDMETHODIMP			QueryContextMenu(HMENU hMenu,
											 UINT indexMenu, 
											 UINT idCmdFirst,
											 UINT idCmdLast, 
											 UINT uFlags);
	STDMETHODIMP			InvokeCommand(LPCMINVOKECOMMANDINFO lpcmi);
	STDMETHODIMP			GetCommandString(UINT_PTR idCmd, 
											 UINT uFlags, 
											 UINT FAR *reserved, 
											 LPSTR pszName, 
											 UINT cchMax);
    
	//IShellPropSheetExt methods
	STDMETHODIMP			AddPages(LPFNADDPROPSHEETPAGE lpfnAddPage,
										LPARAM lParam);
	STDMETHODIMP			ReplacePage(UINT uPageID,
										 LPFNADDPROPSHEETPAGE lpfnReplaceWith, 
										 LPARAM lParam);
	
	//IQueryInfo
	STDMETHODIMP			GetInfoTip(DWORD dwFlags, WCHAR **ppwszTip);
	STDMETHODIMP			GetInfoFlags(LPDWORD pdwFlags);

	//IPersistFile
	STDMETHODIMP			IsDirty();
	STDMETHODIMP			Load(LPCOLESTR pszFileName, DWORD dwMode);
	STDMETHODIMP			Save(LPCOLESTR pszFileName, BOOL fMember);
	STDMETHODIMP			SaveCompleted(LPCOLESTR);
	STDMETHODIMP			GetCurFile(LPOLESTR *ppszFileName);

	//IPersist
	STDMETHODIMP			GetClassID(CLSID *pClassID);

	//IColumnProvider
	STDMETHODIMP			Initialize(LPCSHCOLUMNINIT psci);
	STDMETHODIMP			GetColumnInfo(DWORD dwIndex,SHCOLUMNINFO *psci);
	STDMETHODIMP			GetItemData(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData);

public:
	static EN_FILETYPE		GetFileType(LPCTSTR szFileName);
private:
	BOOL Load();
	void OpenHtmlHelp(HWND hWnd,LPCTSTR szViewFile);
	BOOL PushTimeStamp(LPCTSTR szFile){return m_timestamp.Push(szFile);};
	BOOL PopTimeStamp(LPCTSTR szFile){return m_timestamp.Pop(szFile);};
	void ConfigLoad();
	//IShellPropSheetExt methods
	friend void EnableButton(HWND hDlg,CShellExt *lpcs);
	friend void EnableEdit(HWND hDlg,CShellExt *lpcs,BOOL bEnable);
	friend void DispInfo(HWND hDlg,CShellExt *lpcs);
	friend void DispInfoExt(HWND hDlg,CShellExt *lpcs);
	static void				DrawControl(HWND hWnd,LPDRAWITEMSTRUCT lpInfo);

	void					AddPages_mp3(LPFNADDPROPSHEETPAGE lpfnAddPage,LPARAM lParam);
	void					AddPages_wave(LPFNADDPROPSHEETPAGE lpfnAddPage,LPARAM lParam);
	void					AddPages_avi(LPFNADDPROPSHEETPAGE lpfnAddPage,LPARAM lParam);
	void					AddPages_vqf(LPFNADDPROPSHEETPAGE lpfnAddPage,LPARAM lParam);
	void					AddPages_wma(LPFNADDPROPSHEETPAGE lpfnAddPage,LPARAM lParam);
	void					AddPages_m3u(LPFNADDPROPSHEETPAGE lpfnAddPage,LPARAM lParam);
	void					AddPages_ogg(LPFNADDPROPSHEETPAGE lpfnAddPage,LPARAM lParam);
	void					AddPages_ape(LPFNADDPROPSHEETPAGE lpfnAddPage,LPARAM lParam);
	void					AddPages_mp4(LPFNADDPROPSHEETPAGE lpfnAddPage,LPARAM lParam);
	
	static UINT CALLBACK	PageCallback_mp3_ID3V1(HWND hWnd,UINT uMessage,LPPROPSHEETPAGE ppsp);
	static BOOL CALLBACK	PageDlgProc_mp3_ID3V1(
										HWND hDlg,
										UINT uMessage,
										WPARAM wParam,
										LPARAM lParam);
	static UINT CALLBACK	PageCallback_mp3_ID3V2(
										HWND hWnd,
										UINT uMessage,
										LPPROPSHEETPAGE ppsp);
	static BOOL CALLBACK	PageDlgProc_mp3_ID3V2(
										HWND hDlg,
										UINT uMessage,
										WPARAM wParam,
										LPARAM lParam);
	static UINT CALLBACK	PageCallback_mp3_RMP(
										HWND hWnd,
										UINT uMessage,
										LPPROPSHEETPAGE ppsp);
	static BOOL CALLBACK	PageDlgProc_mp3_RMP(
										HWND hDlg,
										UINT uMessage,
										WPARAM wParam,
										LPARAM lParam);
	static UINT CALLBACK	PageCallback_mp3_LAME(
										HWND hWnd,
										UINT uMessage,
										LPPROPSHEETPAGE ppsp);
	static BOOL CALLBACK	PageDlgProc_mp3_LAME(
										HWND hDlg,
										UINT uMessage,
										WPARAM wParam,
										LPARAM lParam);
	static UINT CALLBACK	PageCallback_mp3_APE(
										HWND hWnd,
										UINT uMessage,
										LPPROPSHEETPAGE ppsp);
	static BOOL CALLBACK	PageDlgProc_mp3_APE(
										HWND hDlg,
										UINT uMessage,
										WPARAM wParam,
										LPARAM lParam);
	static UINT CALLBACK	PageCallback_wave(
										HWND hWnd,
										UINT uMessage,
										LPPROPSHEETPAGE ppsp);
	static BOOL CALLBACK	PageDlgProc_wave(
										HWND hDlg,
										UINT uMessage,
										WPARAM wParam,
										LPARAM lParam);
	static void				DispInfo_wave(HWND hDlg,CShellExt *lpcs);
	static UINT CALLBACK	PageCallback_avi(
										HWND hWnd,
										UINT uMessage,
										LPPROPSHEETPAGE ppsp);
	static BOOL CALLBACK	PageDlgProc_avi(
										HWND hDlg,
										UINT uMessage,
										WPARAM wParam,
										LPARAM lParam);
	static void				DispInfo_avi(HWND hDlg,CShellExt *lpcs);
	static UINT CALLBACK	PageCallback_vqf(
										HWND hWnd,
										UINT uMessage,
										LPPROPSHEETPAGE ppsp);
	static BOOL CALLBACK	PageDlgProc_vqf(
										HWND hDlg,
										UINT uMessage,
										WPARAM wParam,
										LPARAM lParam);
	static void				DispInfo_vqf(HWND hDlg,CShellExt *lpcs);

	static UINT CALLBACK	PageCallback_wma(
										HWND hWnd,
										UINT uMessage,
										LPPROPSHEETPAGE ppsp);
	static BOOL CALLBACK	PageDlgProc_wma(
										HWND hDlg,
										UINT uMessage,
										WPARAM wParam,
										LPARAM lParam);
	static void				DispInfo_wma(HWND hDlg,CShellExt *lpcs);
	
	static UINT CALLBACK	PageCallback_m3u(
										HWND hWnd,
										UINT uMessage,
										LPPROPSHEETPAGE ppsp);
	static BOOL CALLBACK	PageDlgProc_m3u(
										HWND hDlg,
										UINT uMessage,
										WPARAM wParam,
										LPARAM lParam);
	static void				DispInfo_m3u(HWND hDlg,CShellExt *lpcs);

	static UINT CALLBACK	PageCallback_ogg(
										HWND hWnd,
										UINT uMessage,
										LPPROPSHEETPAGE ppsp);
	static BOOL CALLBACK	PageDlgProc_ogg(
										HWND hDlg,
										UINT uMessage,
										WPARAM wParam,
										LPARAM lParam);
	static void				DispInfo_ogg(HWND hDlg,CShellExt *lpcs);
	
	static UINT CALLBACK	PageCallback_ape(
										HWND hWnd,
										UINT uMessage,
										LPPROPSHEETPAGE ppsp);
	static BOOL CALLBACK	PageDlgProc_ape(
										HWND hDlg,
										UINT uMessage,
										WPARAM wParam,
										LPARAM lParam);
	static void				DispInfo_ape(HWND hDlg,CShellExt *lpcs);

	static UINT CALLBACK	PageCallback_mp4(
										HWND hWnd,
										UINT uMessage,
										LPPROPSHEETPAGE ppsp);
	static BOOL CALLBACK	PageDlgProc_mp4(
										HWND hDlg,
										UINT uMessage,
										WPARAM wParam,
										LPARAM lParam);
	static void				DispInfo_mp4(HWND hDlg,CShellExt *lpcs);
	
	//IQueryInfo
	CString					GetInfoTip_mp3();
	CString					GetInfoTip_wave();
	CString					GetInfoTip_avi();
	CString					GetInfoTip_vqf();
	CString					GetInfoTip_wma();
	CString					GetInfoTip_m3u();
	CString					GetInfoTip_ogg();
	CString					GetInfoTip_ape();
	CString					GetInfoTip_mp4();

	//IColumnProvider
	BOOL					GetItemData_all(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData);
	BOOL					GetItemData_mp3(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData);
	BOOL					GetItemData_wave(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData);
	BOOL					GetItemData_avi(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData);
	BOOL					GetItemData_vqf(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData);
	BOOL					GetItemData_wma(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData);
	BOOL					GetItemData_ogg(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData);
	BOOL					GetItemData_ape(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData);
	BOOL					GetItemData_mp4(LPCSHCOLUMNID pscid,LPCSHCOLUMNDATA pscd,VARIANT *pvarData);
};
typedef CShellExt *LPCSHELLEXT;

#endif
