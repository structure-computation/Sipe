#include "StateCloner.h"

StateCloner::StateCloner( Vec<State *> &to_del, Vec<State *> &use_mark_stack ) : use_mark_stack( use_mark_stack ), to_del( to_del ) {
}

State *StateCloner::make( State *smk, State *dst ) {
    dst->prev_mark( smk );

    Scp p( smk->next[ 0 ].s, dst );
    return _make_rec( p, "orig" );
}

State *StateCloner::_new_State( Scp &p ) {
    State *res = new State( *p.state );
    res->prev.resize( 0 );
    res->next.resize( 0 );
    to_del << res;

    created[ p.bid() ] = res;
    return res;
}

State *StateCloner::_make_rec( Scp &p, const char *msg ) {
    // already created ?
    TC::iterator iter = created.find( p.bid() );
    if ( iter != created.end() ) {
        if ( p.display_steps )
            coutn << p.display_prefix << "found same bid " << p.bid();
        return iter->second;
    }

    // if not authorized
    if ( not p.state->eq_op_id() )
        return 0;

    // if dst
    if ( p.state == p.dst )
        return new State;


    // else, get updated instructions
    Vec<const Instruction *> vi;
    int an = -1;
    for( int i = 0; i < p.state->instructions.size(); ++i ) {
        if ( p.state->instructions[ i ]->can_lead_to( p.dst->instructions[ 0 ], p.dst->visited ) ) {
            if ( p.state->action_num == i )
                an = vi.size();
            vi << p.state->instructions[ i ];
        }
    }

    // do we need a set_mark ?
    if ( an >= 0 and vi.size() >= 2 and not p.set_mark ) {
        State *res = _new_State( p );
        res->instructions = vi;
        res->set_mark = true;
        res->action_num = -1;
        p.set_mark = res;
        res->add_next( _make_rec( p, "set_mark" ) );
        return res;
    }

    // do we need a use_mark ?
    if ( p.set_mark and vi.size() == 1 ) {
        State *res = _new_State( p );
        res->use_mark = p.set_mark;
        res->action_num = -1;
        res->instructions = vi;
        p.set_mark = 0;
        res->add_next( _make_rec( p, "use_mark" ) );
        use_mark_stack << res;
        return res;
    }

    // else, make a copy (with a new next and a new prev)
    State *res = _new_State( p );
    res->instructions = vi;
    res->action_num = an;

    for( int i = 0; i < p.state->next.size(); ++i ) {
        State::Next n = p.state->next[ i ];
        Scp np = p; np.state = n.s;
        n.s = _make_rec( np, "cont" );
        if ( n.s )
            res->add_next( n );
    }

    return res;
}


