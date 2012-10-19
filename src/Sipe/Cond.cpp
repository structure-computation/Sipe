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


#include <sstream>
#include "Cond.h"
typedef unsigned char UC;

Cond Cond::full( 0, Cond::p_size );

// display of a char
static std::ostream &cc( std::ostream &os, int r ) {
    if ( r == '\\' )
        return os << "'\\\\'";
    if ( r >= 0x20 and r <= 0x7e and r != 39 )
        return os << "'" << char( r ) << "'";
    return os << r;
}

// display of a char
//static std::ostream &cc( std::ostream &os, char r ) {
//    return cc( os, UC( r ) );
//}

Cond::Cond() {
    for( int i = 0; i < p_size; ++i )
        p[ i ] = 0;
}

Cond::Cond( char c ) {
    for( int i = 0; i < p_size; ++i )
        p[ i ] = i == UC( c );
}

Cond::Cond( char b, char e ) {
    for( int i = 0; i < p_size; ++i )
        p[ i ] = i >= UC( b ) and i <= UC( e );
}

Cond::Cond( int c ) {
    for( int i = 0; i < p_size; ++i )
        p[ i ] = i == c;
}

Cond::Cond( int b, int e ) {
    for( int i = 0; i < p_size; ++i )
        p[ i ] = i >= b and i <= e;
}


Cond::operator bool() const {
    for( int i = 0; i < p_size; ++i )
        if ( p[ i ] )
            return true;
    return false;
}

bool Cond::operator==( const Cond &c ) const {
    for( int i = 0; i < p_size; ++i )
        if ( p[ i ] != c.p[ i ] )
            return false;
    return true;
}

bool Cond::operator!=( const Cond &c ) const {
    for( int i = 0; i < p_size; ++i )
        if ( p[ i ] != c.p[ i ] )
            return true;
    return false;
}

Cond &Cond::operator|=( const Cond &c ) {
    for( int i = 0; i < p_size; ++i )
        p[ i ] = p[ i ] or c.p[ i ];
    return *this;
}

Cond &Cond::operator&=( const Cond &c ) {
    for( int i = 0; i < p_size; ++i )
        p[ i ] = p[ i ] and c.p[ i ];
    return *this;
}

Cond Cond::operator|( const Cond &c ) const {
    Cond res = *this;
    res |= c;
    return res;
}

Cond Cond::operator&( const Cond &c ) const {
    Cond res = *this;
    res &= c;
    return res;
}

Cond Cond::operator~() const {
    Cond res;
    for( int i = 0; i < p_size; ++i )
        res.p[ i ] = not p[ i ];
    return res;
}

Vec<int> Cond::ok_ranges() const {
    return _ranges( true );
}

Vec<int> Cond::ko_ranges() const {
    return _ranges( false );
}

String Cond::ok_cpp( String var, const Cond *not_in ) const {
    return _cpp( var, true, not_in );
}

String Cond::ko_cpp( String var, const Cond *not_in ) const {
    return _cpp( var, false, not_in );
}

int Cond::nz() const {
    int res = 0;
    for( int i = 0; i < p_size; ++i )
        res += bool( p[ i ] );
    return res;
}

int Cond::first_nz() const {
    for( int i = 0; i < p_size; ++i )
        if ( p[ i ] )
            return i;
    return -1;
}

bool Cond::always_checked( bool assume_not_eof ) const {
    for( int i = 0; i < p_size - assume_not_eof; ++i )
        if ( not p[ i ] )
            return false;
    return true;
}

bool Cond::always_checked( const Cond &not_in ) const {
    for( int i = 0; i < p_size; ++i )
        if ( not not_in[ i ] and not p[ i ] )
            return false;
    return true;
}

bool Cond::included_in( const Cond &cond ) const {
    for( int i = 0; i < p_size; ++i )
        if ( p[ i ] and not cond.p[ i ] )
            return false;
    return true;
}

Vec<int> Cond::_ranges( bool w ) const {
    Vec<int> res;
    bool old = not w;
    for( int i = 0; i < p_size; ++i ) {
        if ( old != p[ i ] ) {
            old = p[ i ];
            res << i;
        }
    }
    if ( old == w )
        res << p_size;

    // inclusion of ends
    for( int i = 1; i < res.size(); i += 2 )
        --res[ i ];

    return res;
}

String Cond::_cpp( String var, bool w, const Cond *not_in = 0 ) const {
    Vec<int> ra = _ranges( w );

    // [ ... x - 1 ] | [ x + 1 ... ]
    Vec<int> ne;
    for( int i = 0; i + 2 < ra.size(); i += 2 ) {
        if ( ra[ i + 1 ] + 2 == ra[ i + 2 ] ) {
            ne << ra[ i + 1 ] + 1;
            ra[ i + 1 ] = ra[ i + 3 ];
            ra.remove( i + 2 );
            ra.remove( i + 2 );
            i -= 2;
        }
    }

    // [ 0 .. max ]
    if ( ra.size() == 2 and ra[ 0 ] == 0 and ra[ 1 ] == p_size - 1 )
        ra.resize( 0 );

    // var >= ... and var <= ...
    std::ostringstream res;
    if ( ra.size() > 2 and ne.size() )
        res << "( ";
    for( int i = 0; i < ra.size(); i += 2 ) {
        if ( i )
            res << " or ";

        if ( ra[ i + 0 ] == ra[ i + 1 ] ) {
            cc( res << var << " == ", ra[ i + 0 ] );
        } else if ( ra[ i + 0 ] == 0 ) {
            if ( ra[ i + 1 ] != p_size - 1 )
                cc( res << var << " <= ", ra[ i + 1 ] );
        } else {
            if ( ra[ i + 1 ] != p_size - 1 ) {
                const char *pb = ra.size() + ne.size() > 2 ? "( " : "";
                const char *pe = ra.size() + ne.size() > 2 ? " )" : "";
                cc( cc( res << pb << var << " >= ", ra[ i + 0 ] ) << " and " << var << " <= ", ra[ i + 1 ] ) << pe;
            } else
                cc( res << var << " >= ", ra[ i + 0 ] );
        }
    }
    if ( ra.size() > 2 and ne.size() )
        res << " )";

    // var != ...
    for( int i = 0; i < ne.size(); ++i ) {
        if ( i or ra.size() )
            res << " and ";
        cc( res << var << " != ", ne[ i ] );
    }

    return res.str();
}

static int cost_disp_range( const Vec<int> &r ) {
    int res = 0;
    for( int i = 0; i < r.size(); i += 2 )
        res += 1 + ( r[ i + 0 ] != r[ i + 1 ] ) * 3;
    return res;
}

std::ostream &operator<<( std::ostream &os, const Cond &cond ) {
    Vec<int> r = cond.ok_ranges();

    bool inv = false;
    if ( r.size() > 2 ) {
        Vec<int> a = cond.ko_ranges();
        if ( cost_disp_range( a ) + 3 < cost_disp_range( r ) ) {
            inv = a.size() == 2 and a[ 0 ] == a[ 1 ] ? false : true;
            os << '^';
            if ( inv )
                os << '(';
            r = a;
        }
    }

    if ( r.size() == 2 and r[ 0 ] == r[ 1 ] )
        return cc( os, r[ 0 ] ) << ( inv ? ")" : "" );
    for( int i = 0; i < r.size(); i += 2 ) {
        if ( r[ i + 0 ] == r[ i + 1 ] )
            cc( os << ( i ? "|" : "" ), r[ i + 0 ] );
        else if ( r[ i + 0 ] + 1 == r[ i + 1 ] )
            cc( cc( os << ( i ? "|" : "" ), r[ i + 0 ] ) << "|",  r[ i + 1 ] );
        else
            cc( cc( os << ( i ? "|" : "" ), r[ i + 0 ] ) << "..",  r[ i + 1 ] );
    }
    if ( inv )
        os << ')';
    return os;
}

