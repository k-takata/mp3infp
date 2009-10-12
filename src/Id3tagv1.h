// Id3tagv1.h: CId3tagv1 �N���X�̃C���^�[�t�F�C�X
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ID3TAGV1_H__81689DF7_1DE7_47C6_A34A_41AFEECC3184__INCLUDED_)
#define AFX_ID3TAGV1_H__81689DF7_1DE7_47C6_A34A_41AFEECC3184__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CId3tagv1  
{
public:
	CId3tagv1(BOOL bScmpxGenre = FALSE);
	virtual ~CId3tagv1();
	void SetScmpxGenre(BOOL bSwitch);
	BOOL IsEnable(){return m_bEnable;};

	CString GetTitle();
	void SetTitle(LPCTSTR title);
	CString GetArtist();
	void SetArtist(LPCTSTR artist);
	CString GetAlbum();
	void SetAlbum(LPCTSTR album);
	CString GetYear();
	void SetYear(LPCTSTR year);
	unsigned char GetGenreNum();
	CString GetGenre();
	void SetGenre(unsigned char cGenre);
	void SetGenre(LPCTSTR szGenre);
	unsigned char GetTrackNoNum();
	CString GetTrackNo();
	void SetTrackNo(unsigned char cTrackNo);
	void SetTrackNo(LPCTSTR szTrackNo);
	CString GetComment();
	void SetComment(LPCTSTR comment);

	CString GenreNum2String(unsigned char cGenre);
	long GenreString2Num(LPCTSTR szGenre);

	DWORD Load(LPCTSTR szFileName);
	DWORD LoadMulti(LPCTSTR szFileName);
	DWORD Save(HWND hWnd,LPCTSTR szFileName);
	DWORD DelTag(HWND hWnd,LPCTSTR szFileName);
	void GetId3tagString(char *szTag);
	DWORD MakeTag(HWND hWnd,LPCTSTR szFileName);

private:
	void Release();
	BOOL m_bEnable;			//ID3TAG�������ꍇ��FALSE
	BOOL m_bScmpxGenre;		//SCMPX�g���W���������g�p����
	char m_szTitle[30+1];	//�^�C�g��
	char m_szArtist[30+1];	//�A�[�e�B�X�g
	char m_szAlbum[30+1];	//�A���o��
	char m_szYear[4+1];		//����
	unsigned char m_cGenre;	//�W������
	char m_szComment[30+1];	//�R�����g
	unsigned char m_cTrackNo;//�g���b�N�ԍ�(1�`255 0=N/A)

};

#endif // !defined(AFX_ID3TAGV1_H__81689DF7_1DE7_47C6_A34A_41AFEECC3184__INCLUDED_)
