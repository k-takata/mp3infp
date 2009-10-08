# Microsoft Developer Studio Project File - Name="mp3infp_cpl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mp3infp_cpl - Win32 Release
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "mp3infp_cpl.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "mp3infp_cpl.mak" CFG="mp3infp_cpl - Win32 Release"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "mp3infp_cpl - Win32 Release" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "mp3infp_cpl - Win32 Debug" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mp3infp_cpl - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /machine:I386 /out:"Release/mp3infp.cpl"

!ELSEIF  "$(CFG)" == "mp3infp_cpl - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_WINDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "_WINDLL" /D SIDEBYSIDE_COMMONCONTROL=1 /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"Debug/mp3infp.cpl" /pdbtype:sept
# Begin Custom Build
TargetPath=.\Debug\mp3infp.cpl
TargetName=mp3infp
InputPath=.\Debug\mp3infp.cpl
SOURCE="$(InputPath)"

"c:\windows\system32\$(TargetName).cpl" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo on 
	copy                                    $(TargetPath)                                    c:\windows\system32\  
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "mp3infp_cpl - Win32 Release"
# Name "mp3infp_cpl - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h"
# Begin Group "common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\GlobalCommand.cpp
# End Source File
# Begin Source File

SOURCE=..\GlobalCommand.h
# End Source File
# Begin Source File

SOURCE=..\RegAccess.cpp
# End Source File
# Begin Source File

SOURCE=..\RegAccess.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\mp3infp_cpl.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3infp_cpl.def
# End Source File
# Begin Source File

SOURCE=.\mp3infp_cpl.h
# End Source File
# Begin Source File

SOURCE=.\PageApe.cpp
# End Source File
# Begin Source File

SOURCE=.\PageApe.h
# End Source File
# Begin Source File

SOURCE=.\PageAvi.cpp
# End Source File
# Begin Source File

SOURCE=.\PageAvi.h
# End Source File
# Begin Source File

SOURCE=.\PageM3u.cpp
# End Source File
# Begin Source File

SOURCE=.\PageM3u.h
# End Source File
# Begin Source File

SOURCE=.\PageMain.cpp
# End Source File
# Begin Source File

SOURCE=.\PageMain.h
# End Source File
# Begin Source File

SOURCE=.\PageMp3.cpp
# End Source File
# Begin Source File

SOURCE=.\PageMp3.h
# End Source File
# Begin Source File

SOURCE=.\PageMp4.cpp
# End Source File
# Begin Source File

SOURCE=.\PageMp4.h
# End Source File
# Begin Source File

SOURCE=.\PageOgg.cpp
# End Source File
# Begin Source File

SOURCE=.\PageOgg.h
# End Source File
# Begin Source File

SOURCE=.\PageVqf.cpp
# End Source File
# Begin Source File

SOURCE=.\PageVqf.h
# End Source File
# Begin Source File

SOURCE=.\PageWave.cpp
# End Source File
# Begin Source File

SOURCE=.\PageWave.h
# End Source File
# Begin Source File

SOURCE=.\PageWma.cpp
# End Source File
# Begin Source File

SOURCE=.\PageWma.h
# End Source File
# Begin Source File

SOURCE=.\SetupProperty.cpp
# End Source File
# Begin Source File

SOURCE=.\SetupProperty.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\HAND.CUR
# End Source File
# Begin Source File

SOURCE=.\res\mp3infp.ico
# End Source File
# Begin Source File

SOURCE=.\mp3infp_cpl.rc
# End Source File
# Begin Source File

SOURCE=.\res\mp3infp_cpl.rc2
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\res\mp3infp.cpl.manifest
# End Source File
# End Target
# End Project
