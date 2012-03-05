#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "State.h"

/**
*/
class Language {
public:
    Language();
    virtual ~Language();

    virtual void write( std::ostream &os, const State *state ) = 0;
};

#endif // LANGUAGE_H
