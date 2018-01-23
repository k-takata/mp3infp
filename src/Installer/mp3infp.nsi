; languages.nsi
;
; This is an example of a multilingual installer
; The user can select the language on startup

;--------------------------------
!packhdr "$%TEMP%\exehead.tmp" '"upx.exe" -9 "$%TEMP%\exehead.tmp"'

!define MUI_COMPANY "win32lab.com" ;Define your own software name here
!define MUI_PRODUCT "mp3infp" ;Define your own software name here
!define PUBLISHER "T-Matsuo, Rem and K.Takata"
;!define MUI_VERSION "2.54g/u7" ;Define your own software version here
;OutFile mp3infp254g_u7.exe

!define UNINST_REG_KEY	"Software\Microsoft\Windows\CurrentVersion\Uninstall\${MUI_PRODUCT}"
!define RUN_REG_KEY	"Software\Microsoft\Windows\CurrentVersion\Run"
!define RUNONCE_REG_KEY	"Software\Microsoft\Windows\CurrentVersion\RunOnce"
!define SHDLLS_REG_KEY	"Software\Microsoft\Windows\CurrentVersion\SharedDLLs"
!define COMPANY_REG_KEY	"Software\${MUI_COMPANY}"
!define PRODUCT_REG_KEY	"Software\${MUI_COMPANY}\${MUI_PRODUCT}"

;Var PRGMMAN_HWND

InstallDir "$PROGRAMFILES\${MUI_PRODUCT}"
SetCompressor /SOLID lzma
XPStyle on
RequestExecutionLevel admin

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
LangString MP3INFP_SETUP_NAME ${LANG_ENGLISH} "mp3infp setup"

;LangString MesReboot ${LANG_SIMPCHINESE} "The reboot of PC is required to complete installation. Does it reboot immediately now?"
;LangString MesRebootU ${LANG_SIMPCHINESE} "The reboot of PC is required to complete uninstallation. Does it reboot immediately now?"
;LangString LangRegKey ${LANG_SIMPCHINESE} "default"

;!include "..\lang\Chinese_Traditional.nsh"
;!include "..\lang\Chinese_Simplified.nsh"
!include "Japanese.nsh"
!include "x64.nsh"
!include "Library.nsh"


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

	; Remove old files
	${If} ${RunningX64}
		Delete /REBOOTOK "$INSTDIR\mp3infp_regist_x64.exe"
	${EndIf}
	Delete /REBOOTOK "$INSTDIR\mp3infp_regist.exe"

	File "..\x86\mp3infp_setup.exe"
	File "..\mp3infp_eng.txt"
	File "..\mp3infp.txt"

	CreateDirectory "$INSTDIR\language"
	SetOutPath "$INSTDIR\language"
	
	; Language resources
	; (Use InstallLib even if the files are not DLL.)
	!define LIBRARY_IGNORE_VERSION

	; Japanese
	!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED \
			"..\Installer\Japanese.chm" \
			"$OUTDIR\Japanese.chm" "$OUTDIR"
	!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED \
			"..\x86\Japanese.lng" \
			"$OUTDIR\Japanese.lng" "$OUTDIR"

	; Chinese Traditional
;	!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED \
;			"..\lang\Chinese_Traditional.txt" \
;			"$OUTDIR\Chinese_Traditional.txt" "$OUTDIR"
;	!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED \
;			"..\lang\Chinese_Traditional.lng" \
;			"$OUTDIR\Chinese_Traditional.lng" "$OUTDIR"
	
	; Chinese Simplified
;	!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED \
;			"..\lang\Chinese_Simplified.txt" \
;			"$OUTDIR\Chinese_Simplified.txt" "$OUTDIR"
;	!insertmacro InstallLib DLL NOTSHARED REBOOT_NOTPROTECTED \
;			"..\lang\Chinese_Simplified.lng" \
;			"$OUTDIR\Chinese_Simplified.lng" "$OUTDIR"
	
	;----------------------------------------------------------------
	SetOutPath "$SYSDIR"

	; Remove old files
	${If} ${RunningX64}
		${DisableX64FSRedirection}
		Delete /REBOOTOK "$SYSDIR\mp3infp.cpl"
		${EnableX64FSRedirection}
	${Else}
		Delete /REBOOTOK "$SYSDIR\mp3infp.cpl"
	${EndIf}
	
	; mp3infp.dll
	; (Should we register the DLLs as shared DLLs?)
	!undef LIBRARY_IGNORE_VERSION

	; x86 mp3infp.dll
	!insertmacro InstallLib REGDLL NOTSHARED REBOOT_NOTPROTECTED \
			"..\x86\mp3infp.dll" \
			"$SYSDIR\mp3infp.dll" "$SYSDIR"

	${If} ${RunningX64}
		; x64 mp3infp.dll
		!define LIBRARY_X64
		!insertmacro InstallLib REGDLL NOTSHARED REBOOT_NOTPROTECTED \
				"..\x64\mp3infp.dll" \
				"$SYSDIR\mp3infp.dll" "$SYSDIR"
		!undef LIBRARY_X64
	${EndIf}
	
	;Language set
	WriteRegStr HKCU "${PRODUCT_REG_KEY}" "Language" "$(LangRegKey)"
	WriteRegStr HKCU "${PRODUCT_REG_KEY}" "Installer Language" "$LANGUAGE"
	
	;Start menu
	CreateDirectory "$SMPROGRAMS\${MUI_PRODUCT}"
	CreateShortcut "$SMPROGRAMS\${MUI_PRODUCT}\$(MP3INFP_SETUP_NAME).lnk" "$INSTDIR\mp3infp_setup.exe"
	
	;[1] mp3infp_regist.exe (Obsolete)
	; Delete old registory values.
	DeleteRegValue HKLM "${RUN_REG_KEY}" "${MUI_PRODUCT}"
	DeleteRegValue HKLM "${RUNONCE_REG_KEY}" "${MUI_PRODUCT}"
	${If} ${RunningX64}
		DeleteRegValue HKLM64 "${RUN_REG_KEY}" "${MUI_PRODUCT}"
		DeleteRegValue HKLM64 "${RUNONCE_REG_KEY}" "${MUI_PRODUCT}"
	${EndIf}

	;[2] Uninstall list
	${If} ${RunningX64}
		SetRegView 64
	${EndIf}
	WriteRegStr HKLM "${UNINST_REG_KEY}" "DisplayIcon" "$\"$INSTDIR\mp3infp_setup.exe$\",0"
	WriteRegStr HKLM "${UNINST_REG_KEY}" "DisplayName" "${MUI_PRODUCT} ${MUI_VERSION}"
	WriteRegStr HKLM "${UNINST_REG_KEY}" "DisplayVersion" "${MUI_VERSION}"
	WriteRegStr HKLM "${UNINST_REG_KEY}" "Publisher" "${PUBLISHER}"
	WriteRegStr HKLM "${UNINST_REG_KEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
	SectionGetSize ${SecCopyUI} $0
	WriteRegDWORD HKLM "${UNINST_REG_KEY}" "EstimatedSize" $0
	${If} ${RunningX64}
		SetRegView 32
	${EndIf}
	
	;[3] Store install folder
	WriteRegStr HKLM "${PRODUCT_REG_KEY}" "path" $INSTDIR
	${If} ${RunningX64}
		WriteRegStr HKLM64 "${PRODUCT_REG_KEY}" "path" $INSTDIR
	${EndIf}
	
	WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd
  
;--------------------------------

Function .onInit

	${If} ${RunningX64}
		SetRegView 64
		ReadRegStr $INSTDIR HKLM "${PRODUCT_REG_KEY}" "path"
		StrCmp $INSTDIR "" 0 regok64
		StrCpy $INSTDIR "$PROGRAMFILES64\${MUI_PRODUCT}"
		regok64:
		SetRegView 32
	${Else}
		ReadRegStr $INSTDIR HKLM "${PRODUCT_REG_KEY}" "path"
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
		!define LIBRARY_X64
		!insertmacro UnInstallLib REGDLL NOTSHARED REBOOT_NOTPROTECTED \
				"$SYSDIR\mp3infp.dll"
		!undef LIBRARY_X64
	${EndIf}
	!insertmacro UnInstallLib REGDLL NOTSHARED REBOOT_NOTPROTECTED \
			"$SYSDIR\mp3infp.dll"

	Delete /REBOOTOK "$INSTDIR\mp3infp_setup.exe"
	Delete /REBOOTOK "$INSTDIR\mp3infp.txt"
	Delete /REBOOTOK "$INSTDIR\mp3infp_eng.txt"
	Delete /REBOOTOK "$INSTDIR\language\Japanese.chm"
	Delete /REBOOTOK "$INSTDIR\language\Japanese.lng"
;	Delete /REBOOTOK "$INSTDIR\language\Chinese_Simplified.txt"
;	Delete /REBOOTOK "$INSTDIR\language\Chinese_Simplified.lng"
	Delete /REBOOTOK "$INSTDIR\Uninstall.exe"

	RMDir "$INSTDIR\language"
	RMDir "$INSTDIR"

	;Start menu
	Delete "$SMPROGRAMS\${MUI_PRODUCT}\$(MP3INFP_SETUP_NAME).lnk"
	RMDir "$SMPROGRAMS\${MUI_PRODUCT}"

	; [1]
	DeleteRegValue HKLM "${RUN_REG_KEY}" "${MUI_PRODUCT}"
	DeleteRegValue HKLM "${RUNONCE_REG_KEY}" "${MUI_PRODUCT}"
	${If} ${RunningX64}
		DeleteRegValue HKLM64 "${RUN_REG_KEY}" "${MUI_PRODUCT}"
		DeleteRegValue HKLM64 "${RUNONCE_REG_KEY}" "${MUI_PRODUCT}"
	${EndIf}
	; [2]
	${If} ${RunningX64}
		DeleteRegKey HKLM64 "${UNINST_REG_KEY}"
	${Else}
		DeleteRegKey HKLM "${UNINST_REG_KEY}"
	${EndIf}
	; [3]
	DeleteRegKey HKLM "${PRODUCT_REG_KEY}"
	DeleteRegKey /ifempty HKLM "${COMPANY_REG_KEY}"
	${If} ${RunningX64}
		DeleteRegKey HKLM64 "${PRODUCT_REG_KEY}"
		DeleteRegKey /ifempty HKLM64 "${COMPANY_REG_KEY}"
	${EndIf}
	
	DeleteRegKey HKCU "${PRODUCT_REG_KEY}"
	DeleteRegKey /ifempty HKCU "${COMPANY_REG_KEY}"
	DeleteRegValue HKLM "${SHDLLS_REG_KEY}" "$SYSDIR\mp3infp.dll"
	${If} ${RunningX64}
		DeleteRegValue HKLM64 "${SHDLLS_REG_KEY}" "$SYSDIR\mp3infp.dll"
	${EndIf}
	
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
	ReadRegStr $0 HKCU "${PRODUCT_REG_KEY}" "Installer Language"
	StrCmp $0 "" regskip 0
	StrCpy $LANGUAGE $0
	regskip:
FunctionEnd
