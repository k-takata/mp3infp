# Microsoft Developer Studio Project File - Name="mp3infp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** 編集しないでください **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mp3infp - Win32 Release
!MESSAGE これは有効なﾒｲｸﾌｧｲﾙではありません。 このﾌﾟﾛｼﾞｪｸﾄをﾋﾞﾙﾄﾞするためには NMAKE を使用してください。
!MESSAGE [ﾒｲｸﾌｧｲﾙのｴｸｽﾎﾟｰﾄ] ｺﾏﾝﾄﾞを使用して実行してください
!MESSAGE 
!MESSAGE NMAKE /f "mp3infp.mak".
!MESSAGE 
!MESSAGE NMAKE の実行時に構成を指定できます
!MESSAGE ｺﾏﾝﾄﾞ ﾗｲﾝ上でﾏｸﾛの設定を定義します。例:
!MESSAGE 
!MESSAGE NMAKE /f "mp3infp.mak" CFG="mp3infp - Win32 Release"
!MESSAGE 
!MESSAGE 選択可能なﾋﾞﾙﾄﾞ ﾓｰﾄﾞ:
!MESSAGE 
!MESSAGE "mp3infp - Win32 Release" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE "mp3infp - Win32 Debug" ("Win32 (x86) Dynamic-Link Library" 用)
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mp3infp - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /I "./id3lib/include/" /I "./id3lib/include/id3/" /I "lib/mpeg4ip-1.4.1/lib/mp4v2/" /I "lib/mpeg4ip-1.4.1/include" /I "lib/oggvorbis-win32sdk-1.0.1/include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D ISOLATION_AWARE_ENABLED=1 /D "_WINDLL" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "NDEBUG"
# ADD RSC /l 0x411 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 /nologo /subsystem:windows /dll /map /machine:I386
# SUBTRACT LINK32 /pdb:none /debug
# Begin Custom Build
InputPath=.\Release\mp3infp.dll
SOURCE="$(InputPath)"

"a" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo on 
	Verupdate.exe ver.h 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "mp3infp - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "lib/mpeg4ip-1.4.1/lib/mp4v2/" /I "lib/mpeg4ip-1.4.1/include" /I "lib/oggvorbis-win32sdk-1.0.1/include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D ISOLATION_AWARE_ENABLED=1 /D "_WINDLL" /Yu"stdafx.h" /FD /GZ /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x411 /d "_DEBUG"
# ADD RSC /l 0x411 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /dll /debug /machine:I386 /out:"c:\windows\system32\mp3infp.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none
# Begin Custom Build
TargetPath=c:\windows\system32\mp3infp.dll
TargetName=mp3infp
InputPath=c:\windows\system32\mp3infp.dll
SOURCE="$(InputPath)"

"c:\windows\system32\$(TargetName).dll" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	echo on 
	copy                                                    $(TargetPath)                                                    c:\windows\system32\  
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "mp3infp - Win32 Release"
# Name "mp3infp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h"
# Begin Group "tags"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Id3tagv1.cpp
# End Source File
# Begin Source File

SOURCE=.\Id3tagv1.h
# End Source File
# Begin Source File

SOURCE=.\Id3tagv2.cpp
# End Source File
# Begin Source File

SOURCE=.\Id3tagv2.h
# End Source File
# Begin Source File

SOURCE=.\M3u.cpp
# End Source File
# Begin Source File

SOURCE=.\M3u.h
# End Source File
# Begin Source File

SOURCE=.\Mp3Info.cpp
# End Source File
# Begin Source File

SOURCE=.\Mp3Info.h
# End Source File
# Begin Source File

SOURCE=.\RiffSIF.cpp
# End Source File
# Begin Source File

SOURCE=.\RiffSIF.h
# End Source File
# Begin Source File

SOURCE=.\RMP.cpp
# End Source File
# Begin Source File

SOURCE=.\RMP.h
# End Source File
# Begin Source File

SOURCE=.\Tag_Ape.cpp
# End Source File
# Begin Source File

SOURCE=.\Tag_Ape.h
# End Source File
# Begin Source File

SOURCE=.\Tag_Mp4.cpp
# End Source File
# Begin Source File

SOURCE=.\Tag_Mp4.h
# End Source File
# Begin Source File

SOURCE=.\Tag_Ogg.cpp
# End Source File
# Begin Source File

SOURCE=.\Tag_Ogg.h
# End Source File
# Begin Source File

SOURCE=.\Tag_OpenDML.cpp
# End Source File
# Begin Source File

SOURCE=.\Tag_OpenDML.h
# End Source File
# Begin Source File

SOURCE=.\Tag_Wma.cpp
# End Source File
# Begin Source File

SOURCE=.\Tag_Wma.h
# End Source File
# Begin Source File

SOURCE=.\Vqf.cpp
# End Source File
# Begin Source File

SOURCE=.\Vqf.h
# End Source File
# End Group
# Begin Group "pages"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Dlg_ogg_exttag.cpp
# End Source File
# Begin Source File

SOURCE=.\Dlg_ogg_exttag.h
# End Source File
# Begin Source File

SOURCE=.\Page_ape.cpp
# End Source File
# Begin Source File

SOURCE=.\Page_ape.h
# End Source File
# Begin Source File

SOURCE=.\Page_avi.cpp
# End Source File
# Begin Source File

SOURCE=.\Page_avi.h
# End Source File
# Begin Source File

SOURCE=.\Page_m3u.cpp
# End Source File
# Begin Source File

SOURCE=.\Page_m3u.h
# End Source File
# Begin Source File

SOURCE=.\Page_mp3_APE.cpp
# End Source File
# Begin Source File

SOURCE=.\Page_mp3_APE.h
# End Source File
# Begin Source File

SOURCE=.\Page_mp3_ID3V1.cpp
# End Source File
# Begin Source File

SOURCE=.\Page_mp3_ID3V1.h
# End Source File
# Begin Source File

SOURCE=.\Page_mp3_ID3V2.cpp
# End Source File
# Begin Source File

SOURCE=.\Page_mp3_ID3V2.h
# End Source File
# Begin Source File

SOURCE=.\Page_mp3_LAME.cpp
# End Source File
# Begin Source File

SOURCE=.\Page_mp3_LAME.h
# End Source File
# Begin Source File

SOURCE=.\Page_mp3_RMP.cpp
# End Source File
# Begin Source File

SOURCE=.\Page_mp3_RMP.h
# End Source File
# Begin Source File

SOURCE=.\Page_mp4.cpp
# End Source File
# Begin Source File

SOURCE=.\Page_mp4.h
# End Source File
# Begin Source File

SOURCE=.\Page_ogg.cpp
# End Source File
# Begin Source File

SOURCE=.\Page_ogg.h
# End Source File
# Begin Source File

SOURCE=.\Page_vqf.cpp
# End Source File
# Begin Source File

SOURCE=.\Page_vqf.h
# End Source File
# Begin Source File

SOURCE=.\Page_wave.cpp
# End Source File
# Begin Source File

SOURCE=.\Page_wave.h
# End Source File
# Begin Source File

SOURCE=.\Page_wma.cpp
# End Source File
# Begin Source File

SOURCE=.\Page_wma.h
# End Source File
# End Group
# Begin Group "Info"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Info_MonkeysAudio.cpp
# End Source File
# Begin Source File

SOURCE=.\Info_MonkeysAudio.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\classID.h
# End Source File
# Begin Source File

SOURCE=.\DlgWriteProgress.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgWriteProgress.h
# End Source File
# Begin Source File

SOURCE=.\GetFloppyType.cpp
# End Source File
# Begin Source File

SOURCE=.\GetFloppyType.h
# End Source File
# Begin Source File

SOURCE=.\GlobalCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\GlobalCommand.h
# End Source File
# Begin Source File

SOURCE=.\MMCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\MMCommand.h
# End Source File
# Begin Source File

SOURCE=.\mp3infp.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3infp.def
# End Source File
# Begin Source File

SOURCE=.\mp3infp.h
# End Source File
# Begin Source File

SOURCE=.\mp3infp_ExportFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3infp_ExportFunc.h
# End Source File
# Begin Source File

SOURCE=.\RegAccess.cpp
# End Source File
# Begin Source File

SOURCE=.\RegAccess.h
# End Source File
# Begin Source File

SOURCE=.\ShellExt.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellExt.h
# End Source File
# Begin Source File

SOURCE=.\ShellExt_IColumnProvider.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellExt_IContextMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellExt_IPersist.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellExt_IPersistFile.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellExt_IQueryInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellExt_IShellExtInit.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellExt_IShellPropSheetExt.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellExtClassFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\ShellExtClassFactory.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\wa_ipc.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap_a.bmp
# End Source File
# Begin Source File

SOURCE=.\res\ffw.bmp
# End Source File
# Begin Source File

SOURCE=.\res\icon_aot.ico
# End Source File
# Begin Source File

SOURCE=.\mp3infp.rc
# End Source File
# Begin Source File

SOURCE=.\res\mp3infp.rc2
# End Source File
# Begin Source File

SOURCE=.\res\pause.bmp
# End Source File
# Begin Source File

SOURCE=.\res\play.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\res\rew.bmp
# End Source File
# Begin Source File

SOURCE=.\res\stop.bmp
# End Source File
# Begin Source File

SOURCE=.\ver.h
# End Source File
# End Group
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\lib\oggvorbis-win32sdk-1.0.1\lib\ogg_static_d.lib"

!IF  "$(CFG)" == "mp3infp - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mp3infp - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\lib\oggvorbis-win32sdk-1.0.1\lib\ogg_static.lib"

!IF  "$(CFG)" == "mp3infp - Win32 Release"

!ELSEIF  "$(CFG)" == "mp3infp - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\lib\oggvorbis-win32sdk-1.0.1\lib\vorbis_static_d.lib"

!IF  "$(CFG)" == "mp3infp - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mp3infp - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\lib\oggvorbis-win32sdk-1.0.1\lib\vorbis_static.lib"

!IF  "$(CFG)" == "mp3infp - Win32 Release"

!ELSEIF  "$(CFG)" == "mp3infp - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\lib\oggvorbis-win32sdk-1.0.1\lib\vorbisfile_static_d.lib"

!IF  "$(CFG)" == "mp3infp - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "mp3infp - Win32 Debug"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=".\lib\oggvorbis-win32sdk-1.0.1\lib\vorbisfile_static.lib"

!IF  "$(CFG)" == "mp3infp - Win32 Release"

!ELSEIF  "$(CFG)" == "mp3infp - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\res\mp3infp.dll.manifest
# End Source File
# Begin Source File

SOURCE=.\mp3infp.txt
# End Source File
# Begin Source File

SOURCE=.\mp3infp_eng.txt
# End Source File
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
