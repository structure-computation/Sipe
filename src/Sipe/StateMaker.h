#ifndef STATEMAKER_H
#define STATEMAKER_H

#include "State.h"
#include <map>
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
    Vec<State *> use_mark_stack;
    Vec<State *> &to_del;
    TC created;
};

#endif // STATEMAKER_H
