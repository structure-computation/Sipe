#ifndef LANGUAGE_H
#define LANGUAGE_H

#include "State.h"
#include <map>

/**
*/
class Language {
public:
    Language();
    virtual ~Language();

    virtual void write( std::ostream &os, const State *state, bool write_main = false ) = 0;

protected:
    struct Attr {
        String decl;
        String init;
        String dest;
    };

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
        bool write;
        int op_id;
        int label;
        bool t_ok; ///< true if we have to write "if ( ok )"
    };

    Block *_unfold( const State *state, const State *mark = 0, int num_next = -1, const Cond &not_in = Cond() );
    String _bid( const State *state, int num_next, const Cond &not_in ) const;
    int    _simplify_label( Block *block, int take_label_if_not_present );
    int    _display_dot( Block *block, const char *f = ".block.dot" );
    void   _write_dot_rec( std::ostream &os, Block *block );
    void   _internal_state_read( const State *state );
    void   _make_labels_rec( Block *block );
    void   _assign_label( Block *block );
    void   _make_labels( Block *block );

    std::map<String,Block *> created;
    Vec<String> preliminaries;
    Vec<Block *> block_seq; ///< filled by _make_labels (optionnal)
    Vec<Attr> attributes;
    String struct_name;
    bool need_a_mark;
    int nb_labels;
    int cur_op_id;
};

#endif // LANGUAGE_H
