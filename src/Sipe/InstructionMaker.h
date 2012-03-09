#ifndef INSTRUCTIONMAKER_H
#define INSTRUCTIONMAKER_H

#include "Instruction.h"
#include "LexemMaker.h"
#include "FuncParm.h"
#include <map>

/**
*/
class InstructionMaker {
public:
    InstructionMaker( LexemMaker &lexem_maker );
    ~InstructionMaker();

    Instruction *make( const char *name );

protected:
    struct Par {
        std::map<String,Instruction *> lab_to_inst;
        Vec<Instruction *> goto_inst;
        FuncParm params;
        double freq;
    };

    Instruction *app( Instruction *inst, const Lexem *lex, Par par );
    Instruction *app( Instruction *inst, Instruction *next );
    void _get_labels_rec( const Lexem *lex );

    std::map<String,Instruction> labels;
    Vec<Instruction *> to_del;
    LexemMaker &lexem_maker;
};

#endif // INSTRUCTIONMAKER_H
