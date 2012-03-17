#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "CodeParm.h"
#include "State.h"
#include <map>

/**
*/
class Language {
public:
    Language();
    virtual ~Language();

    virtual void write( std::ostream &os, const CodeParm &cp, const State *state, bool write_main = false ) = 0;

    bool db; ///< display blocks
protected:
    struct Block {
        // filled by _unfold
        const State *state;
        const State *mark;
        Block *write_goto; ///< after the tests
        Cond not_in;
        Cond cond; ///< used if ko != 0
        Block *ok;
        Block *ko;

        // helper attributes for children Languages
        std::set<int> used_marks;
        Block *op_mp;
        bool write;
        int op_id;
        int label;
        bool t_ok; ///< true if we have to write "if ( ok )"
    };

    Block *_unfold_rec( const State *state, const State *mark = 0, int num_next = -1, const Cond &not_in = Cond() );
    void   _get_used_marks_rec( std::set<int> &used_marks, Block *b, const State *set_mark );
    String _bid( const State *state, int num_next, const Cond &not_in ) const;
    void   _set_used_marks_rec( int mark, Block *b, const State *set_mark );
    int    _simplify_label( Block *block, int take_label_if_not_present );
    void   _write_dot_rec( std::ostream &os, Block *block );
    int    _display_dot( Block *block, const char *f = ".block.dot" );
    void   _get_ch_rec( Vec<Block *> &res, Block *block );
    void   _internal_state_read( const State *state );
    void   _make_labels_rec( Block *block );
    void   _assign_label( Block *block );
    Block *_unfold( const State *state );
    void   _make_labels( Block *block );
    Block *_simplified( Block *block );
    void   _get_mark( Block *block );

    std::map<const State *,int> marks;
    std::map<String,Block *> created;
    Vec<Block *> block_seq; ///< filled by _make_labels (optionnal)
    int nb_labels;
    int nb_marks;
    int cur_op_id;
};

#endif // LANGUAGE_H
