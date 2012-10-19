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


#include "DotOut.h"
#include <string.h>
#include <stdlib.h>
#include <sstream>

std::ostream &dot_out( std::ostream &os, const char *beg, int lim ) {
    return dot_out( os, beg, beg + strlen( beg ), lim );
}

std::ostream &dot_out( std::ostream &os, const char *beg, const char *end, int lim ) {
    for( ; beg < end; ++beg, --lim ) {
        if ( lim == 3 and end - beg > 3 ) {
            os << "...";
            break;
        }

        if ( *beg == '"' )
            os << '\\' << '"';
        else if ( *beg == '\n' )
            os << '\\' << 'n';
        else
            os << *beg;
    }

    return os;
}

int exec_dot( const char *f, const char *viewer, bool launch_viewer, bool par ) {
    // call graphviz
    std::ostringstream ss;
    ss << "dot -Tpdf " << f << " > " << f << ".pdf";
    if ( int res = system( ss.str().c_str() ) )
        return res;

    // call viewer
    if ( launch_viewer ) {
        const char *t[] = {
            viewer,
            "okular",
            "xpdf",
            "acroread",
            "evince",
            0,
        };
        for( int i = not viewer; t[ i ]; ++i ) {
            std::ostringstream ss;
            ss << t[ i ] << " " << f << ".pdf";
            if ( par )
                ss << " &";
            if ( system( ss.str().c_str() ) == 0 )
                return 0;
        }
    }

    return 0;
}




