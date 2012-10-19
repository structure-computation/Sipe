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


#ifndef INSTRUCTIONMAKER_H
#define INSTRUCTIONMAKER_H

#include "Instruction.h"
#include "LexemMaker.h"
#include "FuncParm.h"
#include "CodeParm.h"
#include <map>

/**
*/
class InstructionMaker {
public:
    InstructionMaker( ErrorList &error_list, CodeParm &code_parm, LexemMaker &lexem_maker );
    ~InstructionMaker();

    Instruction *make( const char *name );

protected:
    struct Par {
        std::map<String,Instruction *> *labels;
        FuncParm params;
        double freq;
    };

    void _get_labels_rec( std::map<String,Instruction *> &labels, const Lexem *lex );
    Instruction *app( Instruction *inst, const Lexem *lex, Par par );
    Instruction *app( Instruction *inst, Instruction *next );

    std::map<String,Instruction> labels;
    Vec<Instruction *> to_del;
    ErrorList &error_list;
    LexemMaker &lexem_maker;
    CodeParm &code_parm;
};

#endif // INSTRUCTIONMAKER_H
