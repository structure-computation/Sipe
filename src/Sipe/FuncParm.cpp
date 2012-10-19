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


#include "FuncParm.h"

bool FuncParm::operator==( const FuncParm &fp ) const {
    return u_params == fp.u_params and n_params == fp.n_params;
}

void FuncParm::set( const String &key, const String &val ) {
    for( int i = 0; i < n_params.size(); ++i ) {
        if ( n_params[ i ].first == key ) {
            n_params[ i ].second = val;
        }
    }
    n_params << NamedP( key, val );
}

String FuncParm::operator[]( String key ) const {
    for( int i = 0; i < n_params.size(); ++i )
        if ( n_params[ i ].first == key )
            return n_params[ i ].second;
    return String();
}

bool FuncParm::has( String key ) const {
    for( int i = 0; i < n_params.size(); ++i )
        if ( n_params[ i ].first == key )
            return true;
    return false;
}

void FuncParm::remove( String key ) {
    for( int i = 0; i < n_params.size(); ++i )
        if ( n_params[ i ].first == key )
            n_params.remove( i-- );
}

void FuncParm::clear() {
    u_params.resize( 0 );
    n_params.resize( 0 );
}

std::ostream &operator<<( std::ostream &os, const FuncParm &func_parm ) {
    for( int i = 0; i < func_parm.u_params.size(); ++i )
        os << ( i ? "," : "" ) << func_parm.u_params[ i ];
    for( int i = 0; i < func_parm.n_params.size(); ++i )
        os << ( i + func_parm.u_params.size() ? "," : "" ) << func_parm.n_params[ i ].first << "=" << func_parm.n_params[ i ].second;
    return os;
}
