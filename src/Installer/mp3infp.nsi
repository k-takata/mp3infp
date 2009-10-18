; languages.nsi
;
; This is an example of a multilingual installer
; The user can select the language on startup

;--------------------------------
!packhdr "$%TEMP%\exehead.tmp" '"upx.exe" --best "$%TEMP%\exehead.tmp"'

!define MUI_COMPANY "win32lab.com" ;Define your own software name here
!define MUI_PRODUCT "mp3infp" ;Define your own software name here
;!define MUI_VERSION "x.xx" ;Define your own software version here
;OutFile mp3infpxxx.exe

;Var PRGMMAN_HWND

InstallDir "$PROGRAMFILES\${MUI_PRODUCT}"
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

	SetOutPath "$INSTDIR"
	${If} ${RunningX64}
		File /oname=mp3infp_regist_x64.exe "..\x64\mp3infp_regist.exe"
	${EndIf}
	File "..\x86\mp3infp_regist.exe"
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
;		File /oname=$0 "..\x64\Japanese.lng"
;	${Else}
		File /oname=$0 "..\x86\Japanese.lng"
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
	IfFileExists "$SYSDIR\$0" 0 dllCpy
	StrCpy "$1" "0"
	dllExists:
		IntOp "$1" "$1" + "1"
		StrCpy "$0" "mp3infp.$1"
		IfFileExists "$0" dllExists
	dllCpy:
	File /oname=$0 "..\x86\mp3infp.dll"
	Rename /REBOOTOK "$SYSDIR\$0" "$SYSDIR\mp3infp.dll"
	IfRebootFlag YesReboot 0
		RegDLL "mp3infp.dll"
	YesReboot:

	${If} ${RunningX64}
	${DisableX64FSRedirection}
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
	${EnableX64FSRedirection}
	${Else}
	StrCpy "$0" "mp3infp.cpl"
	Delete "$SYSDIR\$0"
	IfFileExists "$0" 0 cplCpy
	StrCpy "$1" "0"
	cplExists:
		IntOp "$1" "$1" + "1"
		StrCpy "$0" "mp3infc.$1"
		IfFileExists "$0" cplExists
	cplCpy:
	File /oname=$0 "..\x86\mp3infp.cpl"
	Rename /REBOOTOK "$SYSDIR\$0" "$SYSDIR\mp3infp.cpl"
	${EndIf}

	;Language set
	WriteRegStr HKCU "Software\${MUI_COMPANY}\${MUI_PRODUCT}" "Language" "$(LangRegKey)"

	${If} ${RunningX64}
		SetRegView 64
		;[1] mp3infp_regist.exe
		DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "mp3infp"
		IfRebootFlag 0 +2
			WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\RunOnce" "mp3infp" "$\"$INSTDIR\mp3infp_regist_x64.exe$\""
	
		;[2] Uninstall list
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mp3infp" "DisplayIcon" "$\"$INSTDIR\mp3infp_regist_x64.exe$\",0"
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mp3infp" "DisplayName" "${MUI_PRODUCT} ${MUI_VERSION}"
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mp3infp" "UninstallString" '"$INSTDIR\uninstall.exe"'
	
		;[3] Store install folder
		WriteRegStr HKLM "Software\${MUI_COMPANY}\${MUI_PRODUCT}" "path" $INSTDIR
		SetRegView 32
	${Else}
		;[2] Uninstall list
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mp3infp" "DisplayIcon" "$\"$INSTDIR\mp3infp_regist.exe$\",0"
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mp3infp" "DisplayName" "${MUI_PRODUCT} ${MUI_VERSION}"
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mp3infp" "UninstallString" '"$INSTDIR\uninstall.exe"'
	${EndIf}

	;[1] mp3infp_regist.exe
	DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "mp3infp"
	IfRebootFlag 0 +2
		WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\RunOnce" "mp3infp" "$\"$INSTDIR\mp3infp_regist.exe$\""

	;[3] Store install folder
	WriteRegStr HKLM "Software\${MUI_COMPANY}\${MUI_PRODUCT}" "path" $INSTDIR

	WriteUninstaller "$INSTDIR\uninstall.exe"

SectionEnd
  
;--------------------------------

Function .onInit

	${If} ${RunningX64}
		SetRegView 64
		ReadRegStr $INSTDIR HKLM "Software\${MUI_COMPANY}\${MUI_PRODUCT}" "path"
		StrCmp $INSTDIR "" 0 regok64
		StrCpy $INSTDIR "$PROGRAMFILES64\${MUI_PRODUCT}"
		regok64:
  		SetRegView 32
	${Else}
		ReadRegStr $INSTDIR HKLM "Software\${MUI_COMPANY}\${MUI_PRODUCT}" "path"
		StrCmp $INSTDIR "" 0 regok
		StrCpy $INSTDIR "$PROGRAMFILES\${MUI_PRODUCT}"
		regok:
	${EndIf}

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

  ;ADD YOUR OWN STUFF HERE!

	${If} ${RunningX64}
		${DisableX64FSRedirection}
		ExecWait '"$SYSDIR\regsvr32.exe" /s /u mp3infp.dll'
		Delete /REBOOTOK "$SYSDIR\mp3infp.dll"
		Delete /REBOOTOK "$SYSDIR\mp3infp.cpl"
		Delete /REBOOTOK "$INSTDIR\mp3infp_regist_x64.exe"
		${EnableX64FSRedirection}
	${Else}
		UnRegDLL "mp3infp.dll"
		Delete /REBOOTOK "$SYSDIR\mp3infp.cpl"
	${EndIf}
	Delete /REBOOTOK "$SYSDIR\mp3infp.dll"

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

	DeleteRegKey HKCU "Software\${MUI_COMPANY}\${MUI_PRODUCT}"
	DeleteRegKey /ifempty HKCU "Software\${MUI_COMPANY}"

	${If} ${RunningX64}
		SetRegView 64
		; [1]
		DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "mp3infp"
		; [2]
		DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mp3infp"
		; [3]
		DeleteRegKey HKLM "Software\${MUI_COMPANY}\${MUI_PRODUCT}"
		DeleteRegKey /ifempty HKLM "Software\${MUI_COMPANY}"

		DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\SharedDLLs" "$SYSDIR\mp3infp.dll"
		SetRegView 32
	${Else}
		; [2]
		DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\mp3infp"
	${EndIf}

	; [1]
	DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Run" "mp3infp"
	; [3]
	DeleteRegKey HKLM "Software\${MUI_COMPANY}\${MUI_PRODUCT}"
	DeleteRegKey /ifempty HKLM "Software\${MUI_COMPANY}"

	DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\SharedDLLs" "$SYSDIR\mp3infp.dll"
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
	;Get language from registry
	ReadRegStr $LANGUAGE HKCU "Software\${MUI_COMPANY}\${MUI_PRODUCT}" "Installer Language"
	StrCmp $LANGUAGE "" 0 regok
	StrCpy $LANGUAGE "English"
	regok:
FunctionEnd
