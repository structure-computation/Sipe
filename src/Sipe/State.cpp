#include "DotOut.h"
#include "State.h"

#include <algorithm>
#include <assert.h>

int State::cur_op_id = 0;

State::State() {
    op_id = 0;

    action_num = -1;
    action   = 0;
    set_mark = 0;
    use_mark = 0;
    rem_mark = 0;
    incc = 0;
    end = 0;
}

void State::add_next( const Next &n ) {
    if ( n.s ) {
        next << n;
        n.s->prev << this;
    }
}

void State::add_next( State *s ) {
    Next n;
    n.s = s;
    n.freq = 1;
    n.cond = Cond::full;
    add_next( n );
}

int State::display_dot( const char *f, const char *prg, bool par ) const {
    std::ofstream os( f );

    os << "digraph LexemMaker {\n";
    ++cur_op_id;
    _write_dot_rec( os );
    os << "}\n";

    os.close();
    return exec_dot( f, prg, true, par );
}

std::ostream &State::write_label( std::ostream &os, int lim ) const {
    if ( incc )
        os << "+1";
    if ( set_mark )
        os << "S" << this;
    if ( use_mark )
        os << "U" << use_mark;
    if ( rem_mark )
        os << "R" << rem_mark;
    if ( next.size() >= 2 )
        os << "?";
    if ( end )
        os << ( end == 2 ? "KO" : "OK" );
    if ( action )
        action->write_label( os );

    if ( action_num >= 0 ) {
        for( int i = 0; i < instructions.size(); ++i ) {
            if ( action_num == i )
                os << "<";
            os << *instructions[ i ];
            if ( action_num == i )
                os << ">";
            if ( i + 1 < instructions.size() )
                os << " or\\n";
        }
    }

    //for( int i = 0; i < paths.ends.size(); ++i )
    //    os << ( i ? " or\\n" : "" ) << *paths.ends[ i ];
    return os;
}

void State::get_children( Vec<State *> &res ) {
    ++cur_op_id;
    _get_children_rec( res );
}

void State::get_end_points( Vec<State *> &res ) {
    ++cur_op_id;
    _get_end_points_rec( res );
}

bool State::has_something_to_execute( bool take_incc_into_account ) const {
    ++cur_op_id;
    return _has_something_to_execute_rec( take_incc_into_account );
}

bool State::has_data_dependant_actions( Vec<const Instruction *> &undep ) const {
    ++cur_op_id;
    return _has_data_dependant_actions_rec( undep, true );
}

void State::prev_mark( State *stop_at ) const {
    ++cur_op_id;
    _prev_mark_rec( stop_at );
}

bool State::eq_op_id() const {
    return op_id == cur_op_id;
}

struct SortNext {
    bool operator()( const State::Next &a, const State::Next &b ) const {
        return a.freq > b.freq;
    }
};


State *State::simplified() {
    Vec<State *> ch;
    get_children( ch );

    // init op_mp and find ends
    Vec<State *> ends;
    for( int i = 0; i < ch.size(); ++i ) {
        ch[ i ]->op_mp = ch[ i ];
        if ( ch[ i ]->end )
            ends << ch[ i ];
    }

    // for each state, look if it leads to nothing interesting but the end
    for( int i = 0; i < ch.size(); ++i ) {
        ++cur_op_id;
        if ( not ch[ i ]->_is_interesting_rec( false, false ) ) {
            Vec<State *> accessible_ends;
            if ( ends.size() == 1 ) {
                accessible_ends << ends;
            } else {
                for( int e = 0; e < ends.size(); ++e ) {
                    ++cur_op_id;
                    if ( ch[ i ]->_can_lead_to_rec( ends[ e ] ) )
                         accessible_ends << ends[ e ];
                }
            }

            if ( accessible_ends.size() == 1 )
                ch[ i ]->op_mp = accessible_ends[ 0 ];
        }
    }

    // for each unmodified state, jump to next interesting states
    for( int i = 0; i < ch.size(); ++i )
        if ( ch[ i ] == ch[ i ]->op_mp and not ch[ i ]->_is_interesting( true, true ) )
            ch[ i ]->op_mp = ch[ i ]->next[ 0 ].s->op_mp;

    // try to simplify the next lists that lead to the same state
    while ( true ) {
        bool cnt = false;
        for( int i = 0; i < ch.size(); ++i )
            if ( ch[ i ] == ch[ i ]->op_mp )
                cnt |= ch[ i ]->_simplify_next_list();
        if ( not cnt )
            break;
    }


    // output
    State *res = this;
    while ( res != res->op_mp )
        res = res->op_mp;

    // sort nexts
    for( int i = 0; i < ch.size(); ++i )
        std::sort( ch[ i ]->next.beg(), ch[ i ]->next.end(), SortNext() );

    ++cur_op_id;
    res->_update_next_prev_rec( 0 );
    return res;
}

bool State::will_write_something() const {
    return ( action_num >= 0 and not instructions[ action_num ]->immediate_execution() ) or
            ( action and not action->immediate_execution() ) or
            end or
            set_mark or
            use_mark or
            rem_mark or
            incc;
}

void State::insert_between_this_and_next( State *nst ) {
    // P( next.size() );
    //if ( next.size() != 1 )
    //    display_dot();
    assert( next.size() == 1 ); //  and next[ 0 ].cond.always_checked()

    State *on = next[ 0 ].s;
    on->prev.erase( this );

    next[ 0 ].s = nst;
    nst->prev << this;

    Vec<State *> ep;
    nst->get_end_points( ep );
    if( not ep.size() )
        nst->display_dot( ".tmp", 0, false );
    for( int i = 0; i < ep.size(); ++i )
        ep[ i ]->add_next( on );
}

bool State::_simplify_next_list() {
    if ( next.size() <= 1 )
        return false;

    for( int i = 1; i < next.size(); ++i ) {
        State *opi = next[ i ].s;
        while ( opi != opi->op_mp )
            opi = opi->op_mp;

        for( int j = 0; j < i; ++j ) {
            State *opj = next[ j ].s;
            while ( opj != opj->op_mp )
                opj = opj->op_mp;

            if ( opj == opi ) {
                next[ j ].freq += next[ i ].freq;
                next[ j ].cond |= next[ i ].cond;
                next.remove( i-- );
                break;
            }
        }
    }

    if ( next.size() == 1 ) {
        op_mp = next[ 0 ].s->op_mp;
        return true;
    }
    return false;
}


void State::_update_next_prev_rec( State *p ) {
    if ( op_id == cur_op_id ) {
        if ( p )
            prev << p;
        return;
    }
    op_id = cur_op_id;

    prev.resize( 0 );
    if ( p )
        prev << p;

    for( int i = 0; i < next.size(); ++i ) {
        State *n = next[ i ].s;
        while ( n != n->op_mp )
            n = n->op_mp;
        next[ i ].s = n;
        next[ i ].s->_update_next_prev_rec( this );
    }
}

bool State::_is_interesting_rec( bool take_incc_into_account, bool take_nb_next_into_account ) const {
    if ( op_id == cur_op_id )
        return false;
    op_id = cur_op_id;

    if ( _is_interesting( take_incc_into_account, take_nb_next_into_account ) )
        return true;
    for( int i = 0; i < next.size(); ++i )
        if ( next[ i ].s->_is_interesting_rec( take_incc_into_account, take_nb_next_into_account ) )
            return true;
    return false;
}

bool State::_is_interesting( bool take_incc_into_account, bool take_nb_next_into_account ) const {
    return
            ( action_num == 0 and instructions.size() == 1 and not instructions[ 0 ]->end ) or
            ( next.size() != 1 and take_nb_next_into_account ) or
            ( incc and take_incc_into_account ) or
            action or
            ( set_mark and used_marks.any() ) or
            ( rem_mark and rem_mark->used_marks.any() ) or
            use_mark;
}

void State::_write_dot_rec( std::ostream &os ) const {
    if ( op_id == cur_op_id )
        return;
    op_id = cur_op_id;

    os << "  node_" << this << " [label=\"";
    write_label( os, 10 );
    os << "\"];";

    for( int i = 0; i < next.size(); ++i ) {
        os << "  node_" << this << " -> node_" << next[ i ].s;
        if ( i and not next[ i ].cond.always_checked() )
            os << " [label=\"" << next[ i ].cond << "\"]";
        //  << "," << next[ i ].freq
        else if ( next.size() >= 2 )
            os << " [color=\"red\"]";
        os << ";";

        next[ i ].s->_write_dot_rec( os );
    }

    //for( int i = 0; i < prev.size(); ++i )
    //    os << "  node_" << prev[ i ] << " -> node_" << this << " [color=\"blue\"];";
}

void State::_repl_mark_rec() {
    if ( op_id == cur_op_id )
        return;
    op_id = cur_op_id;

    if ( use_mark )
        use_mark = use_mark->op_mp;
    if ( rem_mark )
        rem_mark = rem_mark->op_mp;

    for( int i = 0; i < next.size(); ++i )
        next[ i ].s->_repl_mark_rec();
}

void State::_prev_mark_rec( State *stop_at ) const {
    if ( this == stop_at )
        return;

    if ( op_id == cur_op_id )
        return;
    op_id = cur_op_id;

    for( int i = 0; i < prev.size(); ++i )
        prev[ i ]->_prev_mark_rec( stop_at );
}

void State::_get_children_rec( Vec<State *> &res ) {
    if ( op_id == cur_op_id )
        return;
    op_id = cur_op_id;

    res << this;
    for( int i = 0; i < next.size(); ++i )
        next[ i ].s->_get_children_rec( res );
}

void State::_get_end_points_rec( Vec<State *> &res ) {
    if ( op_id == cur_op_id )
        return;
    op_id = cur_op_id;

    if ( next.size() == 0 )
        res << this;
    else
        for( int i = 0; i < next.size(); ++i )
            next[ i ].s->_get_end_points_rec( res );
}


bool State::_can_lead_to_rec( const State *s ) {
    if ( op_id == cur_op_id )
        return false;
    op_id = cur_op_id;

    if ( this == s )
        return true;

    for( int i = 0; i < next.size(); ++i )
        if ( next[ i ].s->_can_lead_to_rec( s ) )
            return true;
    return false;
}

bool State::_has_data_dependant_actions_rec( Vec<const Instruction *> &undep, bool allow_undep_actions ) const {
    if ( op_id == cur_op_id )
        return false;
    op_id = cur_op_id;

    // remaining doubts ?
    if ( action_num >= 0 and instructions.size() >= 2 )
        return true;

    // needs data ?
    if ( action_num >= 0 ) {
        if ( allow_undep_actions == false or instructions[ action_num ]->needs_data() )
            return true;
        undep << instructions[ action_num ];
    }

    for( int i = 0; i < next.size(); ++i )
        if ( next[ i ].s->_has_data_dependant_actions_rec( undep, allow_undep_actions and next.size() == 1 ) )
            return true;
    return false;
}

bool State::_has_something_to_execute_rec( bool take_incc_into_account ) const {
    if ( op_id == cur_op_id )
        return false;
    op_id = cur_op_id;

    if ( action_num >= 0 and not instructions[ action_num ]->end )
        return true;

    if ( incc and take_incc_into_account )
        return true;

    for( int i = 0; i < next.size(); ++i )
        if ( next[ i ].s->_has_something_to_execute_rec( take_incc_into_account ) )
            return true;
    return false;
}

std::ostream &operator<<( std::ostream &os, const State &state ) {
    return state.write_label( os );
}
