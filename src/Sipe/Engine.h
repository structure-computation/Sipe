#ifndef ENGINE_H
#define ENGINE_H

#include "InstructionMaker.h"
#include "StateMaker.h"
#include "LexemMaker.h"
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
