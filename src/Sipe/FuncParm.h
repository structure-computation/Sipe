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


#ifndef FUNCPARM_H
#define FUNCPARM_H

#include "Stream.h"
#include "Vec.h"

/**
*/
class FuncParm {
public:
    typedef std::pair<String,String> NamedP;

    void set( const String &key, const String &val );
    bool operator==( const FuncParm &fp ) const;
    String operator[]( String key ) const;
    bool has( String key ) const;
    void remove( String key );
    void clear();

    Vec<String> u_params;
    Vec<NamedP> n_params;
};

std::ostream &operator<<( std::ostream &os, const FuncParm &func_parm );

#endif // FUNCPARM_H
