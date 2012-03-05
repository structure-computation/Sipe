#ifndef INSTRUCTIONMAKER_H
#define INSTRUCTIONMAKER_H

#include "Instruction.h"
#include "LexemMaker.h"
#include "FuncParm.h"

/**
*/
class InstructionMaker {
public:
    InstructionMaker( LexemMaker &lexem_maker );
    ~InstructionMaker();

    Instruction *make( const char *name );

protected:
    Instruction *app( Instruction *inst, const Lexem *lex, FuncParm params, double freq );
    Instruction *app( Instruction *inst, Instruction *next );

    Vec<Instruction *> to_del;
    LexemMaker &lexem_maker;
};

#endif // INSTRUCTIONMAKER_H
