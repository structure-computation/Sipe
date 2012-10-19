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


#ifndef LEXEMMAKER_H
#define LEXEMMAKER_H

#include "ErrorList.h"
#include "Lexem.h"
#include "Vec.h"

/**
*/
class LexemMaker {
public:
    LexemMaker( ErrorList &error_list );
    Lexem *read( Source *source );

    const Lexem *operator()( const char *beg_name, const char *end_name ) const; ///< find the last machine named $name
    const Lexem *operator[]( const char *name ) const; ///< find the last machine named $name
    const Lexem *base() const; ///< base lexem
    const Lexem *root() const; ///< first lexem in the graph (may be 0)
    bool err( const Lexem *l, const char *msg );

protected:
    int read_tok( Source *source, const char *cur ); ///< return increment

    int push_cpp( Source *source, const char *cur ); ///<
    int push_str( Source *source, const char *cur, char lim ); ///< for "..." or '...'
    int push_tok( Source *source, const char *beg, const char *end, Lexem::Type type, int grp = -1, char delim = 0 ); ///< return increment

    bool assemble_rarg( Lexem *o );
    bool assemble_larg( Lexem *o );
    bool assemble_barg( Lexem *o, int need_left = true, int need_right = true );

    enum { max_op_grp = 16 };

    Lexem *sibling[ max_op_grp ];
    Lexem first_item, *last;
    Vec<Lexem *> stack_last;
    ErrorList &error_list;
    int num_grp_bracket;
};

#endif // LEXEMMAKER_H
