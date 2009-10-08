// RMP.h: CRMP クラスのインターフェイス
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RMP_H__81689DF7_1DE7_47C6_A34A_41AFEECC3184__INCLUDED_)
#define AFX_RMP_H__81689DF7_1DE7_47C6_A34A_41AFEECC3184__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <Mmsystem.h>

class CRMP  
{
public:
			CRMP(BOOL bScmpxGenre = FALSE);
	virtual ~CRMP();
	void	Release();
	void	SetScmpxGenre(BOOL bSwitch);
	void	SetSftDefault(const char *szDefaultSoft);
	BOOL	IsEnable(){return m_bEnable;};
	BOOL	HasId3tag(){return m_bHasId3tag;};
	void	SetHasId3tag(BOOL bHasId3tag){m_bHasId3tag = bHasId3tag;};

	CString GetNAM(){return m_strNAM;};
	void	SetNAM(const char *str){m_strNAM = str;m_bEnable = TRUE;};
	CString GetART(){return m_strART;};
	void	SetART(const char *str){m_strART = str;m_bEnable = TRUE;};
	CString GetPRD(){return m_strPRD;};
	void	SetPRD(const char *str){m_strPRD = str;m_bEnable = TRUE;};
	CString GetCMT(){return m_strCMT;};
	void	SetCMT(const char *str){m_strCMT = str;m_bEnable = TRUE;};
	CString GetCRD(){return m_strCRD;};
	void	SetCRD(const char *str){m_strCRD = str;m_bEnable = TRUE;};
	CString GetGNR(){return m_strGNR;};
	void	SetGNR(const char *str){m_strGNR = str;m_bEnable = TRUE;};
	CString GetCOP(){return m_strCOP;};
	void	SetCOP(const char *str){m_strCOP = str;m_bEnable = TRUE;};
	CString GetENG(){return m_strENG;};
	void	SetENG(const char *str){m_strENG = str;m_bEnable = TRUE;};
	CString GetSRC(){return m_strSRC;};
	void	SetSRC(const char *str){m_strSRC = str;m_bEnable = TRUE;};
	CString GetSFT(){return m_strSFT;};
	void	SetSFT(const char *str){m_strSFT = str;m_bEnable = TRUE;};
	CString GetKEY(){return m_strKEY;};
	void	SetKEY(const char *str){m_strKEY = str;m_bEnable = TRUE;};
	CString	GetTCH(){return m_strTCH;};
	void	SetTCH(const char *str){m_strTCH = str;m_bEnable = TRUE;};
	CString GetLYC(){return m_strLYC;};
	void	SetLYC(const char *str){m_strLYC = str;m_bEnable = TRUE;};
	CString GetCMS(){return m_strCMS;};
	void	SetCMS(const char *str){m_strCMS = str;m_bEnable = TRUE;};
	CString GetMED(){return m_strMED;};
	void	SetMED(const char *str){m_strMED = str;m_bEnable = TRUE;};
	CString GetSBJ(){return m_strSBJ;};
	void	SetSBJ(const char *str){m_strSBJ = str;m_bEnable = TRUE;};
	CString GetMP3(){return m_strMP3;};
	void	SetMP3(const char *str){m_strMP3 = str;m_bEnable = TRUE;};
	CString ReadChunk(HMMIO hmmio,MMCKINFO mmckinfo,FOURCC id);
	BOOL	WriteChunk(HMMIO hmmio,MMCKINFO mmckinfo,FOURCC id,const char *pData,DWORD dwSize);

	DWORD	Load(const char *szFileName);
	DWORD	Save(HWND hWnd,const char *szFileName);
	DWORD	DelTag(HWND hWnd,const char *szFileName);
	DWORD	MakeTag(HWND hWnd,const char *szFileName);

private:
	BOOL		m_bEnable;			//RIFFSIFが無い場合はFALSE
	BOOL		m_bHasId3tag;		//ID3TAGを持っている場合はTRUE
	BOOL		m_bScmpxGenre;		//SCMPX拡張ジャンルを使用する
	CString		m_strDefaultSft;	//ISFTのデフォルト値

	CString		m_strNAM;		//INAM songname
	CString		m_strART;		//IART アーティスト名
	CString		m_strPRD;		//IPRD アルバム名
	CString		m_strCMT;		//ICMT コメント
	CString		m_strCRD;		//ICRD 日付
	CString		m_strGNR;		//IGNR ジャンル

	CString		m_strCOP;		//ICOP 著作権
	CString		m_strENG;		//IENG エンジニア
	CString		m_strSRC;		//ISRC ソース
	CString		m_strSFT;		//ISFT ソフトウェア
	CString		m_strKEY;		//IKEY キーワード
	CString		m_strTCH;		//ITCH 技術者
	CString		m_strLYC;		//ILYC 歌詞
	CString		m_strCMS;		//ICMS コミッション
	CString		m_strMED;		//IMED 中間
	CString		m_strSBJ;		//ISBJ subject
	CString		m_strMP3;		//IMP3 mp3 info
};

#endif // !defined(AFX_RMP_H__81689DF7_1DE7_47C6_A34A_41AFEECC3184__INCLUDED_)
