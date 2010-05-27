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
 *      M. Bakker     mbakker at nero.com
 *
 * Apple iTunes META data
 */

#include "src/impl.h"

namespace mp4v2 { namespace impl {

///////////////////////////////////////////////////////////////////////////////

MP4DataAtom::MP4DataAtom()
    : MP4Atom ( "data" )
    , typeReserved      ( *new MP4Integer16Property( "typeReserved" ))
    , typeSetIdentifier ( *new MP4Integer8Property( "typeSetIdentifier" ))
    , typeCode          ( *new MP4BasicTypeProperty( "typeCode" ))
    , locale            ( *new MP4Integer32Property( "locale" ))
    , metadata          ( *new MP4BytesProperty( "metadata" ))
{
    AddProperty( &typeReserved );
    AddProperty( &typeSetIdentifier );
    AddProperty( &typeCode );
    AddProperty( &locale );
    AddProperty( &metadata );
}

void
MP4DataAtom::Read()
{
    // calculate size of the metadata from the atom size
    metadata.SetValueSize( m_size - 8 );
    MP4Atom::Read();
}

///////////////////////////////////////////////////////////////////////////////

MP4FullAtom::MP4FullAtom( const char* type )
    : MP4Atom ( type )
    , version ( *new MP4Integer8Property( "version" ))
    , flags   ( *new MP4Integer24Property( "flags" ))
{
    AddProperty( &version );
    AddProperty( &flags );
}

///////////////////////////////////////////////////////////////////////////////

MP4ItemAtom::MP4ItemAtom( const char* type )
    : MP4Atom( type )
{
    ExpectChildAtom( "mean", Optional, OnlyOne );
    ExpectChildAtom( "name", Optional, OnlyOne );
    ExpectChildAtom( "data", Required, Many );
}

///////////////////////////////////////////////////////////////////////////////

MP4MeanAtom::MP4MeanAtom()
    : MP4FullAtom ( "mean" )
    , value       ( *new MP4StringProperty( "value" ))
{
    AddProperty( &value );
}

void
MP4MeanAtom::Read()
{
    // calculate size of the metadata from the atom size
    value.SetFixedLength( m_size - 4 );
    MP4Atom::Read();
}

///////////////////////////////////////////////////////////////////////////////

MP4NameAtom::MP4NameAtom()
    : MP4FullAtom( "name" )
    , value       ( *new MP4StringProperty( "value" ))
{
    AddProperty( &value );
}

void
MP4NameAtom::Read()
{
    // calculate size of the metadata from the atom size
    value.SetFixedLength( m_size - 4 );
    MP4FullAtom::Read();
}

///////////////////////////////////////////////////////////////////////////////

MP4UdtaElementAtom::MP4UdtaElementAtom( const char* type )
    : MP4Atom ( type )
    , value   ( *new MP4BytesProperty( "value" ))
{
    AddProperty( &value );
}

void
MP4UdtaElementAtom::Read()
{
    // calculate size of the metadata from the atom size
    value.SetValueSize( m_size );
    MP4Atom::Read();
}

///////////////////////////////////////////////////////////////////////////////

}} // namespace mp4v2::impl
