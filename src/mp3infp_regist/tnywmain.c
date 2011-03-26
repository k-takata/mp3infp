/*
 *	tnywmain.c
 */

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOIME
#include <windows.h>
#include <tchar.h>

#if defined(USE_CMDSHOW) || defined(_WIN64)
#define NAKED
#else
// note: compile option /O1 or /O2 is needed
#define NAKED	__declspec(naked)
#endif

extern int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPSTR lpCmdLine, int nCmdShow);
extern int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
		LPWSTR lpCmdLine, int nCmdShow);

#ifdef UNICODE
#define _tWinMainCRTStartup wWinMainCRTStartup
#else
#define _tWinMainCRTStartup WinMainCRTStartup
#endif

NAKED void _tWinMainCRTStartup(void)
{
	LPTSTR cmdline;
	TBYTE *lpszCommandLine;	// must be unsigned for multibyte strings
	int cmdshow;
	
#ifdef USE_CMDSHOW
	STARTUPINFO si;
	
	GetStartupInfo(&si);
	cmdshow = (si.dwFlags & STARTF_USESHOWWINDOW)
			? si.wShowWindow : SW_SHOWDEFAULT;
#else
	cmdshow = 0;
#endif

#ifdef USE_CMDLINE
	lpszCommandLine = GetCommandLine();
	
	// Skip past program name (first token in command line)
	
	// Check for and handle quoted program name
	if (*lpszCommandLine == '"') {
		// Scan, and skip over, subsequent characters until another
		// double-quote or a null is encountered
		do {
			lpszCommandLine++;
		} while (*lpszCommandLine && (*lpszCommandLine != '"'));
		
		// If we stopped on a double-quote (usual case), skip over it.
		if (*lpszCommandLine == '"')
			lpszCommandLine++;
	} else {
		// First token wasn't a quote
		while (*lpszCommandLine > ' ')
			lpszCommandLine++;
	}
	
	// skip past any white space preceeding the second token.
	while (*lpszCommandLine && (*lpszCommandLine <= ' '))
		lpszCommandLine++;
	
	cmdline = lpszCommandLine;
#else
	cmdline = NULL;
#endif
	
	ExitProcess(_tWinMain(GetModuleHandle(NULL), NULL, cmdline, cmdshow));
}
