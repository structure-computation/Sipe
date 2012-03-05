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
