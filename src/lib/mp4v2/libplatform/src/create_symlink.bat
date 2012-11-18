@echo off

if not exist ..\platform_win32.cpp (
	echo Error: source file not found
	exit
)

rem Delete even they have system attribute.
rem Cygwin's symlink must be recreated by mklink.
del error.cpp File.cpp FileSystem.cpp FileSystem_win32.cpp File_win32.cpp ^
	option.cpp process_win32.cpp platform_win32.cpp random_win32.cpp ^
	time.cpp time_win32.cpp /f /a

mklink error.cpp            ..\sys\error.cpp
mklink File.cpp             ..\io\File.cpp
mklink FileSystem.cpp       ..\io\FileSystem.cpp
mklink FileSystem_win32.cpp ..\io\FileSystem_win32.cpp
mklink File_win32.cpp       ..\io\File_win32.cpp
mklink option.cpp           ..\prog\option.cpp
mklink platform_win32.cpp   ..\platform_win32.cpp
mklink process_win32.cpp    ..\process\process_win32.cpp
mklink random_win32.cpp     ..\number\random_win32.cpp
mklink time.cpp             ..\time\time.cpp
mklink time_win32.cpp       ..\time\time_win32.cpp
