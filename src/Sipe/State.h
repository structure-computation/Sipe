#ifndef STATE_H
#define STATE_H

#include "Instruction.h"
#include <set>

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
    State *simplified();
    bool eq_op_id() const;
    void add_next( State *s );
    void add_next( const Next &n );
    bool will_write_something() const;
    void prev_mark( State *stop_at ) const;
    void get_children( Vec<State *> &res );
    void get_end_points( Vec<State *> &res );
    void insert_between_this_and_next( State *nst );
    std::ostream &write_label( std::ostream &os, int lim = -1 ) const;
    bool has_something_to_execute( bool take_incc_into_account ) const;
    bool has_data_dependant_actions( Vec<const Instruction *> &undep ) const;
    int  display_dot( const char *f = ".state.dot", const char *prg = 0, bool par = true ) const;

    //
    Vec<const Instruction *> instructions;
    std::set<const Instruction *> visited;
    const Instruction *action; ///<
    Vec<bool> used_marks; ///< if set_mark
    Vec<State *> prev;
    bool   set_mark;
    State *use_mark;
    State *rem_mark;
    int action_num;
    Vec<Next> next;
    int incc;
    int end;

    // graph manipulation
    static int cur_op_id;
    mutable int    op_id;
    mutable State *op_mp;

protected:
    void _repl_mark_rec();
    bool _simplify_next_list();
    void _update_next_prev_rec( State *p );
    bool _can_lead_to_rec( const State *s );
    void _prev_mark_rec( State *stop_at ) const;
    void _get_children_rec( Vec<State *> &res );
    void _get_end_points_rec( Vec<State *> &res );
    void _write_dot_rec( std::ostream &os ) const;
    bool _has_something_to_execute_rec( bool take_incc_into_account ) const;
    bool _is_interesting( bool take_incc_into_account, bool take_nb_next_into_account ) const;
    bool _is_interesting_rec( bool take_incc_into_account, bool take_nb_next_into_account ) const;
    bool _has_data_dependant_actions_rec( Vec<const Instruction *> &undep, bool allow_undep_actions ) const;
};

std::ostream &operator<<( std::ostream &os, const State &state );

#endif // STATE_H
