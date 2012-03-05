#ifndef SMP_H
#define SMP_H

#include "Instruction.h"
#include "State.h"
#include "Paths.h"

/**
*/
class Smp {
public:
    Smp();

    String bid() const;
    int  next( int index );
    void remove_branch( int index );
    void join_branches( int j, int i );
    bool surely_leads_to_the_end( int index );

    Paths<const Instruction *> paths;
    String display_prefix;
    bool display_steps;
    bool allow_incc;
    State *pending;
    Ctr<Cond> cond;
};

std::ostream &operator<<( std::ostream &os, const Smp &p );

#endif // SMP_H
