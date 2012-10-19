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


#ifndef STATEMAKER_H
#define STATEMAKER_H

#include "State.h"
#include <map>
#include <set>
struct Smp;

/**
*/
class StateMaker {
public:
    StateMaker( Vec<State *> &to_del );
    State *make( const Instruction *inst, bool ws );

protected:
    State *_make_rec( Smp &state, const char *step );
    State *_new_State( const Smp &p );

    State *_same_bid( Smp &p );
    State *_action_r( Smp &p );
    State *_u_mark_p( Smp &p );
    State *_rm_twice( Smp &p );
    State *_use_pend( Smp &p );
    State *_only_end( Smp &p );
    State *_prior_br( Smp &p );
    State *_has_cond( Smp &p );
    State *_rm_bcond( Smp &p );
    State *_jmp_cond( Smp &p );
    State *_jmp_code( Smp &p );
    State *_jmp_incc( Smp &p );
    State *_use_next( Smp &p );

    typedef std::map<String,State *> TC;
    Vec<State *> &to_del;
    TC created;
};

#endif // STATEMAKER_H
