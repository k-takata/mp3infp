/*
 * The contents of this file are subject to the Mozilla Public
 * License Version 1.1 (the "License"); you may not use this file
 * except in compliance with the License. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS
 * IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
 * implied. See the License for the specific language governing
 * rights and limitations under the License.
 *
 * The Original Code is MPEG4IP.
 *
 * The Initial Developer of the Original Code is Cisco Systems Inc.
 * Portions created by Cisco Systems Inc. are
 * Copyright (C) Cisco Systems Inc. 2001 - 2005.  All Rights Reserved.
 *
 * 3GPP features implementation is based on 3GPP's TS26.234-v5.60,
 * and was contributed by Ximpo Group Ltd.
 *
 * Portions created by Ximpo Group Ltd. are
 * Copyright (C) Ximpo Group Ltd. 2003, 2004.  All Rights Reserved.
 *
 * Contributor(s):
 *      Dave Mackie         dmackie@cisco.com
 *      Alix Marchandise-Franquet   alix@cisco.com
 *              Ximpo Group Ltd.                mp4v2@ximpo.com
 *              Bill May                        wmay@cisco.com
 */

/*
 * MP4 library API functions
 *
 * These are wrapper functions that provide C linkage conventions
 * to the library, and catch any internal errors, ensuring that
 * a proper return value is given.
 */

#include "src/impl.h"

#define PRINT_ERROR(e) \
    VERBOSE_ERROR(((MP4File*)hFile)->GetVerbosity(), e->Print());

using namespace mp4v2::impl;

extern "C" {

///////////////////////////////////////////////////////////////////////////////

MP4FileHandle MP4Read( const char* fileName, uint32_t verbosity )
{
    MP4File* pFile = NULL;
    try {
        pFile = new MP4File( verbosity );
        pFile->Read( fileName, NULL );
        return (MP4FileHandle)pFile;
    }
    catch ( MP4Error* e ) {
        VERBOSE_ERROR( verbosity, e->Print() );
        delete e;
        delete pFile;
        return MP4_INVALID_FILE_HANDLE;
    }
}

MP4FileHandle MP4ReadProvider( const char* fileName, uint32_t verbosity, const MP4FileProvider* fileProvider )
{
    MP4File* pFile = NULL;
    try {
        pFile = new MP4File( verbosity );
        pFile->Read( fileName, fileProvider );
        return (MP4FileHandle)pFile;
    }
    catch ( MP4Error* e ) {
        VERBOSE_ERROR( verbosity, e->Print() );
        delete e;
        delete pFile;
        return MP4_INVALID_FILE_HANDLE;
    }
}

///////////////////////////////////////////////////////////////////////////////

    MP4FileHandle MP4Create (const char* fileName,
                             uint32_t verbosity,
                             uint32_t  flags)
    {
        return MP4CreateEx(fileName, verbosity, flags);
    }

    MP4FileHandle MP4CreateEx (const char* fileName,
                               uint32_t verbosity,
                               uint32_t  flags,
                               int add_ftyp,
                               int add_iods,
                               char* majorBrand,
                               uint32_t minorVersion,
                               char** supportedBrands,
                               uint32_t supportedBrandsCount)
    {
        MP4File* pFile = NULL;
        try {
            pFile = new MP4File(verbosity);
            // LATER useExtensibleFormat, moov first, then mvex's
            pFile->Create(fileName, flags, add_ftyp, add_iods,
                          majorBrand, minorVersion,
                          supportedBrands, supportedBrandsCount);
            return (MP4FileHandle)pFile;
        }
        catch (MP4Error* e) {
            VERBOSE_ERROR(verbosity, e->Print());
            delete e;
            delete pFile;
            return MP4_INVALID_FILE_HANDLE;
        }
    }

    MP4FileHandle MP4Modify(const char* fileName,
                            uint32_t verbosity, uint32_t flags)
    {
        MP4File* pFile = NULL;
        try {
            pFile = new MP4File(verbosity);
            // LATER useExtensibleFormat, moov first, then mvex's
            if (pFile->Modify(fileName))
                return (MP4FileHandle)pFile;
        }
        catch (MP4Error* e) {
            VERBOSE_ERROR(verbosity, e->Print());
            delete e;
        }

        if (pFile) delete pFile;
        return MP4_INVALID_FILE_HANDLE;
    }

    bool MP4Optimize(const char* existingFileName,
                     const char* newFileName,
                     uint32_t verbosity)
    {
        try {
            MP4File* pFile = new MP4File(verbosity);
            pFile->Optimize(existingFileName, newFileName);
            delete pFile;
            return true;
        }
        catch (MP4Error* e) {
            VERBOSE_ERROR(verbosity, e->Print());
            delete e;
        }
        return false;
    }

    void MP4Close(MP4FileHandle hFile)
    {
        if( !MP4_IS_VALID_FILE_HANDLE( hFile ))
            return;

        MP4File& f = *(MP4File*)hFile;
        try {
            f.Close();
        }
        catch ( MP4Error* e ) {
            PRINT_ERROR( e );
            delete e;
        }

        delete &f;
    }

    bool MP4Dump(
        MP4FileHandle hFile,
        FILE* pDumpFile,
        bool dumpImplicits)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->Dump(pDumpFile, dumpImplicits);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }


    /* specific file properties */

    uint32_t MP4GetVerbosity(MP4FileHandle hFile)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetVerbosity();
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    void MP4SetVerbosity(MP4FileHandle hFile, uint32_t verbosity)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetVerbosity(verbosity);
                return;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return;
    }

    MP4Duration MP4GetDuration(MP4FileHandle hFile)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetDuration();
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_DURATION;
    }

    uint32_t MP4GetTimeScale(MP4FileHandle hFile)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTimeScale();
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    bool MP4SetTimeScale(MP4FileHandle hFile, uint32_t value)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetTimeScale(value);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    uint8_t MP4GetODProfileLevel(MP4FileHandle hFile)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetODProfileLevel();
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    bool MP4SetODProfileLevel(MP4FileHandle hFile, uint8_t value)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetODProfileLevel(value);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    uint8_t MP4GetSceneProfileLevel(MP4FileHandle hFile)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetSceneProfileLevel();
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    bool MP4SetSceneProfileLevel(MP4FileHandle hFile, uint8_t value)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetSceneProfileLevel(value);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    uint8_t MP4GetVideoProfileLevel(MP4FileHandle hFile,
                                    MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetVideoProfileLevel();
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
            if (MP4_IS_VALID_TRACK_ID(trackId)) {
                uint8_t *foo;
                uint32_t bufsize;
                uint8_t type;
                // for mpeg4 video tracks, try to look for the VOSH header,
                // which has this info.
                type = MP4GetTrackEsdsObjectTypeId(hFile, trackId);
                if (type == MP4_MPEG4_VIDEO_TYPE) {
                    if (MP4GetTrackESConfiguration(hFile,
                                                   trackId,
                                                   &foo,
                                                   &bufsize)) {
                        uint8_t *ptr = foo;
                        while (bufsize > 0) {
                            if (MP4V2_HTONL(*(uint32_t *)ptr) == 0x1b0) {
                                uint8_t ret = ptr[4];
                                free(foo);
                                return ret;
                            }
                            ptr++;
                            bufsize--;
                        }
                        free(foo);
                    }
                }
            }

        }
        return 0;
    }

    void MP4SetVideoProfileLevel(MP4FileHandle hFile, uint8_t value)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetVideoProfileLevel(value);
                return ;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return ;
    }

    uint8_t MP4GetAudioProfileLevel(MP4FileHandle hFile)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetAudioProfileLevel();
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    void MP4SetAudioProfileLevel(MP4FileHandle hFile, uint8_t value)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetAudioProfileLevel(value);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
    }

    uint8_t MP4GetGraphicsProfileLevel(MP4FileHandle hFile)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetGraphicsProfileLevel();
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    bool MP4SetGraphicsProfileLevel(MP4FileHandle hFile, uint8_t value)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetGraphicsProfileLevel(value);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    /* generic file properties */

    bool MP4HaveAtom (MP4FileHandle hFile, const char *atomName)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File *)hFile)->FindAtom(atomName) != NULL;
            } catch (MP4Error *e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4GetIntegerProperty(
        MP4FileHandle hFile, const char* propName, uint64_t *retvalue)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                *retvalue = ((MP4File*)hFile)->GetIntegerProperty(propName);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4GetFloatProperty(
        MP4FileHandle hFile, const char* propName, float *retvalue)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                *retvalue = ((MP4File*)hFile)->GetFloatProperty(propName);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4GetStringProperty(
        MP4FileHandle hFile, const char* propName,
        const char **retvalue)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                *retvalue =  ((MP4File*)hFile)->GetStringProperty(propName);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4GetBytesProperty(
        MP4FileHandle hFile, const char* propName,
        uint8_t** ppValue, uint32_t* pValueSize)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->GetBytesProperty(propName, ppValue, pValueSize);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        *ppValue = NULL;
        *pValueSize = 0;
        return false;
    }

    bool MP4SetIntegerProperty(
        MP4FileHandle hFile, const char* propName, int64_t value)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetIntegerProperty(propName, value);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4SetFloatProperty(
        MP4FileHandle hFile, const char* propName, float value)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetFloatProperty(propName, value);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4SetStringProperty(
        MP4FileHandle hFile, const char* propName, const char* value)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetStringProperty(propName, value);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4SetBytesProperty(
        MP4FileHandle hFile, const char* propName,
        const uint8_t* pValue, uint32_t valueSize)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetBytesProperty(propName, pValue, valueSize);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    /* track operations */

    MP4TrackId MP4AddTrack(
        MP4FileHandle hFile, const char* type)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->AddSystemsTrack(type);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    MP4TrackId MP4AddSystemsTrack(
        MP4FileHandle hFile, const char* type)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->AddSystemsTrack(type);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    MP4TrackId MP4AddODTrack(MP4FileHandle hFile)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->AddODTrack();
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    MP4TrackId MP4AddSceneTrack(MP4FileHandle hFile)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->AddSceneTrack();
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    MP4TrackId MP4AddAudioTrack(
        MP4FileHandle hFile,
        uint32_t timeScale,
        MP4Duration sampleDuration,
        uint8_t audioType)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->
                       AddAudioTrack(timeScale, sampleDuration, audioType);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

//
// API to initialize ismacryp properties to sensible defaults.
// if the input pointer is null then an ismacryp params is malloc'd.
// caller must see to it that it is properly disposed of.
//
    mp4v2_ismacrypParams *MP4DefaultISMACrypParams(mp4v2_ismacrypParams *ptr)
    {
        try
        {
            if (ptr == NULL) {
                ptr = (mp4v2_ismacrypParams *)MP4Malloc(sizeof(mp4v2_ismacrypParams));
            }
            memset(ptr, 0, sizeof(*ptr));
            return ptr;
        }

        catch (...) {
            return MP4_INVALID_TRACK_ID;
        }
    }


    MP4TrackId MP4AddAC3AudioTrack(
        MP4FileHandle hFile,
        uint32_t samplingRate,
        uint8_t fscod,
        uint8_t bsid,
        uint8_t bsmod,
        uint8_t acmod,
        uint8_t lfeon,
        uint8_t bit_rate_code)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->
                    AddAC3AudioTrack(samplingRate, fscod, bsid, bsmod, acmod, lfeon, bit_rate_code);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    MP4TrackId MP4AddEncAudioTrack(MP4FileHandle hFile,
                                   uint32_t timeScale,
                                   MP4Duration sampleDuration,
                                   mp4v2_ismacrypParams *icPp,
                                   uint8_t audioType)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                if (icPp == NULL) {
                    return ((MP4File*)hFile)->
                           AddEncAudioTrack(timeScale, sampleDuration, audioType,
                                            0, 0,
                                            0, 0,
                                            false, NULL, false);
                } else {
                    return ((MP4File*)hFile)->
                           AddEncAudioTrack(timeScale, sampleDuration, audioType,
                                            icPp->scheme_type, icPp->scheme_version,
                                            icPp->key_ind_len, icPp->iv_len,
                                            icPp->selective_enc, icPp->kms_uri, true);
                }
            } catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }
    MP4TrackId MP4AddAmrAudioTrack(
        MP4FileHandle hFile,
        uint32_t timeScale,
        uint16_t modeSet,
        uint8_t modeChangePeriod,
        uint8_t framesPerSample,
        bool isAmrWB)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->
                       AddAmrAudioTrack(timeScale, modeSet, modeChangePeriod, framesPerSample, isAmrWB);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    void MP4SetAmrVendor(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        uint32_t vendor)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->
                SetAmrVendor(trackId, vendor);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
    }

    void MP4SetAmrDecoderVersion(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        uint8_t decoderVersion)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->
                SetAmrDecoderVersion(trackId, decoderVersion);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
    }

    void MP4SetAmrModeSet(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        uint16_t modeSet)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->
                SetAmrModeSet(trackId, modeSet);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
    }

    uint16_t MP4GetAmrModeSet(
        MP4FileHandle hFile,
        MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->
                       GetAmrModeSet(trackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    MP4TrackId MP4AddHrefTrack (MP4FileHandle hFile,
                                uint32_t timeScale,
                                MP4Duration sampleDuration,
                                const char *base_url)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                MP4File *pFile = (MP4File *)hFile;

                return pFile->AddHrefTrack(timeScale,
                                           sampleDuration,
                                           base_url);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    const char *MP4GetHrefTrackBaseUrl (MP4FileHandle hFile,
                                        MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackStringProperty(trackId,
                        "mdia.minf.stbl.stsd.href.burl.base_url");
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return NULL;
    }

    MP4TrackId MP4AddVideoTrack(
        MP4FileHandle hFile,
        uint32_t timeScale,
        MP4Duration sampleDuration,
        uint16_t width,
        uint16_t height,
        uint8_t videoType)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                MP4File *pFile = (MP4File *)hFile;

                return pFile->AddMP4VideoTrack(timeScale,
                                               sampleDuration,
                                               width,
                                               height,
                                               videoType);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    MP4TrackId MP4AddEncVideoTrack(MP4FileHandle hFile,
                                   uint32_t timeScale,
                                   MP4Duration sampleDuration,
                                   uint16_t width,
                                   uint16_t height,
                                   mp4v2_ismacrypParams *icPp,
                                   uint8_t videoType,
                                   const char *oFormat)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {

                // test for valid ismacrypt session descriptor
                if (icPp == NULL) {
                    return MP4_INVALID_TRACK_ID;
                }
                MP4File *pFile = (MP4File *)hFile;

                return pFile->AddEncVideoTrack(timeScale,
                                               sampleDuration,
                                               width,
                                               height,
                                               videoType,
                                               icPp,
                                               oFormat);

            } catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    MP4TrackId MP4AddColr(
        MP4FileHandle hFile, MP4TrackId refTrackId, uint16_t pri, uint16_t tran, uint16_t mat)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->AddColr(refTrackId, pri, tran, mat);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }


    MP4TrackId MP4AddH264VideoTrack(MP4FileHandle hFile,
                                    uint32_t timeScale,
                                    MP4Duration sampleDuration,
                                    uint16_t width,
                                    uint16_t height,
                                    uint8_t AVCProfileIndication,
                                    uint8_t profile_compat,
                                    uint8_t AVCLevelIndication,
                                    uint8_t sampleLenFieldSizeMinusOne)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                MP4File *pFile = (MP4File *)hFile;

                return pFile->AddH264VideoTrack(timeScale,
                                                sampleDuration,
                                                width,
                                                height,
                                                AVCProfileIndication,
                                                profile_compat,
                                                AVCLevelIndication,
                                                sampleLenFieldSizeMinusOne);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    MP4TrackId MP4AddEncH264VideoTrack(
        MP4FileHandle hFile,
        uint32_t timeScale,
        MP4Duration sampleDuration,
        uint16_t width,
        uint16_t height,
        MP4FileHandle srcFile,
        MP4TrackId srcTrackId,
        mp4v2_ismacrypParams *icPp
    )

    {
        MP4Atom *srcAtom;
        MP4File *pFile;

        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {

                pFile = (MP4File *)srcFile;
                srcAtom = pFile->FindTrackAtom(srcTrackId, "mdia.minf.stbl.stsd.avc1.avcC");
                if (srcAtom == NULL)
                    return MP4_INVALID_TRACK_ID;

                pFile = (MP4File *)hFile;

                return pFile->AddEncH264VideoTrack(timeScale,
                                                   sampleDuration,
                                                   width,
                                                   height,
                                                   srcAtom,
                                                   icPp);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    void MP4AddH264SequenceParameterSet (MP4FileHandle hFile,
                                         MP4TrackId trackId,
                                         const uint8_t *pSequence,
                                         uint16_t sequenceLen)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                MP4File *pFile = (MP4File *)hFile;

                pFile->AddH264SequenceParameterSet(trackId,
                                                   pSequence,
                                                   sequenceLen);
                return;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return;
    }
    void MP4AddH264PictureParameterSet (MP4FileHandle hFile,
                                        MP4TrackId trackId,
                                        const uint8_t *pPict,
                                        uint16_t pictLen)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                MP4File *pFile = (MP4File *)hFile;

                pFile->AddH264PictureParameterSet(trackId,
                                                  pPict,
                                                  pictLen);
                return;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return;
    }

    MP4TrackId MP4AddH263VideoTrack(
        MP4FileHandle hFile,
        uint32_t timeScale,
        MP4Duration sampleDuration,
        uint16_t width,
        uint16_t height,
        uint8_t h263Level,
        uint8_t h263Profile,
        uint32_t avgBitrate,
        uint32_t maxBitrate)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->
                       AddH263VideoTrack(timeScale, sampleDuration, width, height, h263Level, h263Profile, avgBitrate, maxBitrate);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }

        return MP4_INVALID_TRACK_ID;
    }

    void MP4SetH263Vendor(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        uint32_t vendor)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->
                SetH263Vendor(trackId, vendor);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
    }

    void MP4SetH263DecoderVersion(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        uint8_t decoderVersion)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {

            try {
                ((MP4File*)hFile)->
                SetH263DecoderVersion(trackId, decoderVersion);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
    }

    void MP4SetH263Bitrates(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        uint32_t avgBitrate,
        uint32_t maxBitrate)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {

            try {
                ((MP4File*)hFile)->
                SetH263Bitrates(trackId, avgBitrate, maxBitrate);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
    }

    MP4TrackId MP4AddHintTrack(
        MP4FileHandle hFile, MP4TrackId refTrackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->AddHintTrack(refTrackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    MP4TrackId MP4AddTextTrack(
        MP4FileHandle hFile, MP4TrackId refTrackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->AddTextTrack(refTrackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    MP4TrackId MP4AddSubtitleTrack(MP4FileHandle hFile,
                                   uint32_t timescale,
                                   uint16_t width,
                                   uint16_t height)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->AddSubtitleTrack(timescale, width, height);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    MP4TrackId MP4AddChapterTextTrack(
        MP4FileHandle hFile, MP4TrackId refTrackId, uint32_t timescale)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->AddChapterTextTrack(refTrackId, timescale);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    MP4TrackId MP4AddPixelAspectRatio(
        MP4FileHandle hFile, MP4TrackId refTrackId, uint32_t hSpacing, uint32_t vSpacing)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->AddPixelAspectRatio(refTrackId, hSpacing, vSpacing);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    void MP4AddChapter(
        MP4FileHandle hFile, MP4TrackId chapterTrackId, MP4Duration chapterDuration, const char *chapterTitle)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->AddChapter(chapterTrackId, chapterDuration, chapterTitle);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
    }

    void MP4AddNeroChapter(
        MP4FileHandle hFile, MP4Timestamp chapterStart, const char *chapterTitle)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->AddNeroChapter(chapterStart, chapterTitle);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
    }

    MP4ChapterType MP4ConvertChapters(
        MP4FileHandle hFile, MP4ChapterType toChapterType)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile))
        {
            try {
                return ((MP4File*)hFile)->ConvertChapters(toChapterType);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4ChapterTypeNone;
    }

    MP4ChapterType MP4DeleteChapters(
        MP4FileHandle hFile, MP4ChapterType fromChapterType, MP4TrackId chapterTrackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->DeleteChapters(fromChapterType, chapterTrackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4ChapterTypeNone;
    }

    MP4ChapterType MP4GetChapters(
        MP4FileHandle hFile, MP4Chapter_t ** chapterList, uint32_t * chapterCount, MP4ChapterType fromChapterType)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetChapters(chapterList, chapterCount, fromChapterType);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4ChapterTypeNone;
    }

    MP4ChapterType MP4SetChapters(
        MP4FileHandle hFile, MP4Chapter_t * chapterList, uint32_t chapterCount, MP4ChapterType toChapterType)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->SetChapters(chapterList, chapterCount, toChapterType);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4ChapterTypeNone;
    }

    void MP4ChangeMovieTimeScale(
        MP4FileHandle hFile, uint32_t value)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->ChangeMovieTimeScale(value);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
    }

    MP4TrackId MP4CloneTrack (MP4FileHandle srcFile,
                              MP4TrackId srcTrackId,
                              MP4FileHandle dstFile,
                              MP4TrackId dstHintTrackReferenceTrack)
    {
        MP4TrackId dstTrackId = MP4_INVALID_TRACK_ID;

        if (dstFile == NULL) {
            dstFile = srcFile;
        }

        const char* trackType =
            MP4GetTrackType(srcFile, srcTrackId);

        if (!trackType) {
            return dstTrackId;
        }

        const char *media_data_name =
            MP4GetTrackMediaDataName(srcFile, srcTrackId);
        if (media_data_name == NULL) return dstTrackId;

        if (MP4_IS_VIDEO_TRACK_TYPE(trackType)) {
            if (ATOMID(media_data_name) == ATOMID("mp4v")) {
                MP4SetVideoProfileLevel(dstFile,
                                        MP4GetVideoProfileLevel(srcFile));
                dstTrackId = MP4AddVideoTrack(
                                 dstFile,
                                 MP4GetTrackTimeScale(srcFile,
                                                      srcTrackId),
                                 MP4GetTrackFixedSampleDuration(srcFile,
                                                                srcTrackId),
                                 MP4GetTrackVideoWidth(srcFile,
                                                       srcTrackId),
                                 MP4GetTrackVideoHeight(srcFile,
                                                        srcTrackId),
                                 MP4GetTrackEsdsObjectTypeId(srcFile,
                                                             srcTrackId));
            } else if (ATOMID(media_data_name) == ATOMID("avc1")) {
                uint8_t AVCProfileIndication;
                uint8_t profile_compat;
                uint8_t AVCLevelIndication;
                uint32_t sampleLenFieldSizeMinusOne;
                uint64_t temp;

                if (MP4GetTrackH264ProfileLevel(srcFile, srcTrackId,
                                                &AVCProfileIndication,
                                                &AVCLevelIndication) == false) {
                    return dstTrackId;
                }
                if (MP4GetTrackH264LengthSize(srcFile, srcTrackId,
                                              &sampleLenFieldSizeMinusOne) == false) {
                    return dstTrackId;
                }
                sampleLenFieldSizeMinusOne--;
                if (MP4GetTrackIntegerProperty(srcFile, srcTrackId,
                                               "mdia.minf.stbl.stsd.*[0].avcC.profile_compatibility",
                                               &temp) == false) return dstTrackId;
                profile_compat = temp & 0xff;

                dstTrackId = MP4AddH264VideoTrack(dstFile,
                                                  MP4GetTrackTimeScale(srcFile,
                                                                       srcTrackId),
                                                  MP4GetTrackFixedSampleDuration(srcFile,
                                                                                 srcTrackId),
                                                  MP4GetTrackVideoWidth(srcFile,
                                                                        srcTrackId),
                                                  MP4GetTrackVideoHeight(srcFile,
                                                                         srcTrackId),
                                                  AVCProfileIndication,
                                                  profile_compat,
                                                  AVCLevelIndication,
                                                  sampleLenFieldSizeMinusOne);
                uint8_t **seqheader, **pictheader;
                uint32_t *pictheadersize, *seqheadersize;
                uint32_t ix;
                MP4GetTrackH264SeqPictHeaders(srcFile, srcTrackId,
                                              &seqheader, &seqheadersize,
                                              &pictheader, &pictheadersize);
                for (ix = 0; seqheadersize[ix] != 0; ix++) {
                    MP4AddH264SequenceParameterSet(dstFile, dstTrackId,
                                                   seqheader[ix], seqheadersize[ix]);
                    free(seqheader[ix]);
                }
                free(seqheader);
                free(seqheadersize);
                for (ix = 0; pictheadersize[ix] != 0; ix++) {
                    MP4AddH264PictureParameterSet(dstFile, dstTrackId,
                                                  pictheader[ix], pictheadersize[ix]);
                    free(pictheader[ix]);
                }
                free(pictheader);
                free(pictheadersize);
            } else
                return dstTrackId;
        } else if (MP4_IS_AUDIO_TRACK_TYPE(trackType)) {
            if (ATOMID(media_data_name) != ATOMID("mp4a")) return dstTrackId;
            MP4SetAudioProfileLevel(dstFile,
                                    MP4GetAudioProfileLevel(srcFile));
            dstTrackId = MP4AddAudioTrack(
                             dstFile,
                             MP4GetTrackTimeScale(srcFile, srcTrackId),
                             MP4GetTrackFixedSampleDuration(srcFile, srcTrackId),
                             MP4GetTrackEsdsObjectTypeId(srcFile, srcTrackId));

        } else if (MP4_IS_OD_TRACK_TYPE(trackType)) {
            dstTrackId = MP4AddODTrack(dstFile);

        } else if (MP4_IS_SCENE_TRACK_TYPE(trackType)) {
            dstTrackId = MP4AddSceneTrack(dstFile);

        } else if (MP4_IS_HINT_TRACK_TYPE(trackType)) {
            if (dstHintTrackReferenceTrack == MP4_INVALID_TRACK_ID) {
                dstTrackId = MP4_INVALID_TRACK_ID;
            } else {
                dstTrackId = MP4AddHintTrack(
                                 dstFile,
                                 dstHintTrackReferenceTrack);
            }

        } else if (MP4_IS_SYSTEMS_TRACK_TYPE(trackType)) {
            dstTrackId = MP4AddSystemsTrack(dstFile, trackType);

        } else {
            dstTrackId = MP4AddTrack(dstFile, trackType);
        }

        if (dstTrackId == MP4_INVALID_TRACK_ID) {
            return dstTrackId;
        }

        MP4SetTrackTimeScale(
            dstFile,
            dstTrackId,
            MP4GetTrackTimeScale(srcFile, srcTrackId));

        if (MP4_IS_AUDIO_TRACK_TYPE(trackType)
                || MP4_IS_VIDEO_TRACK_TYPE(trackType)) {
            // copy track ES configuration
            uint8_t* pConfig = NULL;
            uint32_t configSize = 0;
            uint32_t verb = MP4GetVerbosity(srcFile);
            MP4SetVerbosity(srcFile, verb & ~(MP4_DETAILS_ERROR));
            bool haveEs = MP4GetTrackESConfiguration(srcFile,
                          srcTrackId,
                          &pConfig,
                          &configSize);
            MP4SetVerbosity(srcFile, verb);
            if (haveEs &&
                    pConfig != NULL && configSize != 0) {
                if (!MP4SetTrackESConfiguration(
                            dstFile,
                            dstTrackId,
                            pConfig,
                            configSize)) {
                    free(pConfig);
                    MP4DeleteTrack(dstFile, dstTrackId);
                    return MP4_INVALID_TRACK_ID;
                }

                free(pConfig);
            }
        }

        if (MP4_IS_HINT_TRACK_TYPE(trackType)) {
            // probably not exactly what is wanted
            // but caller can adjust later to fit their desires

            char* payloadName = NULL;
            char *encodingParms = NULL;
            uint8_t payloadNumber;
            uint16_t maxPayloadSize;

            if (MP4GetHintTrackRtpPayload(
                        srcFile,
                        srcTrackId,
                        &payloadName,
                        &payloadNumber,
                        &maxPayloadSize,
                        &encodingParms)) {

                if (MP4SetHintTrackRtpPayload(
                            dstFile,
                            dstTrackId,
                            payloadName,
                            &payloadNumber,
                            maxPayloadSize,
                            encodingParms) == false) {
                    MP4DeleteTrack(dstFile, dstTrackId);
                    return MP4_INVALID_TRACK_ID;
                }
            }
#if 0
            MP4SetHintTrackSdp(
                dstFile,
                dstTrackId,
                MP4GetHintTrackSdp(srcFile, srcTrackId));
#endif
        }

        return dstTrackId;
    }

// Given a track, make an encrypted clone of it in the dest. file
    MP4TrackId MP4EncAndCloneTrack(MP4FileHandle srcFile,
                                   MP4TrackId srcTrackId,
                                   mp4v2_ismacrypParams *icPp,
                                   MP4FileHandle dstFile,
                                   MP4TrackId dstHintTrackReferenceTrack
                                  )
    {
        const char *oFormat;

        MP4TrackId dstTrackId = MP4_INVALID_TRACK_ID;

        if (dstFile == NULL) {
            dstFile = srcFile;
        }

        const char* trackType = MP4GetTrackType(srcFile, srcTrackId);

        if (!trackType) {
            return dstTrackId;
        }

        if (MP4_IS_VIDEO_TRACK_TYPE(trackType)) {

            // test source file format for avc1
            oFormat = MP4GetTrackMediaDataName(srcFile, srcTrackId);
            if (!strcasecmp(oFormat, "avc1"))
            {
                dstTrackId = MP4AddEncH264VideoTrack(dstFile,
                                                     MP4GetTrackTimeScale(srcFile, srcTrackId),
                                                     MP4GetTrackFixedSampleDuration(srcFile, srcTrackId),
                                                     MP4GetTrackVideoWidth(srcFile, srcTrackId),
                                                     MP4GetTrackVideoHeight(srcFile, srcTrackId),
                                                     srcFile,
                                                     srcTrackId,
                                                     icPp
                                                    );
            }
            else
            {
                MP4SetVideoProfileLevel(dstFile, MP4GetVideoProfileLevel(srcFile));
                dstTrackId = MP4AddEncVideoTrack(dstFile,
                                                 MP4GetTrackTimeScale(srcFile, srcTrackId),
                                                 MP4GetTrackFixedSampleDuration(srcFile, srcTrackId),
                                                 MP4GetTrackVideoWidth(srcFile, srcTrackId),
                                                 MP4GetTrackVideoHeight(srcFile, srcTrackId),
                                                 icPp,
                                                 MP4GetTrackEsdsObjectTypeId(srcFile, srcTrackId),
                                                 oFormat
                                                );
            }

        } else if (MP4_IS_AUDIO_TRACK_TYPE(trackType)) {
            MP4SetAudioProfileLevel(dstFile, MP4GetAudioProfileLevel(srcFile));
            dstTrackId = MP4AddEncAudioTrack(dstFile,
                                             MP4GetTrackTimeScale(srcFile, srcTrackId),
                                             MP4GetTrackFixedSampleDuration(srcFile,
                                                                            srcTrackId),
                                             icPp,
                                             MP4GetTrackEsdsObjectTypeId(srcFile,
                                                                         srcTrackId)
                                            );

        } else if (MP4_IS_OD_TRACK_TYPE(trackType)) {
            dstTrackId = MP4AddODTrack(dstFile);

        } else if (MP4_IS_SCENE_TRACK_TYPE(trackType)) {
            dstTrackId = MP4AddSceneTrack(dstFile);

        } else if (MP4_IS_HINT_TRACK_TYPE(trackType)) {
            if (dstHintTrackReferenceTrack == MP4_INVALID_TRACK_ID) {
                dstTrackId = MP4_INVALID_TRACK_ID;
            } else {
                dstTrackId = MP4AddHintTrack(dstFile,
                                             MP4GetHintTrackReferenceTrackId(srcFile,
                                                                             srcTrackId));
            }
        } else if (MP4_IS_SYSTEMS_TRACK_TYPE(trackType)) {
            dstTrackId = MP4AddSystemsTrack(dstFile, trackType);

        } else {
            dstTrackId = MP4AddTrack(dstFile, trackType);
        }

        if (dstTrackId == MP4_INVALID_TRACK_ID) {
            return dstTrackId;
        }

        MP4SetTrackTimeScale(dstFile,
                             dstTrackId,
                             MP4GetTrackTimeScale(srcFile, srcTrackId));

        if (MP4_IS_AUDIO_TRACK_TYPE(trackType)
                || MP4_IS_VIDEO_TRACK_TYPE(trackType)) {
            // copy track ES configuration
            uint8_t* pConfig = NULL;
            uint32_t configSize = 0;
            if (MP4GetTrackESConfiguration(srcFile, srcTrackId,
                                           &pConfig, &configSize)) {

                if (pConfig != NULL) {
                    MP4SetTrackESConfiguration(dstFile, dstTrackId,
                                               pConfig, configSize);
                }
            }
            if (pConfig != NULL)
                free(pConfig);
        }

        // Bill's change to MP4CloneTrack
        if (MP4_IS_HINT_TRACK_TYPE(trackType)) {
            // probably not exactly what is wanted
            // but caller can adjust later to fit their desires

            char* payloadName = NULL;
            char *encodingParms = NULL;
            uint8_t payloadNumber;
            uint16_t maxPayloadSize;

            if (MP4GetHintTrackRtpPayload(
                        srcFile,
                        srcTrackId,
                        &payloadName,
                        &payloadNumber,
                        &maxPayloadSize,
                        &encodingParms)) {

                (void)MP4SetHintTrackRtpPayload(
                    dstFile,
                    dstTrackId,
                    payloadName,
                    &payloadNumber,
                    maxPayloadSize,
                    encodingParms);
            }
#if 0
            MP4SetHintTrackSdp(
                dstFile,
                dstTrackId,
                MP4GetHintTrackSdp(srcFile, srcTrackId));
#endif
        }

        return dstTrackId;
    }

    MP4TrackId MP4CopyTrack(MP4FileHandle srcFile,
                            MP4TrackId srcTrackId,
                            MP4FileHandle dstFile,
                            bool applyEdits,
                            MP4TrackId dstHintTrackReferenceTrack)
    {
        bool copySamples = true;  // LATER allow false => reference samples

        MP4TrackId dstTrackId =
            MP4CloneTrack(srcFile, srcTrackId, dstFile, dstHintTrackReferenceTrack);

        if (dstTrackId == MP4_INVALID_TRACK_ID) {
            return dstTrackId;
        }

        bool viaEdits =
            applyEdits && MP4GetTrackNumberOfEdits(srcFile, srcTrackId);

        MP4SampleId sampleId = 0;
        MP4SampleId numSamples =
            MP4GetTrackNumberOfSamples(srcFile, srcTrackId);

        MP4Timestamp when = 0;
        MP4Duration editsDuration =
            MP4GetTrackEditTotalDuration(srcFile, srcTrackId);

        while (true) {
            MP4Duration sampleDuration = MP4_INVALID_DURATION;

            if (viaEdits) {
                sampleId = MP4GetSampleIdFromEditTime(
                               srcFile,
                               srcTrackId,
                               when,
                               NULL,
                               &sampleDuration);

                // in theory, this shouldn't happen
                if (sampleId == MP4_INVALID_SAMPLE_ID) {
                    MP4DeleteTrack(dstFile, dstTrackId);
                    return MP4_INVALID_TRACK_ID;
                }

                when += sampleDuration;

                if (when >= editsDuration) {
                    break;
                }
            } else {
                sampleId++;
                if (sampleId > numSamples) {
                    break;
                }
            }

            bool rc = false;

            if (copySamples) {
                rc = MP4CopySample(
                         srcFile,
                         srcTrackId,
                         sampleId,
                         dstFile,
                         dstTrackId,
                         sampleDuration);

            } else {
                rc = MP4ReferenceSample(
                         srcFile,
                         srcTrackId,
                         sampleId,
                         dstFile,
                         dstTrackId,
                         sampleDuration);
            }

            if (!rc) {
                MP4DeleteTrack(dstFile, dstTrackId);
                return MP4_INVALID_TRACK_ID;
            }
        }

        return dstTrackId;
    }

// Given a source track in a source file, make an encrypted copy of
// the track in the destination file, including sample encryption
    MP4TrackId MP4EncAndCopyTrack(MP4FileHandle srcFile,
                                  MP4TrackId srcTrackId,
                                  mp4v2_ismacrypParams *icPp,
                                  encryptFunc_t encfcnp,
                                  uint32_t encfcnparam1,
                                  MP4FileHandle dstFile,
                                  bool applyEdits,
                                  MP4TrackId dstHintTrackReferenceTrack
                                 )
    {
        bool copySamples = true;  // LATER allow false => reference samples

        MP4TrackId dstTrackId =
            MP4EncAndCloneTrack(srcFile, srcTrackId,
                                icPp,
                                dstFile, dstHintTrackReferenceTrack);

        if (dstTrackId == MP4_INVALID_TRACK_ID) {
            return dstTrackId;
        }

        bool viaEdits =
            applyEdits && MP4GetTrackNumberOfEdits(srcFile, srcTrackId);

        MP4SampleId sampleId = 0;
        MP4SampleId numSamples =
            MP4GetTrackNumberOfSamples(srcFile, srcTrackId);

        MP4Timestamp when = 0;
        MP4Duration editsDuration =
            MP4GetTrackEditTotalDuration(srcFile, srcTrackId);

        while (true) {
            MP4Duration sampleDuration = MP4_INVALID_DURATION;

            if (viaEdits) {
                sampleId = MP4GetSampleIdFromEditTime(srcFile,
                                                      srcTrackId,
                                                      when,
                                                      NULL,
                                                      &sampleDuration);

                // in theory, this shouldn't happen
                if (sampleId == MP4_INVALID_SAMPLE_ID) {
                    MP4DeleteTrack(dstFile, dstTrackId);
                    return MP4_INVALID_TRACK_ID;
                }

                when += sampleDuration;

                if (when >= editsDuration) {
                    break;
                }
            } else {
                sampleId++;
                if (sampleId > numSamples) {
                    break;
                }
            }

            bool rc = false;

            if (copySamples) {
                // encrypt and copy
                rc = MP4EncAndCopySample(srcFile,
                                         srcTrackId,
                                         sampleId,
                                         encfcnp,
                                         encfcnparam1,
                                         dstFile,
                                         dstTrackId,
                                         sampleDuration);

            } else {
                // not sure what these are - encrypt?
                rc = MP4ReferenceSample(srcFile,
                                        srcTrackId,
                                        sampleId,
                                        dstFile,
                                        dstTrackId,
                                        sampleDuration);
            }

            if (!rc) {
                MP4DeleteTrack(dstFile, dstTrackId);
                return MP4_INVALID_TRACK_ID;
            }
        }

        return dstTrackId;
    }

    void MP4DeleteTrack(
        MP4FileHandle hFile,
        MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->DeleteTrack(trackId);
                return ;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return;
    }

    uint32_t MP4GetNumberOfTracks(
        MP4FileHandle hFile,
        const char* type,
        uint8_t subType)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetNumberOfTracks(type, subType);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    MP4TrackId MP4FindTrackId(
        MP4FileHandle hFile,
        uint16_t index,
        const char* type,
        uint8_t subType)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->FindTrackId(index, type, subType);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    uint16_t MP4FindTrackIndex(
        MP4FileHandle hFile, MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->FindTrackIndex(trackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return (uint16_t)-1;
    }

    /* specific track properties */

    const char* MP4GetTrackType(
        MP4FileHandle hFile, MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackType(trackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return NULL;
    }
    const char* MP4GetTrackMediaDataName(
        MP4FileHandle hFile, MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackMediaDataName(trackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return NULL;
    }

    bool MP4GetTrackMediaDataOriginalFormat(
        MP4FileHandle hFile, MP4TrackId trackId, char *originalFormat,
        uint32_t buflen)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {

                return ((MP4File*)hFile)->GetTrackMediaDataOriginalFormat(trackId,
                        originalFormat, buflen);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    MP4Duration MP4GetTrackDuration(
        MP4FileHandle hFile, MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackDuration(trackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_DURATION;
    }

    uint32_t MP4GetTrackTimeScale(
        MP4FileHandle hFile, MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackTimeScale(trackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    void MP4SetTrackTimeScale(
        MP4FileHandle hFile, MP4TrackId trackId, uint32_t value)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetTrackTimeScale(trackId, value);
                return;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return;
    }

    uint8_t MP4GetTrackAudioMpeg4Type(
        MP4FileHandle hFile, MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackAudioMpeg4Type(trackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_MPEG4_INVALID_AUDIO_TYPE;
    }



// Replacement to MP4GetTrackVideoType and MP4GetTrackAudioType
// Basically does the same thing but with a more self-explanatory name
    uint8_t MP4GetTrackEsdsObjectTypeId(
        MP4FileHandle hFile, MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {

                return ((MP4File*)hFile)->GetTrackEsdsObjectTypeId(trackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_AUDIO_TYPE;
    }

    MP4Duration MP4GetTrackFixedSampleDuration(
        MP4FileHandle hFile, MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackFixedSampleDuration(trackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_DURATION;
    }

    uint32_t MP4GetTrackBitRate(
        MP4FileHandle hFile, MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            MP4File *pFile = (MP4File *)hFile;
            try {
                // 2010-05-27 K.Takata
                uint32_t bitrate = pFile->GetTrackIntegerProperty(trackId,
                                                      "mdia.minf.stbl.stsd.*.esds.decConfigDescr.avgBitrate");
                if (bitrate != 0) {
                    return bitrate;
                }
            }
            catch (MP4Error* e) {
                //PRINT_ERROR(e);  we don't really need to print this.
                delete e;
            }
            // if we're here, we can't get the bitrate from above -
            // lets calculate it
            try {
                MP4Duration trackDur;
                trackDur = MP4GetTrackDuration(hFile, trackId);
                uint64_t msDuration =
                    pFile->ConvertFromTrackDuration(trackId, trackDur,
                                                    MP4_MSECS_TIME_SCALE);
                if (msDuration == 0) return 0;

                MP4Track *pTrack = pFile->GetTrack(trackId);
                uint64_t bytes = pTrack->GetTotalOfSampleSizes();
                bytes *= UINT64_C(8 * 1000);
                bytes /= msDuration;
                return (uint32_t)bytes;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e); // print this one.
                delete e;
            }

        }
        return 0;
    }

    bool MP4GetTrackESConfiguration(
        MP4FileHandle hFile, MP4TrackId trackId,
        uint8_t** ppConfig, uint32_t* pConfigSize)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->GetTrackESConfiguration(
                    trackId, ppConfig, pConfigSize);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        *ppConfig = NULL;
        *pConfigSize = 0;
        return false;
    }
    bool MP4GetTrackVideoMetadata(
        MP4FileHandle hFile, MP4TrackId trackId,
        uint8_t** ppConfig, uint32_t* pConfigSize)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->GetTrackVideoMetadata(
                    trackId, ppConfig, pConfigSize);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        *ppConfig = NULL;
        *pConfigSize = 0;
        return false;
    }

    bool MP4SetTrackESConfiguration(
        MP4FileHandle hFile, MP4TrackId trackId,
        const uint8_t* pConfig, uint32_t configSize)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetTrackESConfiguration(
                    trackId, pConfig, configSize);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4GetTrackH264ProfileLevel (MP4FileHandle hFile,
                                      MP4TrackId trackId,
                                      uint8_t *pProfile,
                                      uint8_t *pLevel)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                *pProfile =
                    ((MP4File *)hFile)->GetTrackIntegerProperty(trackId,
                            "mdia.minf.stbl.stsd.*[0].avcC.AVCProfileIndication");
                *pLevel =
                    ((MP4File *)hFile)->GetTrackIntegerProperty(trackId,
                            "mdia.minf.stbl.stsd.*[0].avcC.AVCLevelIndication");

                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }
    void MP4GetTrackH264SeqPictHeaders (MP4FileHandle hFile,
                                        MP4TrackId trackId,
                                        uint8_t ***pSeqHeader,
                                        uint32_t **pSeqHeaderSize,
                                        uint8_t ***pPictHeader,
                                        uint32_t **pPictHeaderSize)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->GetTrackH264SeqPictHeaders(trackId,
                        pSeqHeader,
                        pSeqHeaderSize,
                        pPictHeader,
                        pPictHeaderSize);
                return;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return;
    }
    bool MP4GetTrackH264LengthSize (MP4FileHandle hFile,
                                    MP4TrackId trackId,
                                    uint32_t *pLength)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                *pLength = 1 +
                           ((MP4File*) hFile)->GetTrackIntegerProperty(trackId,
                                   "mdia.minf.stbl.stsd.*[0].avcC.lengthSizeMinusOne");
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    MP4SampleId MP4GetTrackNumberOfSamples(
        MP4FileHandle hFile, MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackNumberOfSamples(trackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    uint16_t MP4GetTrackVideoWidth(
        MP4FileHandle hFile, MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackIntegerProperty(trackId,
                        "mdia.minf.stbl.stsd.*.width");
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    uint16_t MP4GetTrackVideoHeight(
        MP4FileHandle hFile, MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackIntegerProperty(trackId,
                        "mdia.minf.stbl.stsd.*.height");
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    double MP4GetTrackVideoFrameRate(
        MP4FileHandle hFile, MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackVideoFrameRate(trackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0.0;
    }

    int MP4GetTrackAudioChannels (MP4FileHandle hFile,
                                  MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackAudioChannels(trackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return -1;
    }

// returns true if the track is a media track encrypted according to ismacryp
    bool MP4IsIsmaCrypMediaTrack(
        MP4FileHandle hFile, MP4TrackId trackId)
    {
        bool retval = false;
        uint32_t verb = MP4GetVerbosity(hFile);
        MP4SetVerbosity(hFile, verb & ~(MP4_DETAILS_ERROR));

        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                retval = ((MP4File*)hFile)->IsIsmaCrypMediaTrack(trackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        MP4SetVerbosity(hFile, verb);
        return retval;
    }


    /* generic track properties */

    bool MP4HaveTrackAtom (MP4FileHandle hFile,
                           MP4TrackId trackId,
                           const char *atomName)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->FindTrackAtom(trackId, atomName) != NULL;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4GetTrackIntegerProperty (
        MP4FileHandle hFile, MP4TrackId trackId,
        const char* propName,
        uint64_t *retvalue)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                *retvalue = ((MP4File*)hFile)->GetTrackIntegerProperty(trackId,
                            propName);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4GetTrackFloatProperty(
        MP4FileHandle hFile, MP4TrackId trackId,
        const char* propName,
        float *retvalue)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                *retvalue = ((MP4File*)hFile)->GetTrackFloatProperty(trackId, propName);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4GetTrackStringProperty(
        MP4FileHandle hFile, MP4TrackId trackId,
        const char* propName,
        const char **retvalue)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                *retvalue = ((MP4File*)hFile)->GetTrackStringProperty(trackId, propName);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4GetTrackBytesProperty(
        MP4FileHandle hFile, MP4TrackId trackId, const char* propName,
        uint8_t** ppValue, uint32_t* pValueSize)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->GetTrackBytesProperty(
                    trackId, propName, ppValue, pValueSize);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        *ppValue = NULL;
        *pValueSize = 0;
        return false;
    }

    bool MP4SetTrackIntegerProperty(
        MP4FileHandle hFile, MP4TrackId trackId,
        const char* propName, int64_t value)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetTrackIntegerProperty(trackId,
                        propName, value);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4SetTrackFloatProperty(
        MP4FileHandle hFile, MP4TrackId trackId,
        const char* propName, float value)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetTrackFloatProperty(trackId, propName, value);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4SetTrackStringProperty(
        MP4FileHandle hFile, MP4TrackId trackId,
        const char* propName, const char* value)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetTrackStringProperty(trackId, propName, value);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4SetTrackBytesProperty(
        MP4FileHandle hFile, MP4TrackId trackId,
        const char* propName, const uint8_t* pValue, uint32_t valueSize)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetTrackBytesProperty(
                    trackId, propName, pValue, valueSize);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    /* sample operations */

    bool MP4ReadSample(
        /* input parameters */
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4SampleId sampleId,
        /* output parameters */
        uint8_t** ppBytes,
        uint32_t* pNumBytes,
        MP4Timestamp* pStartTime,
        MP4Duration* pDuration,
        MP4Duration* pRenderingOffset,
        bool* pIsSyncSample)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->ReadSample(
                    trackId,
                    sampleId,
                    ppBytes,
                    pNumBytes,
                    pStartTime,
                    pDuration,
                    pRenderingOffset,
                    pIsSyncSample);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        *pNumBytes = 0;
        return false;
    }

    bool MP4ReadSampleFromTime(
        /* input parameters */
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4Timestamp when,
        /* output parameters */
        uint8_t** ppBytes,
        uint32_t* pNumBytes,
        MP4Timestamp* pStartTime,
        MP4Duration* pDuration,
        MP4Duration* pRenderingOffset,
        bool* pIsSyncSample)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                MP4SampleId sampleId =
                    ((MP4File*)hFile)->GetSampleIdFromTime(
                        trackId, when, false);

                ((MP4File*)hFile)->ReadSample(
                    trackId,
                    sampleId,
                    ppBytes,
                    pNumBytes,
                    pStartTime,
                    pDuration,
                    pRenderingOffset,
                    pIsSyncSample);

                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        *pNumBytes = 0;
        return false;
    }

    bool MP4WriteSample(
        MP4FileHandle  hFile,
        MP4TrackId     trackId,
        const uint8_t* pBytes,
        uint32_t       numBytes,
        MP4Duration    duration,
        MP4Duration    renderingOffset,
        bool           isSyncSample )
    {
        if( MP4_IS_VALID_FILE_HANDLE( hFile )) {
            try {
                ((MP4File*)hFile)->WriteSample(
                    trackId,
                    pBytes,
                    numBytes,
                    duration,
                    renderingOffset,
                    isSyncSample );
                return true;
            }
            catch( MP4Error* e ) {
                PRINT_ERROR( e );
                delete e;
            }
        }
        return false;
    }

    bool MP4WriteSampleDependency(
        MP4FileHandle  hFile,
        MP4TrackId     trackId,
        const uint8_t* pBytes,
        uint32_t       numBytes,
        MP4Duration    duration,
        MP4Duration    renderingOffset,
        bool           isSyncSample,
        uint32_t       dependencyFlags )
    {
        if( MP4_IS_VALID_FILE_HANDLE( hFile )) {
            try {
                ((MP4File*)hFile)->WriteSampleDependency(
                    trackId,
                    pBytes,
                    numBytes,
                    duration,
                    renderingOffset,
                    isSyncSample,
                    dependencyFlags );
                return true;
            }
            catch( MP4Error* e ) {
                PRINT_ERROR( e );
                delete e;
            }
        }
        return false;
    }

    bool MP4CopySample(
        MP4FileHandle srcFile,
        MP4TrackId srcTrackId,
        MP4SampleId srcSampleId,
        MP4FileHandle dstFile,
        MP4TrackId dstTrackId,
        MP4Duration dstSampleDuration)
    {
        bool rc;
        uint8_t* pBytes = NULL;
        uint32_t numBytes = 0;
        MP4Duration sampleDuration;
        MP4Duration renderingOffset;
        bool isSyncSample;

        // Note: we leave it up to the caller to ensure that the
        // source and destination tracks are compatible.
        // i.e. copying audio samples into a video track
        // is unlikely to do anything useful

        rc = MP4ReadSample(
                 srcFile,
                 srcTrackId,
                 srcSampleId,
                 &pBytes,
                 &numBytes,
                 NULL,
                 &sampleDuration,
                 &renderingOffset,
                 &isSyncSample);

        if (!rc) {
            return false;
        }

        if (dstFile == MP4_INVALID_FILE_HANDLE) {
            dstFile = srcFile;
        }
        if (dstTrackId == MP4_INVALID_TRACK_ID) {
            dstTrackId = srcTrackId;
        }
        if (dstSampleDuration != MP4_INVALID_DURATION) {
            sampleDuration = dstSampleDuration;
        }

        rc = MP4WriteSample(
                 dstFile,
                 dstTrackId,
                 pBytes,
                 numBytes,
                 sampleDuration,
                 renderingOffset,
                 isSyncSample);

        free(pBytes);

        return rc;
    }

    bool MP4EncAndCopySample(
        MP4FileHandle srcFile,
        MP4TrackId srcTrackId,
        MP4SampleId srcSampleId,
        encryptFunc_t encfcnp,
        uint32_t encfcnparam1,
        MP4FileHandle dstFile,
        MP4TrackId dstTrackId,
        MP4Duration dstSampleDuration)
    {
        bool rc;
        uint8_t* pBytes = NULL;
        uint32_t numBytes = 0;
        uint8_t* encSampleData = NULL;
        uint32_t encSampleLength = 0;
        MP4Duration sampleDuration;
        MP4Duration renderingOffset;
        bool isSyncSample;

        // Note: we leave it up to the caller to ensure that the
        // source and destination tracks are compatible.
        // i.e. copying audio samples into a video track
        // is unlikely to do anything useful

        rc = MP4ReadSample(
                 srcFile,
                 srcTrackId,
                 srcSampleId,
                 &pBytes,
                 &numBytes,
                 NULL,
                 &sampleDuration,
                 &renderingOffset,
                 &isSyncSample);

        if (!rc) {
            return false;
        }

        if (dstFile == MP4_INVALID_FILE_HANDLE) {
            dstFile = srcFile;
        }
        if (dstTrackId == MP4_INVALID_TRACK_ID) {
            dstTrackId = srcTrackId;
        }
        if (dstSampleDuration != MP4_INVALID_DURATION) {
            sampleDuration = dstSampleDuration;
        }

        //if (ismacrypEncryptSampleAddHeader(ismaCryptSId, numBytes, pBytes,
        //                        &encSampleLength, &encSampleData) != 0) {
        if (encfcnp(encfcnparam1, numBytes, pBytes,
                    &encSampleLength, &encSampleData) != 0) {
            fprintf(stderr,
                    "Can't encrypt the sample and add its header %u\n",
                    srcSampleId);
        }

        rc = MP4WriteSample(
                 dstFile,
                 dstTrackId,
                 encSampleData,
                 encSampleLength,
                 sampleDuration,
                 renderingOffset,
                 isSyncSample);

        free(pBytes);

        if (encSampleData != NULL) {
            free(encSampleData);
        }

        return rc;
    }

    bool MP4ReferenceSample(
        MP4FileHandle srcFile,
        MP4TrackId srcTrackId,
        MP4SampleId srcSampleId,
        MP4FileHandle dstFile,
        MP4TrackId dstTrackId,
        MP4Duration dstSampleDuration)
    {
        // LATER Not yet implemented
        return false;
    }

    uint32_t MP4GetSampleSize(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4SampleId sampleId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetSampleSize(
                           trackId, sampleId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    uint32_t MP4GetTrackMaxSampleSize(
        MP4FileHandle hFile,
        MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackMaxSampleSize(trackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    MP4SampleId MP4GetSampleIdFromTime(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4Timestamp when,
        bool wantSyncSample)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetSampleIdFromTime(
                           trackId, when, wantSyncSample);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_SAMPLE_ID;
    }

    MP4Timestamp MP4GetSampleTime(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4SampleId sampleId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetSampleTime(
                           trackId, sampleId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TIMESTAMP;
    }

    MP4Duration MP4GetSampleDuration(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4SampleId sampleId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetSampleDuration(
                           trackId, sampleId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_DURATION;
    }

    MP4Duration MP4GetSampleRenderingOffset(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4SampleId sampleId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetSampleRenderingOffset(
                           trackId, sampleId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_DURATION;
    }

    bool MP4SetSampleRenderingOffset(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4SampleId sampleId,
        MP4Duration renderingOffset)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetSampleRenderingOffset(
                    trackId, sampleId, renderingOffset);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    int8_t MP4GetSampleSync(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4SampleId sampleId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetSampleSync(
                           trackId, sampleId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return -1;
    }


    uint64_t MP4ConvertFromMovieDuration(
        MP4FileHandle hFile,
        MP4Duration duration,
        uint32_t timeScale)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->ConvertFromMovieDuration(
                           duration, timeScale);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return (uint64_t)MP4_INVALID_DURATION;
    }

    uint64_t MP4ConvertFromTrackTimestamp(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4Timestamp timeStamp,
        uint32_t timeScale)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->ConvertFromTrackTimestamp(
                           trackId, timeStamp, timeScale);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return (uint64_t)MP4_INVALID_TIMESTAMP;
    }

    MP4Timestamp MP4ConvertToTrackTimestamp(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        uint64_t timeStamp,
        uint32_t timeScale)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->ConvertToTrackTimestamp(
                           trackId, timeStamp, timeScale);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TIMESTAMP;
    }

    uint64_t MP4ConvertFromTrackDuration(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4Duration duration,
        uint32_t timeScale)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->ConvertFromTrackDuration(
                           trackId, duration, timeScale);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return (uint64_t)MP4_INVALID_DURATION;
    }

    MP4Duration MP4ConvertToTrackDuration(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        uint64_t duration,
        uint32_t timeScale)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->ConvertToTrackDuration(
                           trackId, duration, timeScale);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_DURATION;
    }

    bool MP4GetHintTrackRtpPayload(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId,
        char** ppPayloadName,
        uint8_t* pPayloadNumber,
        uint16_t* pMaxPayloadSize,
        char **ppEncodingParams)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->GetHintTrackRtpPayload(
                    hintTrackId, ppPayloadName, pPayloadNumber, pMaxPayloadSize,
                    ppEncodingParams);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4SetHintTrackRtpPayload(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId,
        const char* pPayloadName,
        uint8_t* pPayloadNumber,
        uint16_t maxPayloadSize,
        const char *encode_params,
        bool include_rtp_map,
        bool include_mpeg4_esid)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetHintTrackRtpPayload(
                    hintTrackId, pPayloadName, pPayloadNumber, maxPayloadSize, encode_params,
                    include_rtp_map, include_mpeg4_esid);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    const char* MP4GetSessionSdp(
        MP4FileHandle hFile)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetSessionSdp();
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return NULL;
    }

    bool MP4SetSessionSdp(
        MP4FileHandle hFile,
        const char* sdpString)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetSessionSdp(sdpString);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4AppendSessionSdp(
        MP4FileHandle hFile,
        const char* sdpString)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->AppendSessionSdp(sdpString);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    const char* MP4GetHintTrackSdp(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetHintTrackSdp(hintTrackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return NULL;
    }

    bool MP4SetHintTrackSdp(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId,
        const char* sdpString)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetHintTrackSdp(hintTrackId, sdpString);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4AppendHintTrackSdp(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId,
        const char* sdpString)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->AppendHintTrackSdp(hintTrackId, sdpString);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    MP4TrackId MP4GetHintTrackReferenceTrackId(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->
                       GetHintTrackReferenceTrackId(hintTrackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TRACK_ID;
    }

    bool MP4ReadRtpHint(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId,
        MP4SampleId hintSampleId,
        uint16_t* pNumPackets)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->ReadRtpHint(
                    hintTrackId, hintSampleId, pNumPackets);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    uint16_t MP4GetRtpHintNumberOfPackets(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetRtpHintNumberOfPackets(hintTrackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    int8_t MP4GetRtpPacketBFrame(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId,
        uint16_t packetIndex)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->
                       GetRtpPacketBFrame(hintTrackId, packetIndex);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return -1;
    }

    int32_t MP4GetRtpPacketTransmitOffset(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId,
        uint16_t packetIndex)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->
                       GetRtpPacketTransmitOffset(hintTrackId, packetIndex);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    bool MP4ReadRtpPacket(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId,
        uint16_t packetIndex,
        uint8_t** ppBytes,
        uint32_t* pNumBytes,
        uint32_t ssrc,
        bool includeHeader,
        bool includePayload)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->ReadRtpPacket(
                    hintTrackId, packetIndex,
                    ppBytes, pNumBytes,
                    ssrc, includeHeader, includePayload);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    MP4Timestamp MP4GetRtpTimestampStart(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetRtpTimestampStart(hintTrackId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TIMESTAMP;
    }

    bool MP4SetRtpTimestampStart(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId,
        MP4Timestamp rtpStart)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetRtpTimestampStart(
                    hintTrackId, rtpStart);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4AddRtpHint(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId)
    {
        return MP4AddRtpVideoHint(hFile, hintTrackId, false, 0);
    }

    bool MP4AddRtpVideoHint(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId,
        bool isBframe,
        uint32_t timestampOffset)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->AddRtpHint(hintTrackId,
                                              isBframe, timestampOffset);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4AddRtpPacket(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId,
        bool setMbit,
        int32_t transmitOffset)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->AddRtpPacket(
                    hintTrackId, setMbit, transmitOffset);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4AddRtpImmediateData(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId,
        const uint8_t* pBytes,
        uint32_t numBytes)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->AddRtpImmediateData(hintTrackId,
                                                       pBytes, numBytes);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4AddRtpSampleData(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId,
        MP4SampleId sampleId,
        uint32_t dataOffset,
        uint32_t dataLength)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->AddRtpSampleData(
                    hintTrackId, sampleId, dataOffset, dataLength);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4AddRtpESConfigurationPacket(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->AddRtpESConfigurationPacket(hintTrackId);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4WriteRtpHint(
        MP4FileHandle hFile,
        MP4TrackId hintTrackId,
        MP4Duration duration,
        bool isSyncSample)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->WriteRtpHint(
                    hintTrackId, duration, isSyncSample);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }
    /* 3GPP specific operations */

    bool MP4Make3GPCompliant(
        const char* fileName,
        uint32_t verbosity,
        char* majorBrand,
        uint32_t minorVersion,
        char** supportedBrands,
        uint32_t supportedBrandsCount,
        bool deleteIodsAtom)
    {
        MP4File* pFile;
        pFile = NULL;

        try {
            pFile = new MP4File(verbosity);
            pFile->Modify(fileName);
            pFile->Make3GPCompliant(fileName, majorBrand, minorVersion, supportedBrands, supportedBrandsCount, deleteIodsAtom);
            pFile->Close();
            delete pFile;
            return true;
        }
        catch (MP4Error* e) {
            VERBOSE_ERROR(verbosity, e->Print());
            delete e;
        }
        delete pFile;
        return false;
    }

    /* ISMA specific operations */

    bool MP4MakeIsmaCompliant(
        const char* fileName,
        uint32_t verbosity,
        bool addIsmaComplianceSdp)
    {
        MP4File* pFile;
        pFile = NULL;

        try {
            pFile = new MP4File(verbosity);
            pFile->Modify(fileName);
            pFile->MakeIsmaCompliant(addIsmaComplianceSdp);
            pFile->Close();
            delete pFile;
            return true;
        }
        catch (MP4Error* e) {
            VERBOSE_ERROR(verbosity, e->Print());
            delete e;
        }
        delete pFile;
        return false;
    }

    char* MP4MakeIsmaSdpIod(
        uint8_t videoProfile,
        uint32_t videoBitrate,
        uint8_t* videoConfig,
        uint32_t videoConfigLength,
        uint8_t audioProfile,
        uint32_t audioBitrate,
        uint8_t* audioConfig,
        uint32_t audioConfigLength,
        uint32_t verbosity)
    {
        MP4File* pFile = NULL;

        try {
            pFile = new MP4File(verbosity);

            uint8_t* pBytes = NULL;
            uint64_t numBytes = 0;

            pFile->CreateIsmaIodFromParams(
                videoProfile,
                videoBitrate,
                videoConfig,
                videoConfigLength,
                audioProfile,
                audioBitrate,
                audioConfig,
                audioConfigLength,
                &pBytes,
                &numBytes);

            char* iodBase64 =
                MP4ToBase64(pBytes, numBytes);
            MP4Free(pBytes);

            char* sdpIod =
                (char*)MP4Malloc(strlen(iodBase64) + 64);
            snprintf(sdpIod, strlen(iodBase64) + 64,
                     "a=mpeg4-iod: \042data:application/mpeg4-iod;base64,%s\042",
                     iodBase64);
            MP4Free(iodBase64);

            delete pFile;

            return sdpIod;
        }
        catch (MP4Error* e) {
            VERBOSE_ERROR(verbosity, e->Print());
            delete e;
        }
        return NULL;
    }

    /* Edit list */

    MP4EditId MP4AddTrackEdit(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4EditId editId,
        MP4Timestamp startTime,
        MP4Duration duration,
        bool dwell)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                MP4EditId newEditId =
                    ((MP4File*)hFile)->AddTrackEdit(trackId, editId);

                if (newEditId != MP4_INVALID_EDIT_ID) {
                    ((MP4File*)hFile)->SetTrackEditMediaStart(
                        trackId, newEditId, startTime);
                    ((MP4File*)hFile)->SetTrackEditDuration(
                        trackId, newEditId, duration);
                    ((MP4File*)hFile)->SetTrackEditDwell(
                        trackId, newEditId, dwell);
                }

                return newEditId;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_EDIT_ID;
    }

    bool MP4DeleteTrackEdit(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4EditId editId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->DeleteTrackEdit(trackId, editId);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    uint32_t MP4GetTrackNumberOfEdits(
        MP4FileHandle hFile,
        MP4TrackId trackId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackNumberOfEdits(trackId);
            }
            catch (MP4Error* e) {
                //PRINT_ERROR(e);
                delete e;
            }
        }
        return 0;
    }

    MP4Timestamp MP4GetTrackEditMediaStart(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4EditId editId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackEditMediaStart(
                           trackId, editId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_TIMESTAMP;
    }

    MP4Duration MP4GetTrackEditTotalDuration(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4EditId editId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackEditTotalDuration(
                           trackId, editId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_DURATION;
    }

    bool MP4SetTrackEditMediaStart(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4EditId editId,
        MP4Timestamp startTime)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetTrackEditMediaStart(
                    trackId, editId, startTime);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    MP4Duration MP4GetTrackEditDuration(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4EditId editId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackEditDuration(trackId, editId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_DURATION;
    }

    bool MP4SetTrackEditDuration(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4EditId editId,
        MP4Duration duration)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetTrackEditDuration(trackId, editId, duration);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    int8_t MP4GetTrackEditDwell(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4EditId editId)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetTrackEditDwell(trackId, editId);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return -1;
    }

    bool MP4SetTrackEditDwell(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4EditId editId,
        bool dwell)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                ((MP4File*)hFile)->SetTrackEditDwell(trackId, editId, dwell);
                return true;
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return false;
    }

    bool MP4ReadSampleFromEditTime(
        /* input parameters */
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4Timestamp when,
        /* output parameters */
        uint8_t** ppBytes,
        uint32_t* pNumBytes,
        MP4Timestamp* pStartTime,
        MP4Duration* pDuration,
        MP4Duration* pRenderingOffset,
        bool* pIsSyncSample)
    {
        MP4SampleId sampleId =
            MP4GetSampleIdFromEditTime(
                hFile,
                trackId,
                when,
                pStartTime,
                pDuration);

        return MP4ReadSample(
                   hFile,
                   trackId,
                   sampleId,
                   ppBytes,
                   pNumBytes,
                   NULL,
                   NULL,
                   pRenderingOffset,
                   pIsSyncSample);
    }

    MP4SampleId MP4GetSampleIdFromEditTime(
        MP4FileHandle hFile,
        MP4TrackId trackId,
        MP4Timestamp when,
        MP4Timestamp* pStartTime,
        MP4Duration* pDuration)
    {
        if (MP4_IS_VALID_FILE_HANDLE(hFile)) {
            try {
                return ((MP4File*)hFile)->GetSampleIdFromEditTime(
                           trackId, when, pStartTime, pDuration);
            }
            catch (MP4Error* e) {
                PRINT_ERROR(e);
                delete e;
            }
        }
        return MP4_INVALID_SAMPLE_ID;
    }

    /* Utlities */

    char* MP4BinaryToBase16(
        const uint8_t* pData,
        uint32_t dataSize)
    {
        if (pData || dataSize == 0) {
            try {
                return MP4ToBase16(pData, dataSize);
            }
            catch (MP4Error* e) {
                delete e;
            }
        }
        return NULL;
    }

    char* MP4BinaryToBase64(
        const uint8_t* pData,
        uint32_t dataSize)
    {
        if (pData || dataSize == 0) {
            try {
                return MP4ToBase64(pData, dataSize);
            }
            catch (MP4Error* e) {
                delete e;
            }
        }
        return NULL;
    }

    void MP4Free (void *p)
    {
        if (p != NULL)
            free(p);
    }

    void MP4AddIPodUUID (MP4FileHandle hFile, MP4TrackId trackId)
    {
        MP4Track* track = ((MP4File*)hFile)->GetTrack(trackId);
        MP4Atom* avc1 = track->GetTrakAtom()->FindChildAtom("mdia.minf.stbl.stsd.avc1")
                        ;
        avc1->AddChildAtom(new IPodUUIDAtom());
    }

///////////////////////////////////////////////////////////////////////////////

bool MP4GetTrackLanguage(
    MP4FileHandle hFile,
    MP4TrackId    trackId,
    char*         code )
{
    if( !MP4_IS_VALID_FILE_HANDLE( hFile ))
        return false;

    try {
        return ((MP4File*)hFile)->GetTrackLanguage( trackId, code );
    }
    catch( MP4Error* e ) {
        PRINT_ERROR( e );
        delete e;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////

bool MP4SetTrackLanguage(
    MP4FileHandle hFile,
    MP4TrackId    trackId,
    const char*   code )
{
    if( !MP4_IS_VALID_FILE_HANDLE( hFile ))
        return false;

    try {
        return ((MP4File*)hFile)->SetTrackLanguage( trackId, code );
    }   
    catch( MP4Error* e ) {
        PRINT_ERROR( e );
        delete e;
    }   

    return false;
}

///////////////////////////////////////////////////////////////////////////////

bool MP4GetTrackName(
    MP4FileHandle hFile,
    MP4TrackId    trackId,
    char**        name )
{
    if( !MP4_IS_VALID_FILE_HANDLE( hFile ))
        return false;

    try {
        return ((MP4File*)hFile)->GetTrackName( trackId, name );
    }
    catch( MP4Error* e ) {
        PRINT_ERROR( e );
        delete e;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////

bool MP4SetTrackName(
    MP4FileHandle hFile,
    MP4TrackId    trackId,
    const char*   code )
{
    if( !MP4_IS_VALID_FILE_HANDLE( hFile ))
        return false;

    try {
        return ((MP4File*)hFile)->SetTrackName( trackId, code );
    }
    catch( MP4Error* e ) {
        PRINT_ERROR( e );
        delete e;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////

bool MP4GetTrackDurationPerChunk(
    MP4FileHandle hFile,
    MP4TrackId    trackId, 
    MP4Duration*  duration )
{
    if( !MP4_IS_VALID_FILE_HANDLE( hFile ))
        return false;

    try {
        *duration = ((MP4File*)hFile)->GetTrackDurationPerChunk( trackId );
        return true;
    }
    catch( MP4Error* e ) {
        PRINT_ERROR( e );
        delete e;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////

bool MP4SetTrackDurationPerChunk(
    MP4FileHandle hFile,
    MP4TrackId    trackId,
    MP4Duration   duration )
{
    if( !MP4_IS_VALID_FILE_HANDLE( hFile ))
        return false;

    try {
        ((MP4File*)hFile)->SetTrackDurationPerChunk( trackId, duration );
        return true;
    }
    catch( MP4Error* e ) {
        PRINT_ERROR( e );
        delete e;
    }

    return false;
}

///////////////////////////////////////////////////////////////////////////////

} // extern "C"
