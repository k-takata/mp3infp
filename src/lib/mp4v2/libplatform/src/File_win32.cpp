#include "libplatform/impl.h"
#include <tchar.h>
// 2010-06-22 K.Takata

namespace mp4v2 { namespace platform { namespace io {

///////////////////////////////////////////////////////////////////////////////

class StandardFileProvider : public FileProvider
{
public:
    StandardFileProvider();

    bool open( std::string name, Mode mode );
    bool seek( Size pos );
    bool read( void* buffer, Size size, Size& nin, Size maxChunkSize );
    bool write( const void* buffer, Size size, Size& nout, Size maxChunkSize );
    bool close();

private:
    FILE *_handle;
};

///////////////////////////////////////////////////////////////////////////////

StandardFileProvider::StandardFileProvider()
    : _handle( NULL )
{
}

bool
StandardFileProvider::open( std::string name, Mode mode )
{
    TCHAR *modestr;

    switch( mode ) {
        case MODE_UNDEFINED:
        case MODE_READ:
        default:
            modestr = _T("rb");
            break;

        case MODE_MODIFY:
            modestr = _T("r+b");
            break;

        case MODE_CREATE:
            modestr = _T("w+b");
            break;
    }

#ifdef UNICODE
    WCHAR namew[MP4V2_PATH_MAX];
    MultiByteToWideChar(CP_UTF8, 0, name.c_str(), -1, namew, MP4V2_PATH_MAX);
    _handle = _wfopen( namew, modestr );
#else
    _handle = fopen( name.c_str(), modestr );
#endif
    return _handle == NULL;
}

bool
StandardFileProvider::seek( Size pos )
{
    return _fseeki64( _handle, pos, SEEK_SET ) != 0;
}

bool
StandardFileProvider::read( void* buffer, Size size, Size& nin, Size maxChunkSize )
{
    size_t count = fread( buffer, 1, size, _handle );
    if ( count == 0 ) {
        return true;
    }
    nin = count;
    return false;
}

bool
StandardFileProvider::write( const void* buffer, Size size, Size& nout, Size maxChunkSize )
{
    size_t count = fwrite( buffer, 1, size, _handle );
    if ( count == 0 ) {
        return true;
    }
    nout = count;
    return false;
}

bool
StandardFileProvider::close()
{
    return fclose( _handle ) != 0;
}

///////////////////////////////////////////////////////////////////////////////

FileProvider&
FileProvider::standard()
{
    return *new StandardFileProvider();
}

///////////////////////////////////////////////////////////////////////////////

}}} // namespace mp4v2::platform::io
