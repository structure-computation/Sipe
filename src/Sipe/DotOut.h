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


int exec_dot( const char *filename, const char *viewer = 0, bool launch_viewer = true, bool par = true );

#endif // DOTOUT_H
