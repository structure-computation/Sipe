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


#ifndef COND_H
#define COND_H

#include "Stream.h"
#include "Vec.h"

/**
*/
class Cond {
public:
    enum {
        eof = 256,
        p_size = eof + 1
    };

    Cond();

    Cond( char c );
    Cond( char b, char e ); ///< range [ b, e ]. e is included

    Cond( int c );
    Cond( int b, int e ); ///< range [ b, e ]. e is included

    operator bool() const;

    bool operator==( const Cond &c ) const;
    bool operator!=( const Cond &c ) const;

    Cond &operator|=( const Cond &c );
    Cond &operator&=( const Cond &c );

    Cond operator|( const Cond &c ) const;
    Cond operator&( const Cond &c ) const;

    Cond operator~() const; ///< negation

    Cond &operator<<( int v ) { p[ v ] = 1; return *this; }
    bool operator[]( int i ) const { return p[ i ]; }

    Vec<int> ok_ranges() const; ///< as ranges like [ b, e ]
    Vec<int> ko_ranges() const; ///< as ranges like [ b, e ]

    String ok_cpp( String var, const Cond *not_in = 0 ) const; ///< ok condition for cpp
    String ko_cpp( String var, const Cond *not_in = 0 ) const; ///< ko condition for cpp

    int  nz() const;
    int  first_nz() const;
    bool always_checked( bool assume_not_eof = false ) const; ///< true if n_ch == 0 or any works
    bool always_checked( const Cond &not_in ) const; ///< true if n_ch == 0 or any works
    bool included_in( const Cond &cond ) const;

    static Cond full;
protected:
    String _cpp( String var, bool w, const Cond *not_in ) const; ///<
    Vec<int> _ranges( bool w ) const;

    bool p[ p_size ];

    friend std::ostream &operator<<( std::ostream &os, const Cond &cond );
};

#endif // COND_H
