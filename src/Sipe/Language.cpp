#include "Language.h"

Language::Language() {
}

Language::~Language() {
}

void Language::read_state( const State *state ) {
    if ( state->action ) {
        if ( state->action->func ) {
            if ( state->action->func->name == "_add_attr" ) {
                Attr a;
                a.decl = 0 < state->action->func->parm.u_params.size() ? state->action->func->parm.u_params[ 0 ] : "";
                a.init = 1 < state->action->func->parm.u_params.size() ? state->action->func->parm.u_params[ 1 ] : "";
                a.dest = 2 < state->action->func->parm.u_params.size() ? state->action->func->parm.u_params[ 2 ] : "";
                for( int i = 0; ; ++i ) {
                    if ( i == attributes.size() ) {
                        attributes << a;
                        break;
                    }
                    if ( attributes[ i ].decl == a.decl )
                        break;
                }
            }
        }
    }
}
