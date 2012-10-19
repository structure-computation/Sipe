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


#include "Scp.h"
#include <sstream>

Scp::Scp( State *state, State *dst ) : state( state ), dst( dst ) {
    display_steps = false;
    set_mark = 0;
    use_mark = 0;
}

String Scp::bid() const {
    std::ostringstream os;

    os << state;

    if ( set_mark )
        os << "_M" << set_mark;

    if ( use_mark )
        os << "_U" << use_mark;

    return os.str();
}

