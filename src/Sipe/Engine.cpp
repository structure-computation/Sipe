#include "Engine.h"
#include "Predef.h"

Engine::Engine() : instruction_maker( code_parm, lexem_maker ), state_maker( to_del ), lexem_maker( error_list ) {
    sources = 0;

    di = false;
    ds = false;
    dl = false;
    ws = false;

    read( "predef", predef, false, false );
}

Engine::~Engine() {
    for( int i = 0; i < to_del.size(); ++i )
        delete to_del[ i ];
    delete sources;
}

void Engine::read( const char *provenance, const char *data, bool dl_allowed, bool need_cp ) {
    sources = new Source( provenance, data, sources, need_cp );
    if ( sources->data ) {
        Lexem *res = lexem_maker.read( sources );
        if ( dl_allowed and dl and res )
            res->display_dot( ".lex.dot" );
    }
}

void Engine::read( const char *provenance, bool dl_allowed ) {
    sources = new Source( provenance, sources );
    if ( sources->data ) {
        Lexem *res = lexem_maker.read( sources );
        if ( dl_allowed and dl and res )
            res->display_dot( ".lex.dot" );
    }
}

Instruction *Engine::make_instruction_seq( const char *machine ) {
    Instruction *res = instruction_maker.make( machine );
    if ( res and di )
        res->display_dot();
    return res;
}

State *Engine::make_state_seq( const Instruction *inst ) {
    State *res = state_maker.make( inst, ws );
    if ( res and ds )
        res->display_dot();
    return res;
}

State *Engine::make_state_seq( const char *machine ) {
    return make_state_seq( make_instruction_seq( machine ) );
}

int Engine::ret() const {
    return not error_list;
}
