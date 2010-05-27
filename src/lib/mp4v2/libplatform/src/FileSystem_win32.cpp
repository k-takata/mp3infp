#include "libplatform/impl.h"

namespace mp4v2 { namespace platform { namespace io {

///////////////////////////////////////////////////////////////////////////////

bool
FileSystem::exists( string path_ )
{
// 2010-05-28 K.Takata
#ifdef UNICODE
    WIN32_FIND_DATAW buf;
    WCHAR pathw[MP4V2_PATH_MAX];
    MultiByteToWideChar(CP_UTF8, 0, path_.c_str(), -1, pathw, MP4V2_PATH_MAX);
    HANDLE h = FindFirstFileW( pathw, &buf );
#else
    WIN32_FIND_DATAA buf;
    HANDLE h = FindFirstFileA( path_.c_str(), &buf );
#endif
    if( h == INVALID_HANDLE_VALUE )
        return false;
    FindClose( h );
    return true;
}

///////////////////////////////////////////////////////////////////////////////

bool
FileSystem::isDirectory( string path_ )
{
// 2010-05-28 K.Takata
#ifdef UNICODE
    WIN32_FIND_DATAW buf;
    WCHAR pathw[MP4V2_PATH_MAX];
    MultiByteToWideChar(CP_UTF8, 0, path_.c_str(), -1, pathw, MP4V2_PATH_MAX);
    HANDLE h = FindFirstFileW( pathw, &buf );
#else
    WIN32_FIND_DATAA buf;
    HANDLE h = FindFirstFileA( path_.c_str(), &buf );
#endif
    if( h == INVALID_HANDLE_VALUE )
        return false;
    FindClose( h );
    return (buf.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

///////////////////////////////////////////////////////////////////////////////

bool
FileSystem::isFile( string path_ )
{
// 2010-05-28 K.Takata
#ifdef UNICODE
    WIN32_FIND_DATAW buf;
    WCHAR pathw[MP4V2_PATH_MAX];
    MultiByteToWideChar(CP_UTF8, 0, path_.c_str(), -1, pathw, MP4V2_PATH_MAX);
    HANDLE h = FindFirstFileW( pathw, &buf );
#else
    WIN32_FIND_DATAA buf;
    HANDLE h = FindFirstFileA( path_.c_str(), &buf );
#endif
    if( h == INVALID_HANDLE_VALUE )
        return false;
    FindClose( h );
    return (buf.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
}

///////////////////////////////////////////////////////////////////////////////

bool
FileSystem::getFileSize( string path_, File::Size& size_ )
{
    size_ = 0;
    WIN32_FILE_ATTRIBUTE_DATA data;
// 2010-05-28 K.Takata
#ifdef UNICODE
    WCHAR pathw[MP4V2_PATH_MAX];
    MultiByteToWideChar(CP_UTF8, 0, path_.c_str(), -1, pathw, MP4V2_PATH_MAX);
    if( !GetFileAttributesExW( pathw, GetFileExInfoStandard, (LPVOID)&data ))
        return true;
#else
    if( !GetFileAttributesExA( path_.c_str(), GetFileExInfoStandard, (LPVOID)&data ))
        return true;
#endif
    size_ = ((File::Size)data.nFileSizeHigh << 32) | data.nFileSizeLow;
    return false;
}

///////////////////////////////////////////////////////////////////////////////

bool
FileSystem::rename( string from, string to )
{
// 2010-05-28 K.Takata
#ifdef UNICODE
    WCHAR fromw[MP4V2_PATH_MAX], tow[MP4V2_PATH_MAX];
    MultiByteToWideChar(CP_UTF8, 0, from.c_str(), -1, fromw, MP4V2_PATH_MAX);
    MultiByteToWideChar(CP_UTF8, 0, to.c_str(), -1, tow, MP4V2_PATH_MAX);
    if( exists( to )) {
        if( ::_wremove( tow ))
            return true;
    }
    return ::_wrename( fromw, tow ) != 0;
#else
    if( exists( to )) {
        if( ::remove( to.c_str() ))
            return true;
    }
    return ::rename( from.c_str(), to.c_str() ) != 0;
#endif
}

///////////////////////////////////////////////////////////////////////////////

string FileSystem::DIR_SEPARATOR  = "\\";
string FileSystem::PATH_SEPARATOR = ";";

///////////////////////////////////////////////////////////////////////////////

}}} // namespace mp4v2::platform::io
