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
