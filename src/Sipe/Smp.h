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


#ifndef SMP_H
#define SMP_H

#include "Instruction.h"
#include "State.h"

/**
*/
class Smp {
public:
    Smp();

    String bid() const;
    int  next( int index );
    void remove_branch( int index );
    void join_branches( int j, int i );
    void init( const Instruction *inst );
    bool surely_leads_to_the_end( int index );
    void rm_double_pending();

    const Instruction *action_next_round;
    Vec<const Instruction *> ok;
    bool use_mark_from_pending;
    String display_prefix;
    Vec<int> nok_to_pok;
    State *mark;

    bool display_steps;
    Vec<Smp> pending;
    bool allow_incc;
    Ctr<Cond> cond;
};

std::ostream &operator<<( std::ostream &os, const Smp &p );

#endif // SMP_H
