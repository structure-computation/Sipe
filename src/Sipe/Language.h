#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "State.h"

/**
*/
class Language {
public:
    Language();
    virtual ~Language();

    virtual void write( std::ostream &os, const State *state, bool write_main = false ) = 0;

protected:
    struct Attr {
        String decl;
        String init;
        String dest;
    };

    void read_state( const State *state );

    Vec<String> preliminaries;
    Vec<Attr> attributes;
};

#endif // LANGUAGE_H
