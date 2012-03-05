#include <sstream>
#include "Smp.h"

Smp::Smp() {
    display_steps = false;
    allow_incc = true;
    pending = 0;
}

String Smp::bid() const {
    std::ostringstream os;
    for( int i = 0; i < paths.ends.size(); ++i )
        os << ( i ? " I" : "I" ) << paths.ends[ i ]->data;

    if ( pending )
        os << " [pending=" << pending << "]";

    if ( cond ) {
        Vec<const Instruction *> nc;
        for( int i = 0; i < paths.ends.size(); ++i )
            paths.ends[ i ]->data->get_next_conds( nc, not allow_incc );
        if ( nc.size() )
            os << " [C=" << *cond << "]";
    }

    // we add incc_forbidden only if it will be needed
    Vec<const Instruction *> ok;
    for( int i = 0; i < paths.ends.size(); ++i )
        ok << paths.ends[ i ]->data;
    if ( allow_incc == false and Instruction::can_lead_to_an_incc( ok ) )
        os << " [no incc]";


    return os.str();
}

void Smp::remove_branch( int index ) {
    paths.remove( index );
}

bool Smp::surely_leads_to_the_end( int index ) {
    return paths.ends[ index ]->data->surely_leads_to_the_end( cond.get_ptr(), allow_incc == 0 );
}

int Smp::next( int index ) {
    Vec<const Instruction *> n;
    n << paths.ends[ index ]->data->next;
    paths.next( index, n );
    return n.size();
}

std::ostream &operator<<( std::ostream &os, const Smp &p ) {
    for( int i = 0; i < p.paths.ends.size(); ++i )
        os << ( i ? " " : "" ) << *p.paths.ends[ i ]->data;


    if ( p.cond )
        os << " [cond=" << *p.cond << "]";
    if ( not p.allow_incc )
        os << " [no incc]";
    if ( p.pending )
        os << " [pending=" << p.pending << "]";
    return os; // << " " << p.bid();
}
