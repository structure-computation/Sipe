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
