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


#ifndef STATECLONER_H
#define STATECLONER_H

#include "Scp.h"
#include <map>

/**
*/
class StateCloner {
public:
    StateCloner( Vec<State *> &to_del, Vec<State *> &use_mark_stack );

    State *make( State *smk, State *dst );

protected:
    State *_new_State( Scp &p );
    State *_make_rec( Scp &p, const char *msg );

    typedef std::map<String,State *> TC;
    Vec<State *> &use_mark_stack;
    Vec<State *> &to_del;
    TC created;
};

#endif // STATECLONER_H
