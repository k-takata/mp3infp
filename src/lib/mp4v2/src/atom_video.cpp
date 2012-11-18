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
 * Copyright (C) Cisco Systems Inc. 2004.  All Rights Reserved.
 *
 * Contributor(s):
 *      Bill May  wmay@cisco.com
 */

#include "src/impl.h"

namespace mp4v2 {
namespace impl {

///////////////////////////////////////////////////////////////////////////////

MP4VideoAtom::MP4VideoAtom (MP4File &file, const char *type)
        : MP4Atom(file, type)
{
    AddReserved(*this, "reserved1", 6); /* 0 */

    AddProperty( /* 1 */
        new MP4Integer16Property(*this, "dataReferenceIndex"));

    AddReserved(*this, "reserved2", 16); /* 2 */

    AddProperty( /* 3 */
        new MP4Integer16Property(*this, "width"));
    AddProperty( /* 4 */
        new MP4Integer16Property(*this, "height"));

    AddReserved(*this, "reserved3", 14); /* 5 */

    MP4StringProperty* pProp =
        new MP4StringProperty(*this, "compressorName");
    pProp->SetFixedLength(32);
    pProp->SetCountedFormat(true);
    pProp->SetValue("");
    AddProperty(pProp); /* 6 */

    AddProperty(/* 7 */
        new MP4Integer16Property(*this, "depth"));
    AddProperty(/* 8 */
        new MP4Integer16Property(*this, "colorTableId"));
    ExpectChildAtom("smi ", Optional, OnlyOne);
}

void MP4VideoAtom::Generate()
{
    MP4Atom::Generate();

    ((MP4Integer16Property*)m_pProperties[1])->SetValue(1);

    // property reserved3 has non-zero fixed values
    static uint8_t reserved3[14] = {
        0x00, 0x48, 0x00, 0x00,
        0x00, 0x48, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x01,
    };
    m_pProperties[5]->SetReadOnly(false);
    ((MP4BytesProperty*)m_pProperties[5])->
    SetValue(reserved3, sizeof(reserved3));
    m_pProperties[5]->SetReadOnly(true);

    // depth and color table id values - should be set later
    // as far as depth - color table is most likely 0xff
    ((MP4IntegerProperty *)m_pProperties[7])->SetValue(0x18);
    ((MP4IntegerProperty *)m_pProperties[8])->SetValue(0xffff);

}

///////////////////////////////////////////////////////////////////////////////

}
} // namespace mp4v2::impl
