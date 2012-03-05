#ifndef LEXEMMAKER_H
#define LEXEMMAKER_H

#include "ErrorList.h"
#include "Lexem.h"
#include "Vec.h"

/**
*/
class LexemMaker {
public:
    LexemMaker( ErrorList &error_list );
    Lexem *read( Source *source );

    const Lexem *operator()( const char *beg_name, const char *end_name ) const; ///< find the last machine named $name
    const Lexem *operator[]( const char *name ) const; ///< find the last machine named $name
    const Lexem *base() const; ///< base lexem
    const Lexem *root() const; ///< first lexem in the graph (may be 0)
    bool err( const Lexem *l, const char *msg );

protected:
    int read_tok( Source *source, const char *cur ); ///< return increment

    int push_cpp( Source *source, const char *cur ); ///<
    int push_str( Source *source, const char *cur, char lim ); ///< for "..." or '...'
    int push_tok( Source *source, const char *beg, const char *end, Lexem::Type type, int grp = -1, char delim = 0 ); ///< return increment

    bool assemble_rarg( Lexem *o );
    bool assemble_larg( Lexem *o );
    bool assemble_barg( Lexem *o, int need_left = true, int need_right = true );

    enum { max_op_grp = 16 };

    Lexem *sibling[ max_op_grp ];
    Lexem first_item, *last;
    Vec<Lexem *> stack_last;
    ErrorList &error_list;
    int num_grp_bracket;
};

#endif // LEXEMMAKER_H
