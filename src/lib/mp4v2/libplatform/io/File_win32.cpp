#include "src/impl.h"
#include "libplatform/impl.h" /* for platform_win32_impl.h which declares Utf8ToFilename */
#include <windows.h>
#include <tchar.h>
// 2010-06-22 K.Takata

extern "C"
int __cdecl _fseeki64_w2k(FILE *stream, __int64 offset, int whence);

namespace mp4v2 {
    using namespace impl;
}

/**
 * Set this to 1 to compile in extra debugging
 */
#define EXTRA_DEBUG 0

/**
 * @def LOG_PRINTF
 *
 * call log.printf if EXTRA_DEBUG is defined to 1.  Do
 * nothing otherwise
 */
#if EXTRA_DEBUG
#define LOG_PRINTF(X) log.printf X
#else
#define LOG_PRINTF(X)
#endif

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

    /**
     * The UTF-8 encoded file name
     */
    std::string _name;
};

///////////////////////////////////////////////////////////////////////////////

StandardFileProvider::StandardFileProvider()
    : _handle( NULL )
{
}

/**
 * Open a file
 *
 * @param name the name of a file to open
 * @param mode the mode to open @p name
 *
 * @retval false successfully opened @p name
 * @retval true error opening @p name
 */
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

    win32::Utf8ToFilename filename(name);

    if (!filename.IsUTF16Valid())
    {
        // The logging is done
        return true;
    }

    ASSERT(LPCWSTR(filename));
    _handle = _wfopen( filename, modestr );
    if (_handle == NULL)
    {
        log.errorf("%s: _wfopen(%s) failed (%d)",__FUNCTION__,filename.utf8.c_str(),errno);
        return true;
    }

    /*
    ** Make a copy of the name for future log messages, etc.
    */
    log.verbose2f("%s: _wfopen(%s) succeeded",__FUNCTION__,filename.utf8.c_str());

    _name = filename.utf8;
    return false;
}

/**
 * Seek to an offset in the file
 *
 * @param pos the offset from the beginning of the file to
 * seek to
 *
 * @retval false successfully seeked to @p pos
 * @retval true error seeking to @p pos
 */
bool
StandardFileProvider::seek( Size pos )
{
    int ret;

    ASSERT(_handle != NULL);

#if 1
    ret = _fseeki64_w2k( _handle, pos, SEEK_SET );
#else
    fpos_t fpos = pos;
    ret = fsetpos( _handle, &fpos );
#endif
    if ( ret )
    {
        log.errorf("%s: _fseeki64(%s,%" PRId64 ") failed (%d)",__FUNCTION__,_name.c_str(),
                                pos,ret);
        return true;
    }

    return false;
}

/**
 * Read from the file
 *
 * @param buffer populated with at most @p size bytes from
 * the file
 *
 * @param size the maximum number of bytes to read
 *
 * @param nin the 
 *
 * @retval false successfully read from the file
 * @retval true error reading from the file
 */
bool
StandardFileProvider::read( void* buffer, Size size, Size& nin, Size maxChunkSize )
{
    size_t nread = 0;

    ASSERT(_handle != NULL);

    // ReadFile takes a DWORD for number of bytes to read so
    // make sure we're not asking for more than fits.
    // MAXDWORD from WinNT.h.
    ASSERT(size <= MAXDWORD);
    nread = fread( buffer, 1, (DWORD)(size & MAXDWORD), _handle );
    if( nread == 0 )
    {
        log.errorf("%s: fread(%s,%d) failed (%d)",__FUNCTION__,_name.c_str(),
                   (DWORD)(size & MAXDWORD),errno);
        return true;
    }
    LOG_PRINTF((MP4_LOG_VERBOSE3,"%s: fread(%s,%d) succeeded: read %d byte(s)",__FUNCTION__,
               _name.c_str(),(DWORD)(size & MAXDWORD),nread));
    nin = nread;
    return false;
}

/**
 * Write to the file
 *
 * @param buffer the data to write
 *
 * @param size the number of bytes of @p buffer to write
 *
 * @param nout populated with the number of bytes actually
 * written if the function succeeds
 *
 * @retval false successfully wrote to the file
 * @retval true error writing to the file
 */
bool
StandardFileProvider::write( const void* buffer, Size size, Size& nout, Size maxChunkSize )
{
    size_t nwrote = 0;

    ASSERT(_handle != NULL);

    // ReadFile takes a DWORD for number of bytes to read so
    // make sure we're not asking for more than fits.
    // MAXDWORD from WinNT.h.
    ASSERT(size <= MAXDWORD);
    nwrote = fwrite( buffer, 1, (DWORD)(size & MAXDWORD), _handle );
    if( nwrote == 0 )
    {
        log.errorf("%s: fwrite(%s,%d) failed (%d)",__FUNCTION__,_name.c_str(),
                   (DWORD)(size & MAXDWORD),errno);
        return true;
    }
    log.verbose2f("%s: fwrite(%s,%d) succeeded: wrote %d byte(s)",__FUNCTION__,
                  _name.c_str(),(DWORD)(size & MAXDWORD),nwrote);
    nout = nwrote;
    return false;
}

/**
 * Close the file
 *
 * @retval false successfully closed the file
 * @retval true error closing the file
 */
bool
StandardFileProvider::close()
{
    int retval;

    retval = fclose( _handle );
    if (retval)
    {
        log.errorf("%s: fclose(%s) failed (%d)",__FUNCTION__,
                   _name.c_str(),errno);
    }

    // Whether we succeeded or not, clear the handle and
    // forget the name
    _handle = NULL;
    _name.clear();

    return retval;
}

///////////////////////////////////////////////////////////////////////////////

FileProvider&
FileProvider::standard()
{
    return *new StandardFileProvider();
}

///////////////////////////////////////////////////////////////////////////////

}}} // namespace mp4v2::platform::io
