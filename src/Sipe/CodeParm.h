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


#ifndef CODEPARM_H
#define CODEPARM_H

#include "Stream.h"
#include "Vec.h"

/**
  Parameters for code creation
*/
class CodeParm {
public:
    CodeParm();

    struct Attr {
        String decl;
        String init;
        String dest;
    };

    Vec<String> preliminaries;
    Vec<Attr> attributes;
    String struct_name;
};

#endif // CODEPARM_H
