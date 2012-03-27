#include <sstream>
#include "Smp.h"

Smp::Smp() {
    use_mark_from_pending = false;
    action_next_round = 0;
    display_steps = false;
    allow_incc = true;
    mark = 0;
}

String Smp::bid() const {
    std::ostringstream os;
    for( int i = 0; i < ok.size(); ++i )
        os << ( i ? " I" : "I" ) << ok[ i ];

    for( int i = 0; i < pending.size(); ++i ) {
        os << " [pending=[" << pending[ i ].bid() << "] n2k=" << pending[ i ].nok_to_pok << "]";
    }

    if ( mark )
        os << " [mark]";

    if ( use_mark_from_pending )
        os << " [use_mark_from_pending]";

    if ( action_next_round )
        os << " [action_next_round=" << action_next_round << "]";

    if ( cond ) {
        Vec<const Instruction *> nc;
        for( int i = 0; i < ok.size(); ++i )
            ok[ i ]->get_next_conds( nc, not allow_incc );
        if ( nc.size() )
            os << " [C=" << *cond << "]";
    }

    // we add incc_forbidden only if it will be needed
    if ( allow_incc == false and Instruction::can_lead_to_an_incc( ok ) )
        os << " [no incc]";

    return os.str();
}

void Smp::join_branches( int j, int i ) {
    remove_branch( i );
}

void Smp::remove_branch( int index ) {
    for( int i = 0; i < pending.size(); ++i ) {
        int v = pending[ i ].nok_to_pok[ index ], cv = 0;
        pending[ i ].nok_to_pok.remove( index );
        for( int j = 0; j < pending[ i ].nok_to_pok.size(); ++j )
            cv += pending[ i ].nok_to_pok[ j ] == v;
        if ( not cv ) {
            pending[ i ].ok.remove( v );
            for( int j = 0; j < pending[ i ].nok_to_pok.size(); ++j )
                pending[ i ].nok_to_pok[ j ] -= pending[ i ].nok_to_pok[ j ] > v;
        }
    }

    ok.remove( index );
    rm_double_pending();
}

void Smp::init( const Instruction *inst ) {
    ok << inst;
}

bool Smp::surely_leads_to_the_end( int index ) {
    return ok[ index ]->surely_leads_to_the_end( cond.get_ptr(), allow_incc == 0 );
}

int Smp::next( int index ) {
    int n = ok[ index ]->next.size();
    for( int i = 0; i < pending.size(); ++i ) {
        int v = pending[ i ].nok_to_pok[ index ];
        for( int j = 0; j < n - 1; ++j )
            pending[ i ].nok_to_pok.insert( index, v );
    }

    ok.replace( index, ok[ index ]->next );
    rm_double_pending();
    return n;
}

void Smp::rm_double_pending() {
    for( int i = 0; i < pending.size(); ++i ) {
        for( int j = 0; j < i; ++j ) {
            if ( pending[ j ].bid() == pending[ i ].bid() and pending[ j ].nok_to_pok == pending[ i ].nok_to_pok ) {
                pending.remove( i-- );
                P( "REMOVED" );
                break;
            }
        }
    }
}

std::ostream &operator<<( std::ostream &os, const Smp &p ) {
    for( int i = 0; i < p.ok.size(); ++i )
        os << ( i ? " " : "" ) << *p.ok[ i ];

    if ( p.cond )
        os << " [cond=" << *p.cond << "]";
    if ( not p.allow_incc )
        os << " [no incc]";
    for( int i = 0; i < p.pending.size(); ++i )
        os << " [pending=" << p.pending[ i ].ok << " n2p=" << p.pending[ i ].nok_to_pok << "]";
    if ( p.mark )
        os << " [mark]";
    return os; // << " " << p.bid();
}
