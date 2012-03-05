#include "Engine.h"
#include "Predef.h"

Engine::Engine() : instruction_maker( lexem_maker ), lexem_maker( error_list ) {
    sources = 0;

    di = false;
    ds = false;
    dl = false;
    ws = false;

    read( "predef", predef, false );
}

Engine::~Engine() {
    delete sources;
}

void Engine::read( const char *provenance, bool dl_allowed ) {
    sources = new Source( provenance, sources );
    if ( sources->data ) {
        Lexem *res = lexem_maker.read( sources );
        if ( dl_allowed and dl and res )
            res->display_dot( ".lex.dot" );
    }
}

void Engine::read( const char *provenance, const char *data, bool dl_allowed ) {
    sources = new Source( provenance, data, sources );
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
