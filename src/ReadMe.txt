mp3infp/u シェル拡張型タグエディタ (mp3インフピー) (Unicode版)
Ver.2.55 beta3 Source code for Windows 7/8/10
Copyright(C) 1998-2007 T-Matsuo (win32lab.com)
Copyright(C) 2009 Rem
Copyright(C) 2009-2018 K.Takata

■ビルド環境(別途入手してください)
・Windows Driver Kit 7.1.0 (7600.16385.1)
https://www.microsoft.com/en-us/download/details.aspx?id=11800
・Windows Driver Kit 6001.18002 (Win2k 用にビルドする場合)

■mp3infp/u (Unicode版) の公開関数
今までの mp3infp_～ という関数に加え、mp3infp_～A、mp3infp_～W という
関数をエクスポートしています。（mp3infp_unicode.def 参照）

例)
  mp3infp_Load      従来通りの ANSI 版関数
  mp3infp_LoadA     mp3infp_Load と同一
  mp3infp_LoadW     Unicode 版関数

■ビルド方法
本バージョンは、WDK 7.1.0 を使ってビルドされています。（WDK には MFC も含まれ
ています。）
x64 ビルドの場合、スタートメニューから、

  Windows Driver Kits
    WDK 7600.16385.1
      Build Environments
        Windows Server 2003  (あるいは Windows 7)
          x64 Free Build Environment

を選択すると x64 リリースビルド用の環境が開きますので、
mp3infp の src ディレクトリに移動して、コマンドラインから build -g を
実行するとビルドされます。mp3infp.dll は dll ディレクトリの下に
ディレクトリが作成され、その下に出力されます。

x86 ビルドの場合、スタートメニューから以下を選択してください。

  Windows Driver Kits
    WDK 7600.16385.1
      Build Environments
        Windows XP
          x86 Free Build Environment

Win2k 向け x86 ビルドが必要な場合、WDK 6001.18002 をインストールした上で、
スタートメニューから

  Windows Driver Kits
    WDK 6001.18002
      Build Environments
        Windows 2000
          Open a build window for Windows 2000 x86 Free Build Environment

を選択してください。

※ WDK 7.0.0 以降では、Win2k 用の実行ファイルはビルドはサポートされていません。
   旧バージョンの WDK 6001.18002 が必要です。
   また、WDK 6001.18002 を使った場合でも、一部の関数を使用すると、
   Win2k では実行できないファイルができてしまいます。w2kstub.cpp は
   それを無理やり修正するためのソースです（一部 XP 用）。

※ WDK に含まれている MFC は 6.0 です。（Visual C++ 6.0 と同等）
   MFC 7.0 からは CString が拡張され、Unicode 化がやりやすくなっている
   のですが、今回はそれらの機能は使用できないため、ソースが分かりにくく
   なっている部分があるかもしれません。

■使用ライブラリ
OggVorbis形式のファイル読み書きには下記オープンソースライブラリを使用します。
 >THE OggVorbis SOURCE CODE IS (C) COPYRIGHT 1994-2001
 >by the XIPHOPHORUS Company http://www.xiph.org/

mp4の読み書きにはmp4v2ライブラリを使用しています
http://code.google.com/p/mp4v2/
※ソースを一部改変しています(改変箇所には "K.Takata" の署名を記載しています)

■ライセンス
このソフトウェアはLGPLライセンスで配布しています。
扱いに付いては添付のLGPL許諾契約書(lgpl.txt)を参照してください。

----------------------------------
mp3infp/u
https://github.com/k-takata/mp3infp

Win32工作小屋
http://win32lab.com/
