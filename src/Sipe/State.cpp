#include "DotOut.h"
#include "State.h"

int State::cur_op_id = 0;

State::State() {
    op_id = 0;

    set_mark = 0;
    use_mark = 0;
    rem_mark = 0;
    action = 0;
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

int State::display_dot( const char *f, const char *prg ) const {
    std::ofstream os( f );

    os << "digraph LexemMaker {\n";
    ++cur_op_id;
    _write_dot_rec( os );
    os << "}\n";

    os.close();
    return exec_dot( f, prg );
}

std::ostream &State::write_label( std::ostream &os, int lim ) const {
    if ( action )
        action->write_label( os );
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

    //for( int i = 0; i < paths.ends.size(); ++i )
    //    os << ( i ? " or\\n" : "" ) << *paths.ends[ i ];
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
        else if ( next.size() >= 2 )
            os << " [color=\"red\"]";
        os << ";";

        next[ i ].s->_write_dot_rec( os );
    }

    //for( int i = 0; i < prev.size(); ++i )
    //    os << "  node_" << prev[ i ] << " -> node_" << this << " [color=\"blue\"];";
}


std::ostream &operator<<( std::ostream &os, const State &state ) {
    return state.write_label( os );
}
