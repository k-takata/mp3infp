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
 *		Dave Mackie		dmackie@cisco.com
 */

#include "mp4common.h"

MP4FreeAtom::MP4FreeAtom() 
	: MP4Atom("free") 
{
}

void MP4FreeAtom::Read() 
{
	Skip();
}

void MP4FreeAtom::Write() 
{
	ASSERT(m_pFile);

	bool use64 = (GetSize() > (0xFFFFFFFF - 8)); 
	BeginWrite(use64);
#if 1
	for (uint64_t ix = 0; ix < GetSize(); ix++) {
	  m_pFile->WriteUInt8(0);
	}
#else
	m_pFile->SetPosition(m_pFile->GetPosition() + GetSize());
#endif
	FinishWrite(use64);
}

