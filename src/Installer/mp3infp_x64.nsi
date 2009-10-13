; languages.nsi
;
; This is an example of a multilingual installer
; The user can select the language on startup

;--------------------------------
!packhdr "$%TEMP%\exehead.tmp" '"upx.exe" -9 "$%TEMP%\exehead.tmp"'

!define MUI_COMPANY "win32lab.com" ;Define your own software name here
!define MUI_PRODUCT "mp3infp" ;Define your own software name here
!define MUI_VERSION "2.54g" ;Define your own software version here
OutFile mp3infp254g_u3_x64.exe

;Var PRGMMAN_HWND

InstallDir "$PROGRAMFILES64\${MUI_PRODUCT}"
SetCompressor /SOLID lzma
XPStyle on

;--------------------------------

; First is default
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Japanese.nlf"
;LoadLanguageFile "${NSISDIR}\Contrib\Language files\TradChinese.nlf"
;LoadLanguageFile "${NSISDIR}\Contrib\Language files\SimpChinese.nlf"

; Set name using the normal interface (Name command)
LangString Name ${LANG_ENGLISH} "${MUI_PRODUCT} v${MUI_VERSION}"
LangString Name ${LANG_JAPANESE} "${MUI_PRODUCT} v${MUI_VERSION}"
;LangString Name ${LANG_TRADCHINESE} "${MUI_PRODUCT} v${MUI_VERSION}"
;LangString Name ${LANG_SIMPCHINESE} "${MUI_PRODUCT} v${MUI_VERSION}"

LangString MesReboot ${LANG_ENGLISH} "The reboot of PC is required to complete installation. Does it reboot immediately now?"
LangString MesRebootU ${LANG_ENGLISH} "The reboot of PC is required to complete uninstallation. Does it reboot immediately now?"
LangString LangRegKey ${LANG_ENGLISH} "default"

;LangString MesReboot ${LANG_SIMPCHINESE} "The reboot of PC is required to complete installation. Does it reboot immediately now?"
;LangString MesRebootU ${LANG_SIMPCHINESE} "The reboot of PC is required to complete uninstallation. Does it reboot immediately now?"
;LangString LangRegKey ${LANG_SIMPCHINESE} "default"

;!include "..\lang\Chinese_Traditional.nsh"
;!include "..\lang\Chinese_Simplified.nsh"
!include "Japanese.nsh"
!include "x64.nsh"


Name $(Name)

; Set one text for all languages (simply don't use a LangString)
CompletedText "mp3infp install completed"

;--------------------------------
;LangString ^LICENSEDATA ${LANG_ENGLISH} "..\mp3infp_eng.txt"
;LangString ^LICENSEDATA ${LANG_JAPANESE} "..\mp3infp.txt"
LicenseLangString SLICENSEFILE  ${LANG_ENGLISH} "..\mp3infp_eng.txt"
LicenseLangString SLICENSEFILE  ${LANG_JAPANESE} "..\mp3infp.txt"
;LicenseLangString SLICENSEFILE  ${LANG_TRADCHINESE} "..\lang\Chinese_Traditional.txt"
;LicenseLangString SLICENSEFILE  ${LANG_SIMPCHINESE} "..\lang\Chinese_Simplified.txt"
LicenseData $(SLICENSEFILE)

;--------------------------------

Page license
;Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

;--------------------------------
;Installer Sections
Section "main files" SecCopyUI
	${DisableX64FSRedirection}
	SetRegView 64

	SetOutPath "$INSTDIR"
;	${If} ${RunningX64}
		File "..\x64\mp3infp_regist.exe"
;	${Else}
;		File "..\x86\mp3infp_regist.exe"
;	${EndIf}
	File "..\mp3infp_eng.txt"
	File "..\mp3infp.txt"
	CreateDirectory "$INSTDIR\language"
	SetOutPath "$INSTDIR\language"
	
	; japanese
	StrCpy "$0" "Japanese.chm"
	Delete "$INSTDIR\language\$0"
	IfFileExists "$0" 0 jp_chmCpy
	StrCpy "$1" "0"
	jp_chmExists:
		IntOp "$1" "$1" + "1"
		StrCpy "$0" "Japanese.chm.$1"
		IfFileExists "$0" jp_chmExists
	jp_chmCpy:
	File /oname=$0 "..\Installer\Japanese.chm"
	Rename /REBOOTOK "$INSTDIR\language\$0" "$INSTDIR\language\Japanese.chm"
	
	StrCpy "$0" "Japanese.lng"
	Delete "$INSTDIR\language\$0"
	IfFileExists "$0" 0 jpnCpy
	StrCpy "$1" "0"
	jpnExists:
		IntOp "$1" "$1" + "1"
		StrCpy "$0" "Japanese.lng.$1"
		IfFileExists "$0" jpnExists
	jpnCpy:
;	${If} ${RunningX64}
		File /oname=$0 "..\x64\Japanese.lng"
;	${Else}
;		File /oname=$0 "..\x86\Japanese.lng"
;	${EndIf}
	Rename /REBOOTOK "$INSTDIR\language\$0" "$INSTDIR\language\Japanese.lng"
	
	; ChineseTraditional
;	StrCpy "$0" "Chinese_Traditional.txt"
;	Delete "$INSTDIR\language\$0"
;	IfFileExists "$0" 0 cht_chmCpy
;	StrCpy "$1" "0"
;	cht_chmExists:
;		IntOp "$1" "$1" + "1"
;		StrCpy "$0" "Chinese_Traditional.txt.$1"
;		IfFileExists "$0" cht_chmExists
;	cht_chmCpy:
;	File /oname=$0 "..\lang\Chinese_Traditional.txt"
;	Rename /REBOOTOK "$INSTDIR\language\$0" "$INSTDIR\language\Chinese_Traditional.txt"
	
;	StrCpy "$0" "Chinese_Traditional.lng"
;	Delete "$INSTDIR\language\$0"
;	IfFileExists "$0" 0 chtCpy
;	StrCpy "$1" "0"
;	chtExists:
;		IntOp "$1" "$1" + "1"
;		StrCpy "$0" "Chinese_Traditional.lng.$1"
;		IfFileExists "$0" chtExists
;	chtCpy:
;	File /oname=$0 "..\lang\Chinese_Traditional.lng"
;	Rename /REBOOTOK "$INSTDIR\language\$0" "$INSTDIR\language\Chinese_Traditional.lng"
	
	; ChineseSimplified
;	StrCpy "$0" "Chinese_Simplified.txt"
;	Delete "$INSTDIR\language\$0"
;	IfFileExists "$0" 0 cht_chmCpy
;	StrCpy "$1" "0"
;	cht_chmExists:
;		IntOp "$1" "$1" + "1"
;		StrCpy "$0" "Chinese_Simplified.txt.$1"
;		IfFileExists "$0" cht_chmExists
;	cht_chmCpy:
;	File /oname=$0 "..\lang\Chinese_Simplified.txt"
;	Rename /REBOOTOK "$INSTDIR\language\$0" "$INSTDIR\language\Chinese_Simplified.txt"
	
;	StrCpy "$0" "Chinese_Simplified.lng"
;	Delete "$INSTDIR\language\$0"
;	IfFileExists "$0" 0 chtCpy
;	StrCpy "$1" "0"
;	chtExists:
;		IntOp "$1" "$1" + "1"
;		StrCpy "$0" "Chinese_Simplified.lng.$1"
;		IfFileExists "$0" chtExists
;	chtCpy:
;	File /oname=$0 "..\lang\Chinese_Simplified.lng"
;	Rename /REBOOTOK "$INSTDIR\language\$0" "$INSTDIR\language\Chinese_Simplified.lng"
	
	;----------------------------------------------------------------
	SetOutPath "$SYSDIR"

	StrCpy "$0" "mp3infp.dll"
	Delete "$SYSDIR\$0"
	IfFileExists "$SYSDIR\$0" 0 dllCpy64
	StrCpy "$1" "0"
	dllExists64:
		IntOp "$1" "$1" + "1"
		StrCpy "$0" "mp3infp.$1"
		IfFileExists "$0" dllExists64
	dllCpy64:
	File /oname=$0 "..\x64\mp3infp.dll"
	Rename /REBOOTOK "$SYSDIR\$0" "$SYSDIR\mp3infp.dll"
	IfRebootFlag YesReboot64 0
		ExecWait '"$SYSDIR\regsvr32.exe" /s mp3infp.dll'
	YesReboot64:
	
	StrCpy "$0" "mp3infp.cpl"
	Delete "$SYSDIR\$0"
	IfFileExists "$0" 0 cplCpy64
	StrCpy "$1" "0"
	cplExists64:
		IntOp "$1" "$1" + "1"
		StrCpy "$0" "mp3infc.$1"
		IfFileExists "$0" cplExists64
	cplCpy64:
	File /oname=$0 "..\x64\mp3infp.cpl"
	Rename /REBOOTOK "$SYSDIR\$0" "$SYSDIR\mp3infp.cpl"

	;Language set
	WriteRegStr HKCU "Software\${MUI_COMPANY}\${MUI_PRODUCT}" "Language" "$(LangRegKey)"
	
	;[1] mp3infp_regist.exe
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "mp3infp" "$\"$INSTDIR\mp3infp_regist.exe$\""
	
	;[2] Uninstall list
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mp3infp" "DisplayIcon" "$\"$INSTDIR\mp3infp_regist.exe$\",0"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mp3infp" "DisplayName" "${MUI_PRODUCT} ${MUI_VERSION} (x64)"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mp3infp" "UninstallString" '"$INSTDIR\uninstall.exe"'
	
	;[3] Store install folder
	WriteRegStr HKLM "Software\${MUI_COMPANY}\${MUI_PRODUCT}" "path" $INSTDIR
	
	WriteUninstaller "$INSTDIR\Uninstall.exe"

	SetRegView 32
	${EnableX64FSRedirection}
SectionEnd
  
;--------------------------------

Function .onInit

	${Unless} ${RunningX64}
		MessageBox MB_OK "Cannot install x64 on x86 machine"
		Abort
	${EndIf}

	SetRegView 64
	ReadRegStr $INSTDIR HKLM "Software\${MUI_COMPANY}\${MUI_PRODUCT}" "path"
	StrCmp $INSTDIR "" 0 regok
	StrCpy $INSTDIR "$PROGRAMFILES64\${MUI_PRODUCT}"
	regok:
  	SetRegView 32

	;Language selection dialog

	IfSilent Silent 0
	Push ""
	Push ${LANG_ENGLISH}
	Push "English"
	Push ${LANG_JAPANESE}
	Push "Japanese"
;	Push ${LANG_TRADCHINESE}
;	Push "Chinese Traditional"
;	Push ${LANG_SIMPCHINESE}
;	Push "Chinese Simplified"
	Push A ; A means auto count languages
	       ; for the auto count to work the first empty push (Push "") must remain
	LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

	Pop $LANGUAGE
	
	StrCmp $LANGUAGE "cancel" 0 +2
		Abort
	Silent:
FunctionEnd

Function .onInstSuccess
	IfRebootFlag 0 NoReboot
		MessageBox MB_YESNO|MB_ICONQUESTION \
			$(MesReboot) \
			IDNO NoReboot
			Reboot
	NoReboot:
FunctionEnd


;--------------------------------
;Uninstaller Section

Section "Uninstall"
	${DisableX64FSRedirection}
	SetRegView 64

	;ADD YOUR OWN STUFF HERE!

	ExecWait '"$SYSDIR\regsvr32.exe" /s /u mp3infp.dll'
	Delete /REBOOTOK "$SYSDIR\mp3infp.dll"
	Delete /REBOOTOK "$SYSDIR\mp3infp.cpl"

	Delete /REBOOTOK "$INSTDIR\mp3infp_regist.exe"
	Delete /REBOOTOK "$INSTDIR\mp3infp.txt"
	Delete /REBOOTOK "$INSTDIR\mp3infp_eng.txt"
	Delete /REBOOTOK "$INSTDIR\language\Japanese.chm"
	Delete /REBOOTOK "$INSTDIR\language\Japanese.lng"
;	Delete /REBOOTOK "$INSTDIR\language\Chinese_Simplified.txt"
;	Delete /REBOOTOK "$INSTDIR\language\Chinese_Simplified.lng"
	Delete /REBOOTOK "$INSTDIR\Uninstall.exe"

	RMDir "$INSTDIR\language"
	RMDir "$INSTDIR"

	; [1]
	DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "mp3infp"
	; [2]
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mp3infp"
	; [3]
	DeleteRegKey HKLM "Software\${MUI_COMPANY}\${MUI_PRODUCT}"
	DeleteRegKey /ifempty HKLM "Software\${MUI_COMPANY}"
	
	DeleteRegKey HKCU "Software\${MUI_COMPANY}\${MUI_PRODUCT}"
	DeleteRegKey /ifempty HKCU "Software\${MUI_COMPANY}"
	DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\SharedDLLs" "$SYSDIR\mp3infp.dll"

	SetRegView 32
	${EnableX64FSRedirection}
SectionEnd

Function un.onUninstSuccess
	IfRebootFlag 0 NoReboot
		MessageBox MB_YESNO|MB_ICONQUESTION \
			$(MesRebootU) \
			IDNO NoReboot
			Reboot
	NoReboot:
FunctionEnd

;--------------------------------
;Uninstaller Functions

Function un.onInit
	SetRegView 64
	;Get language from registry
	ReadRegStr $LANGUAGE HKCU "Software\${MUI_COMPANY}\${MUI_PRODUCT}" "Installer Language"
	StrCmp $LANGUAGE "" 0 regok
	StrCpy $LANGUAGE "English"
	regok:
  	SetRegView 32
FunctionEnd
