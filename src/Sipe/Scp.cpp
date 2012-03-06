#include "Scp.h"
#include <sstream>

Scp::Scp( State *state, State *dst ) : state( state ), dst( dst ) {
    display_steps = false;
    set_mark = 0;
    use_mark = 0;
}

String Scp::bid() const {
    std::ostringstream os;

    os << state;

    if ( set_mark )
        os << "_M" << set_mark;

    if ( use_mark )
        os << "_U" << use_mark;

    return os.str();
}

