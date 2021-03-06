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


#include "ErrorList.h"
#include <stdlib.h>

static bool term_supports_color() {
    const char *c_term = getenv("TERM");
    return c_term and std::string( c_term ) != "dumb";
}

static void display_line( std::ostream &out, const char *complete_line, unsigned length_complete_line, int col, bool display_col ) {
    if ( display_col )
        out << "  ";
    if ( length_complete_line < 64 ) {
        out.write(complete_line,length_complete_line);
        if ( display_col ) {
            out << "\n";
            for ( int i = 1;i < 2 + col;++i )
                out << " ";
        }
    } else {
        if ( length_complete_line - col < 64 ) { // only the ending
            out << "...";
            out.write( complete_line + length_complete_line - 64, 64 );
            if ( display_col ) {
                out << "\n";
                for ( unsigned i = 1;i < 2 + 64 + 3 - length_complete_line + col;++i )
                    out << " ";
            }
        } else if ( col < 64 ) { // only the beginning
            out.write( complete_line, 64 );
            out << "...";
            if ( display_col ) {
                out << "\n";
                for ( int i = 1;i < 2 + col;++i )
                    out << " ";
            }
        } else { // middle
            out << "...";
            out.write( complete_line + col - 32, 61 );
            out << "...";
            if ( display_col ) {
                out << "\n";
                for ( int i = 1;i < 2 + 32 + 3;++i )
                    out << " ";
            }
        }
    }
    if ( display_col )
        out << "^";
}

struct Provenance {
    Provenance( const char *beg, const char *pos ) {
        if ( not beg ) {
            col  = 0;
            line = 0;
            return;
        }

        const char *b = pos, *e = pos;
        while ( b >= beg and *b != '\n' and *b != '\r' )
            --b;
        ++b;
        while ( *e and *e != '\n' and *e != '\r' )
            ++e;
        if ( b > e )
            b = e;

        complete_line = std::string( b, e );

        col = pos - b + 1;
        line = 1;
        for ( b = pos; b >= beg; --b )
            line += ( *b == '\n' );
    }
    std::string complete_line;
    int col, line;
};


ErrorList::ErrorList() : out( std::cerr ) {
    display_escape_sequences = term_supports_color();
    display_col = true;
    clear();
}

void ErrorList::clear() {
    ok = true;
}

ErrorList::operator bool() const {
    return ok;
}

void ErrorList::add( Source *source, const char *str, const char *msg ) {
    ok = false;
    if ( source ) {
        if ( str ) {
            Provenance po( source->data, str );

            if ( display_escape_sequences )
                *out.stream << "\033[1m";
            if ( source->provenance )
                *out.stream << source->provenance << ":";
            if ( po.line )
                *out.stream << po.line << ":" << po.col << ": ";
            if ( msg )
                *out.stream << "error: " << msg << ( display_col ? "\n" : " in '" );
            if ( po.complete_line.size() )
                display_line( *out.stream, po.complete_line.c_str(), po.complete_line.size(), po.col, display_col );
            *out.stream << ( display_col ? "\n" : "'\n" );
            if ( display_escape_sequences )
                *out.stream << "\033[0m";
        } else
            out << source->provenance << ": " << msg;
    } else
        out << msg;
}

