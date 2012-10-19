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


#ifndef ENGINE_H
#define ENGINE_H

#include "InstructionMaker.h"
#include "StateMaker.h"
#include "LexemMaker.h"
#include "CodeParm.h"
#include "Source.h"
#include "Stream.h"

/**
*/
class Engine {
public:
    Engine();
    ~Engine();

    void read( const char *provenance, const char *data, bool dl_allowed = true, bool need_cp = true );
    void read( const char *provenance, bool dl_allowed = true );
    Instruction *make_instruction_seq( const char *machine );
    State *make_state_seq( const Instruction *inst );
    State *make_state_seq( const char *machine );
    int ret() const; ///< return code

    CodeParm code_parm;
    bool dl; ///< display lexem graph ?
    bool di; ///< display instruction graph ?
    bool ds; ///< display state graph ?
    bool ws; ///< display states seq ?

protected:
    InstructionMaker instruction_maker;
    StateMaker state_maker;
    LexemMaker lexem_maker;
    ErrorList error_list;
    Vec<State *> to_del;
    Source *sources;
};

#endif // ENGINE_H
