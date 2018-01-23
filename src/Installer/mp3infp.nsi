; languages.nsi
;
; This is an example of a multilingual installer
; The user can select the language on startup

;--------------------------------
!define COMPANY "win32lab.com" ;Define your own software name here
!define PRODUCT "mp3infp" ;Define your own software name here
!define PUBLISHER "T-Matsuo, Rem and K.Takata"
;!define VERSION "2.54g/u7" ;Define your own software version here
;OutFile mp3infp254g_u7.exe

!define UNINST_REG_KEY	"Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}"
!define RUN_REG_KEY	"Software\Microsoft\Windows\CurrentVersion\Run"
!define RUNONCE_REG_KEY	"Software\Microsoft\Windows\CurrentVersion\RunOnce"
!define SHDLLS_REG_KEY	"Software\Microsoft\Windows\CurrentVersion\SharedDLLs"
!define COMPANY_REG_KEY	"Software\${COMPANY}"
!define PRODUCT_REG_KEY	"Software\${COMPANY}\${PRODUCT}"
!define INSTDIR_REG_VALNAME  "path"
!define INSTLANG_REG_VALNAME "Installer Language"

Unicode true
SetCompressor /SOLID lzma
XPStyle on
RequestExecutionLevel admin
ManifestDPIAware true

!packhdr "$%TEMP%\exehead.tmp" '"upx.exe" -9 "$%TEMP%\exehead.tmp"'

;--------------------------------

!include "x64.nsh"
!include "Library.nsh"
!include "MUI2.nsh"


;--------------------------------
; Interface Settings

!define MUI_ABORTWARNING
!define MUI_UNABORTWARNING
!define MUI_LANGDLL_ALLLANGUAGES

; Uncomment the following lines for debugging.
;!define MUI_FINISHPAGE_NOAUTOCLOSE
;!define MUI_UNFINISHPAGE_NOAUTOCLOSE

;--------------------------------
; Language Selection Dialog Settings

; Remember the installer language
!define MUI_LANGDLL_REGISTRY_ROOT "HKCU"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_REG_KEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "${INSTLANG_REG_VALNAME}"

;--------------------------------
; Pages

!insertmacro MUI_PAGE_LICENSE "$(SLICENSEFILE)"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

;--------------------------------
; Languages

!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Japanese"

LangString LangRegKey ${LANG_ENGLISH} "default"
LangString MP3INFP_SETUP_NAME ${LANG_ENGLISH} "mp3infp setup"

!include "Japanese.nsh"
;!include "..\lang\Chinese_Traditional.nsh"
;!include "..\lang\Chinese_Simplified.nsh"

LicenseLangString SLICENSEFILE  ${LANG_ENGLISH} "..\mp3infp_eng.txt"
LicenseLangString SLICENSEFILE  ${LANG_JAPANESE} "..\mp3infp.txt"
;LicenseLangString SLICENSEFILE  ${LANG_TRADCHINESE} "..\lang\Chinese_Traditional.txt"
;LicenseLangString SLICENSEFILE  ${LANG_SIMPCHINESE} "..\lang\Chinese_Simplified.txt"

LangString Name ${LANG_ENGLISH} "${PRODUCT} v${VERSION}"
LangString Name ${LANG_JAPANESE} "${PRODUCT} v${VERSION}"
;LangString Name ${LANG_TRADCHINESE} "${PRODUCT} v${VERSION}"
;LangString Name ${LANG_SIMPCHINESE} "${PRODUCT} v${VERSION}"
Name "$(Name)"

;--------------------------------
; Reserve Files

!insertmacro MUI_RESERVEFILE_LANGDLL

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
;	!undef LIBRARY_IGNORE_VERSION

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
	WriteRegStr HKCU "${PRODUCT_REG_KEY}" "${INSTLANG_REG_VALNAME}" "$LANGUAGE"

	;Start menu
	CreateDirectory "$SMPROGRAMS\${PRODUCT}"
	CreateShortcut "$SMPROGRAMS\${PRODUCT}\$(MP3INFP_SETUP_NAME).lnk" "$INSTDIR\mp3infp_setup.exe"

	;[1] mp3infp_regist.exe (Obsolete)
	; Delete old registory values.
	DeleteRegValue HKLM "${RUN_REG_KEY}" "${PRODUCT}"
	DeleteRegValue HKLM "${RUNONCE_REG_KEY}" "${PRODUCT}"
	${If} ${RunningX64}
		DeleteRegValue HKLM64 "${RUN_REG_KEY}" "${PRODUCT}"
		DeleteRegValue HKLM64 "${RUNONCE_REG_KEY}" "${PRODUCT}"
	${EndIf}

	;[2] Uninstall list
	${If} ${RunningX64}
		SetRegView 64
	${EndIf}
	WriteRegStr HKLM "${UNINST_REG_KEY}" "DisplayIcon" '"$INSTDIR\mp3infp_setup.exe",0'
	WriteRegStr HKLM "${UNINST_REG_KEY}" "DisplayName" "${PRODUCT} ${VERSION}"
	WriteRegStr HKLM "${UNINST_REG_KEY}" "DisplayVersion" "${VERSION}"
	WriteRegStr HKLM "${UNINST_REG_KEY}" "Publisher" "${PUBLISHER}"
	WriteRegStr HKLM "${UNINST_REG_KEY}" "UninstallString" '"$INSTDIR\uninstall.exe"'
	SectionGetSize ${SecCopyUI} $0
	WriteRegDWORD HKLM "${UNINST_REG_KEY}" "EstimatedSize" $0
	${If} ${RunningX64}
		SetRegView 32
	${EndIf}

	;[3] Store install folder
	WriteRegStr HKLM "${PRODUCT_REG_KEY}" "${INSTDIR_REG_VALNAME}" $INSTDIR
	${If} ${RunningX64}
		WriteRegStr HKLM64 "${PRODUCT_REG_KEY}" "${INSTDIR_REG_VALNAME}" $INSTDIR
	${EndIf}

	WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

;--------------------------------

Function .onInit

	!insertmacro MUI_LANGDLL_DISPLAY

	; Load install folder
	ReadRegStr $0 HKLM "${PRODUCT_REG_KEY}" "${INSTDIR_REG_VALNAME}"
	${If} "$0" == ""
		; Not previously installed.
		${If} "$INSTDIR" == ""
			; Install folder is not specified by the command line. (/D=path)
			${If} ${RunningX64}
				StrCpy $INSTDIR "$PROGRAMFILES64\${PRODUCT}"
			${Else}
				StrCpy $INSTDIR "$PROGRAMFILES\${PRODUCT}"
			${EndIf}
		${EndIf}
	${Else}
		; Old version might be already installed. Install there. (Ignore /D.)
		StrCpy $INSTDIR $0
	${EndIf}

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
	Delete "$SMPROGRAMS\${PRODUCT}\$(MP3INFP_SETUP_NAME).lnk"
	RMDir "$SMPROGRAMS\${PRODUCT}"

	; [1]
	DeleteRegValue HKLM "${RUN_REG_KEY}" "${PRODUCT}"
	DeleteRegValue HKLM "${RUNONCE_REG_KEY}" "${PRODUCT}"
	${If} ${RunningX64}
		DeleteRegValue HKLM64 "${RUN_REG_KEY}" "${PRODUCT}"
		DeleteRegValue HKLM64 "${RUNONCE_REG_KEY}" "${PRODUCT}"
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

;--------------------------------
;Uninstaller Functions

Function un.onInit
	;Get language from registry
	!insertmacro MUI_UNGETLANGUAGE
FunctionEnd
