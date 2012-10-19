/*
 Copyright 2012 Structure Computation  www.structure-computation.com
 Copyright 2012 Hugo Leclerc

 This file is part of Sipe.

 Sipe is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Sipe is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with Sipe. If not, see <http://www.gnu.org/licenses/>.
*/


#include "Source.h"
#include <stdlib.h>
#include <string.h>
#include <fstream>
using namespace std;

Source::Source( const char *file, Source *prev ) : prev( prev ) {
    ssize_t size_prov = strlen( file ) + 1;

    std::ifstream f( file );
    if ( f ) {
        f.seekg( 0, ios_base::end );
        ssize_t size_data = f.tellg() + ssize_t( 1 );
        f.seekg( 0, ios_base::beg );

        rese = (char *)malloc( size_prov + size_data );

        f.read( rese + size_prov, size_data - 1 );
        rese[ size_prov + size_data - 1 ] = 0;
        data = rese + size_prov;
    } else {
        rese = (char *)malloc( size_prov );
        data = 0;
    }

    // copy file -> provenance
    memcpy( rese, file, size_prov );
    provenance = rese;
}

Source::Source( const char *file, const char *text, Source *prev, bool need_cp ) : prev( prev ) {
    if ( need_cp ) {
        ssize_t size_prov = strlen( file ) + 1;
        ssize_t size_data = strlen( text ) + 1;
        rese = (char *)malloc( size_prov + size_data );

        memcpy( rese, file, size_prov );
        provenance = rese;

        memcpy( rese + size_prov, text, size_data );
        data = rese + size_prov;
    } else {
        provenance = file;
        data = text;
        rese = 0;
    }
}

Source::~Source() {
    if ( rese )
        free( rese );
    delete prev;
}
