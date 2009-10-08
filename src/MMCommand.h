#ifndef _MMCOMMAND_H
#define _MMCOMMAND_H


BOOL findVideoCodecName1(CString &strVideoFormat,DWORD biCompression,DWORD fccHeader);
void GetWaveAudioFormat(IN const char *szFileName,
						IN DWORD dwStreamSize,
						OUT CString &strFormat,
						OUT CString &strTime,
						IN int iWaveCodecFind);
void GetAviFormat(IN const char *szFileName,
						OUT CString &strAudioFormat,
						OUT CString &strVideoFormat,
						OUT CString &strStreamFormat,
						OUT CString &strTime,
						OUT BOOL &bAvi2,
						IN int iAviCodecFind);

void PlayWinamp(HWND hWnd,char *szPlayFile);
void PauseWinamp();
void StopWinamp();
void Rew5Winamp();
void Ffw5Winamp();
int IsPlayingWinamp(char *szFileName);
int GetPosWinamp();
void SetPosWinamp(int pos);
CString GetPlayingFilenameWinamp();

#endif	//_MMCOMMAND_H