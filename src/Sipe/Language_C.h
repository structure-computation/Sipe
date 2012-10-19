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


#ifndef LANGUAGE_C_H
#define LANGUAGE_C_H

#include "Language.h"
#include <map>

/**
*/
class Language_C : public Language {
public:
    Language_C( bool cpp );
    virtual ~Language_C();

    virtual void write( std::ostream &os, const CodeParm &cp, const State *state, bool write_main = false );

    const char *buffer_length;
protected:
    void _write_init_func( std::ostream &os, const char *sp, const char *sn );
    void _write_dest_func( std::ostream &os, const char *sp, const char *sn );
    void _write_parse_file_func( std::ostream &os );
    void _write_preliminaries( std::ostream &os );
    void _write_declarations( std::ostream &os );
    void _write_parse_func( std::ostream &os );
    void _write_main_func( std::ostream &os );
    void _write_code( std::ostream &os, const State *mark );

    struct Cnt {
        Vec<State *> active_marks;
        Block *block;
        int label;
    };

    const CodeParm *cp;
    Vec<Cnt> cnt;
    String f_suf;
    bool cpp;
};

#endif // LANGUAGE_C_H
