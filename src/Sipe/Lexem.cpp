#include "DotOut.h"
#include "Lexem.h"

#include <string.h>
#include <fstream>

using namespace std;

Lexem::Lexem( Type type, Source *source, const char *beg, const char *end ) : type( type ), source( source ), beg( beg ), end( end ) {
    children[ 0 ] = 0;
    children[ 1 ] = 0;
    parent = 0;
    next = 0;
    prev = 0;
    sibling = 0;
}

Lexem::~Lexem() {
    delete children[ 0 ];
    delete children[ 1 ];
    delete next;
}

int Lexem::display_dot( const char *f, const char *prg ) const {
    std::ofstream of( f );
    StreamSepMaker<std::ostream> os( of );
    os << "digraph LexemMaker {";

    int m = write_dot( os, 0 );

    for( int i = 0; i <= m; ++i ) {
        os << "  " << i << " [ shape=plaintext ];";
        if ( i )
            os << "  " << i - 1 << " -> " << i << ";";
    }

    os << "}";
    of.close();

    return exec_dot( f, prg );
}

int Lexem::write_dot( StreamSepMaker<std::ostream> &os, int p ) const {
    int res = p;

    *os.stream << "{ rank=same; " << p << " node_" << this << " [label=\"";
    dot_out( *os.stream, beg, end );
    os << "\"] }";
    if ( next ) {
        os << "  node_" << this << " -> node_" << next << ";";
        res = max( res, next->write_dot( os, p ) );
    }
    for( int i = 0; i < 2; ++i ) {
        if ( const Lexem *c = children[ i ] ) {
            os << "  node_" << this << " -> node_" << c << " [color=\"green\"];";
            res = max( res, c->write_dot( os, p + 1 ) );
        }
    }

    if ( prev )
        os << "  node_" << prev << " -> node_" << this << " [color=\"yellow\"];";
    if ( parent )
        os << "  node_" << parent << " -> node_" << this << " [color=\"red\"];";
    return res;
}

bool Lexem::eq( const char *_beg, const char *_end ) const {
    return end - beg == _end - _beg and strncmp( beg, _beg, end - beg ) == 0;
}

bool Lexem::eq( Type _type, const char *str ) const {
    return type == _type and eq( str );
}

bool Lexem::eq( const char *str ) const {
    return strlen( str ) == size_t( end - beg ) and strncmp( beg, str, end - beg ) == 0;
}

int Lexem::to_int() const {
    int res = 0;
    for( const char *s = beg; s != end; ++s )
        res = 10 * res + ( *s - '0' );
    return res;
}

std::ostream &operator<<( std::ostream &os, const Lexem &l ) {
    os.write( l.beg, l.end - l.beg );
    return os;
}

