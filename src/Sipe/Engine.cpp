/*
 Copyright 2012 Structure Computation  www.structure-computation.com
 Copyright 2012 Hugo Leclerc

 This file is part of Sipe.

 Sipe is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Sipe is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.
 You should have received a copy of the GNU General Public License
 along with Sipe. If not, see <http://www.gnu.org/licenses/>.
*/


#include "Engine.h"
#include "Predef.h"

Engine::Engine() : instruction_maker( error_list, code_parm, lexem_maker ), state_maker( to_del ), lexem_maker( error_list ) {
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
