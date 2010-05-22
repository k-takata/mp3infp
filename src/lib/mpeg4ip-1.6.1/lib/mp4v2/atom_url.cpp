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

MP4UrlAtom::MP4UrlAtom(const char *type) 
	: MP4Atom(type)
{
	AddVersionAndFlags();
	AddProperty(new MP4StringProperty("location"));
}

void MP4UrlAtom::Read() 
{
	// read the version and flags properties
	ReadProperties(0, 2);

	// check if self-contained flag is set
	if (!(GetFlags() & 1)) {
		// if not then read url location
		ReadProperties(2);
	}

	Skip();	// to end of atom
}

void MP4UrlAtom::Write() 
{
	MP4StringProperty* pLocationProp =
		(MP4StringProperty*)m_pProperties[2];

	// if no url location has been set
	// then set self-contained flag
	// and don't attempt to write anything
	if (pLocationProp->GetValue() == NULL) {
		SetFlags(GetFlags() | 1);
		pLocationProp->SetImplicit(true);
	} else {
		SetFlags(GetFlags() & 0xFFFFFE);
		pLocationProp->SetImplicit(false);
	}

	// write atom as usual
	MP4Atom::Write();
}
