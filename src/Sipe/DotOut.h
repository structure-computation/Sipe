#ifndef DOTOUT_H
#define DOTOUT_H

#include "Stream.h"
#include <sstream>
#include <fstream>

std::ostream &dot_out( std::ostream &os, const char *beg, int lim = -1 );
std::ostream &dot_out( std::ostream &os, const char *beg, const char *end, int lim = -1 );

template<class T>
std::ostream &dot_out( std::ostream &os, const T &val, int lim = -1 ) {
    std::ostringstream ss;
    ss << val;
    return dot_out( os, ss.str().c_str(), lim );
}


int exec_dot( const char *filename, const char *viewer = 0, bool launch_viewer = true );

#endif // DOTOUT_H
