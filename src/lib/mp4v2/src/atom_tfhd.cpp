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
 * Copyright (C) Cisco Systems Inc. 2001.  All Rights Reserved.
 *
 * Contributor(s):
 *      Dave Mackie     dmackie@cisco.com
 */

#include "src/impl.h"

namespace mp4v2 {
namespace impl {

///////////////////////////////////////////////////////////////////////////////

MP4TfhdAtom::MP4TfhdAtom(MP4File &file)
        : MP4Atom(file, "tfhd")
{
    AddVersionAndFlags();   /* 0, 1 */
    AddProperty( /* 2 */
        new MP4Integer32Property(*this, "trackId"));
}

void MP4TfhdAtom::AddProperties(uint32_t flags)
{
    if (flags & 0x01) {
        // note this property is signed 64!
        AddProperty(
            new MP4Integer64Property(*this, "baseDataOffset"));
    }
    if (flags & 0x02) {
        AddProperty(
            new MP4Integer32Property(*this, "sampleDescriptionIndex"));
    }
    if (flags & 0x08) {
        AddProperty(
            new MP4Integer32Property(*this, "defaultSampleDuration"));
    }
    if (flags & 0x10) {
        AddProperty(
            new MP4Integer32Property(*this, "defaultSampleSize"));
    }
    if (flags & 0x20) {
        AddProperty(
            new MP4Integer32Property(*this, "defaultSampleFlags"));
    }
}

void MP4TfhdAtom::Read()
{
    /* read atom version, flags, and trackId */
    ReadProperties(0, 3);

    /* need to create the properties based on the atom flags */
    AddProperties(GetFlags());

    /* now we can read the remaining properties */
    ReadProperties(3);

    Skip(); // to end of atom
}

///////////////////////////////////////////////////////////////////////////////

}
} // namespace mp4v2::impl
