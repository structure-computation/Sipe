#ifndef FUNC_H
#define FUNC_H

#include "FuncParm.h"

/**
*/
class Func {
public:
    Func( const char *beg, const char *end, const FuncParm &parm );

    String   name;
    FuncParm parm;
};

std::ostream &operator<<( std::ostream &os, const Func &func );

#endif // FUNC_H
