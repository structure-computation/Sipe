#include "Func.h"

Func::Func( const char *beg, const char *end, const FuncParm &parm ) : name( beg, end ), parm( parm ) {
}

std::ostream &operator<<( std::ostream &os, const Func &func ) {
    return os << func.name << "[" << func.parm << "]";
}
