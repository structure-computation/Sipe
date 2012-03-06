#include <sstream>
#include "Smp.h"

Smp::Smp() {
    display_steps = false;
    allow_incc = true;
    pending = 0;
}

String Smp::bid() const {
    std::ostringstream os;
    for( int i = 0; i < ok.size(); ++i )
        os << ( i ? " I" : "I" ) << ok[ i ];

    for( int i = 0; i < paction.size(); ++i )
        os << " P" << paction[ i ];

    if ( pending )
        os << " [pending=" << pending << "]";


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
    ok.remove( index );
}

void Smp::init( const Instruction *inst ) {
    ok << inst;
    visited.insert( inst );
}

bool Smp::surely_leads_to_the_end( int index ) {
    return ok[ index ]->surely_leads_to_the_end( cond.get_ptr(), allow_incc == 0 );
}

int Smp::next( int index ) {
    visited.insert( ok[ index ] );
    int n = ok[ index ]->next.size();
    ok.replace( index, ok[ index ]->next );
    return n;
}

std::ostream &operator<<( std::ostream &os, const Smp &p ) {
    for( int i = 0; i < p.ok.size(); ++i )
        os << ( i ? " " : "" ) << *p.ok[ i ];

    if ( p.cond )
        os << " [cond=" << *p.cond << "]";
    if ( not p.allow_incc )
        os << " [no incc]";
    if ( p.pending )
        os << " [pending=" << p.pending << "]";
    return os; // << " " << p.bid();
}
