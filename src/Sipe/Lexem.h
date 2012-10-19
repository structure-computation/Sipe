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


#ifndef LEXEM_H
#define LEXEM_H

#include "Stream.h"
#include "Source.h"

/**
*/
class Lexem {
public:
    typedef enum {
        OPERATOR,
        VARIABLE,
        STRING,
        NUMBER,
        CODE,
        NONE
    } Type;

    Lexem( Type type, Source *source, const char *beg, const char *end );
    ~Lexem();

    int display_dot( const char *f = ".res.dot", const char *prg = 0 ) const;
    int write_dot( StreamSepMaker<std::ostream> &os, int p ) const;

    bool eq( const char *beg, const char *end ) const;
    bool eq( Type type, const char *str ) const;
    bool eq( const char *str ) const;
    int to_int() const;

    Type type;
    Source *source;
    const char *beg, *end;
    Lexem *children[ 2 ], *parent, *next, *prev, *sibling;
};

std::ostream &operator<<( std::ostream &os, const Lexem &l );


#endif // LEXEM_H
