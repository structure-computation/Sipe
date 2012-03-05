#ifndef STATE_H
#define STATE_H

#include "Instruction.h"
#include "Paths.h"

/**
*/
class State {
public:
    struct Next {
        double freq;
        Cond cond;
        State *s;
    };

    State();
    void add_next( State *s );
    void add_next( const Next &n );
    std::ostream &write_label( std::ostream &os, int lim = -1 ) const;
    int display_dot( const char *f = ".state.dot", const char *prg = 0 ) const;

    //
    Paths<const Instruction *> paths;
    const Instruction *action;
    Vec<State *> prev;
    bool   set_mark;
    State *use_mark;
    State *rem_mark;
    Vec<Next> next;
    int incc;
    int end;

    // graph manipulation
    static int cur_op_id;
    mutable int    op_id;
    mutable State *op_mp;

protected:
    void _write_dot_rec( std::ostream &os ) const;
};

std::ostream &operator<<( std::ostream &os, const State &state );

#endif // STATE_H
