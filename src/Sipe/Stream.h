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


#ifndef STREAM_H
#define STREAM_H

#include "StreamSep.h"
#include <iostream>
#include <string>

#ifdef METIL_COMP_DIRECTIVE
#pragma cpp_flag -Wreorder
#endif // METIL_COMP_DIRECTIVE

extern StreamSepMaker<std::ostream> coutn;
extern StreamSepMaker<std::ostream> cerrn;

#define P( A ) coutn << #A << " -> " << ( A )

typedef std::string String;

#endif // STREAM_H
