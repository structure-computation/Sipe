#ifndef ERRORLIST_H
#define ERRORLIST_H

#include "Stream.h"
#include "Source.h"

/**
*/
class ErrorList {
public:
    ErrorList();
    void clear();
    operator bool() const;
    void add( Source *source, const char *beg, const char *msg );

    StreamSepMaker<std::ostream> out;
    bool display_escape_sequences;
    bool display_col;
    bool ok;
};

#endif // ERRORLIST_H
