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

namespace mp4v2 { namespace impl {

///////////////////////////////////////////////////////////////////////////////

MP4StblAtom::MP4StblAtom(MP4File &file)
        : MP4Atom(file, "stbl")
{
    ExpectChildAtom("stsd", Required, OnlyOne);
    ExpectChildAtom("stts", Required, OnlyOne);
    ExpectChildAtom("ctts", Optional, OnlyOne);
    ExpectChildAtom("stsz", Required, OnlyOne);
    ExpectChildAtom("stz2", Optional, OnlyOne);
    ExpectChildAtom("stsc", Required, OnlyOne);
    ExpectChildAtom("stco", Optional, OnlyOne);
    ExpectChildAtom("co64", Optional, OnlyOne);
    ExpectChildAtom("stss", Optional, OnlyOne);
    ExpectChildAtom("stsh", Optional, OnlyOne);
    ExpectChildAtom("stdp", Optional, OnlyOne);
    ExpectChildAtom("sdtp", Optional, OnlyOne);
}

void MP4StblAtom::Generate()
{
    // as usual
    MP4Atom::Generate();

    // but we also need one of the chunk offset atoms
    MP4Atom* pChunkOffsetAtom;
    if (m_File.Use64Bits(GetType())) {
        pChunkOffsetAtom = CreateAtom(m_File, this, "co64");
    } else {
        pChunkOffsetAtom = CreateAtom(m_File, this, "stco");
    }

    AddChildAtom(pChunkOffsetAtom);

    // and ask it to self generate
    pChunkOffsetAtom->Generate();
}

///////////////////////////////////////////////////////////////////////////////

}} // namespace mp4v2::impl
