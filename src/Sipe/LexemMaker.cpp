#include "LexemMaker.h"
#include <string.h>

static inline bool lower ( const char *a ) { return *a >= 'a' and *a <= 'z'; }
static inline bool upper ( const char *a ) { return *a >= 'A' and *a <= 'Z'; }
static inline bool number( const char *a ) { return *a >= '0' and *a <= '9'; }
static inline bool letter( const char *a ) { return lower ( a ) or upper ( a ); }
static inline bool begvar( const char *a ) { return letter( a ) or *a == '_'  ; }
static inline bool cntvar( const char *a ) { return begvar( a ) or number( a ); }

static const char *starts_with( const char *beg, const char *str ) {
    for( ; ; ++beg, ++str ) {
        if ( *str == 0 )
            return beg;
        if ( *beg != *str )
            return 0;
    }
}

LexemMaker::LexemMaker( ErrorList &error_list ) : error_list( error_list ), first_item( Lexem::NONE, 0, 0, 0 ) {
    // static information on operators
    num_grp_bracket = -1;
    #define OPERATOR( S, N, G ) if ( strcmp( S, "[" ) == 0 ) num_grp_bracket = G;
    #include "Operators.h"
    #undef OPERATOR
}

Lexem *LexemMaker::read( Source *source ) {
    // init
    last = &first_item;
    while ( last->next )
        last = last->next;
    Lexem *res = last;

    for( int i = 0; i < max_op_grp; ++i )
        sibling[ i ] = 0;

    // tokenize
    const char *data = source->data;
    while ( int inc = read_tok( source, data ) )
        data += inc;

    // operators
    enum { need_none = 0, need_larg = 1, need_rarg = 2, need_barg = 3 };
    int behavior[ max_op_grp ];
    #define OPERATOR( S, N, G ) behavior[ G ] = N;
    #include "Operators.h"
    #undef OPERATOR

    for( int num_grp = max_op_grp - 1; num_grp >= 0; --num_grp ) {
        for( Lexem *item = sibling[ num_grp ]; item; item = item->sibling ) {
            switch( behavior[ num_grp ] ) {
            case need_none: break;
            case need_larg: assemble_larg( item ); break;
            case need_rarg: assemble_rarg( item ); break;
            case need_barg: assemble_barg( item ); break;
            }
        }
    }

    return res->next;
}

const Lexem *LexemMaker::base() const {
    return &first_item;
}

const Lexem *LexemMaker::root() const {
    return first_item.next;
}

const Lexem *LexemMaker::operator()( const char *beg_name, const char *end_name ) const {
    const Lexem *res = 0;
    for( const Lexem *item = root(); item ; item = item->next )
        if ( item->eq( Lexem::OPERATOR, "=" ) and item->children[ 0 ]->eq( beg_name, end_name ) )
            res = item->next;
    return res;
}

const Lexem *LexemMaker::operator[]( const char *name ) const {
    return operator()( name, name + strlen( name ) );
}

int LexemMaker::read_tok( Source *source, const char *cur ) {
    const char *beg = cur;

    // variable
    if ( begvar( beg ) ) {
        while ( cntvar( ++cur ) );
        return push_tok( source, beg, cur, Lexem::VARIABLE );
    }

    // number
    if ( number( beg ) ) {
        while ( number( ++cur ) );
        return push_tok( source, beg, cur, Lexem::NUMBER );
    }

    // "{"
    if ( *beg == '{' )
        return push_cpp( source, beg );

    // '...'
    if ( *beg == '\'' or *beg == '"' )
        return push_str( source, beg, *beg );

    // #
    if ( *beg == '#' ) {
        while ( *( ++cur ) and *cur != '\n' );
        return cur - beg;
    }

    // "("
    if ( *beg == '(' or *beg == ')' or *beg == '[' or *beg == ']' ) {
        return push_tok( source, beg, beg + 1, Lexem::OPERATOR, *beg == '[' ? num_grp_bracket : -1, *beg );
    }

    // space
    if ( *beg == ' ' or *beg == '\n' or *beg == '\r' or *beg == '\t' )
        return 1;

    // operators
    #define OPERATOR( S, N, G ) if ( const char *end = starts_with( beg, S ) ) return push_tok( source, beg, end, Lexem::OPERATOR, G );
    #include "Operators.h"
    #undef OPERATOR

    // ...
    return *beg != 0;
}

bool LexemMaker::err( const Lexem *l, const char *msg ) {
    error_list.add( l->source, l->beg, msg );
    return false;
}

int LexemMaker::push_cpp( Source *source, const char *cur ) {
    const char *beg = cur++;

    for( int nba = 1; *cur and nba; ++cur ) {
        if ( *cur == '}' ) { // }
            --nba;
        } else if ( *cur == '{' ) { // {
            ++nba;
        } else if ( cur[ 0 ] == '/' and cur[ 1 ] == '/' ) { // // ...
            for( ; *( ++cur ) and *cur != '\n'; );
        } else if ( cur[ 0 ] == '/' and cur[ 1 ] == '*' ) { // /* ... */
            for( cur += 2; *cur and ( cur[ 0 ] != '*' or cur[ 1 ] != '/' ); ++cur );
            cur += *cur != 0;
        } else if ( *cur == '"' ) { // "..."
            for( ; *( ++cur ) != '"' and *cur; cur += cur[ 0 ] == '\\' and cur[ 1 ] == '"' );
        } else if ( *cur == '\'' ) { // '.'
            cur += 2 + ( cur[ 1 ] == '\\' );
        }
    }

    return push_tok( source, beg, cur, Lexem::CODE );
}

int LexemMaker::push_str( Source *source, const char *cur, char lim ) {
    const char *beg = cur;
    while ( *( ++cur ) != lim ) {
        if ( not *cur ) {
            error_list.add( source, beg, "Unterminated string" );
            return 0;
        }
    }
    return push_tok( source, beg + 1, cur, Lexem::STRING ) + 2;
}

int LexemMaker::push_tok( Source *source, const char *beg, const char *end, Lexem::Type type, int grp, char delim ) {
    if ( delim == ')' or delim == ']' ) {
        last = stack_last.pop();
        if ( ( delim == ')' and not last->eq( "(" ) ) or ( delim == ']' and not last->eq( "[" ) ) )
            err( last, "Non matching parenthesis." );
        return end - beg;
    }

    Lexem *item = new Lexem( type, source, beg, end );
    if ( last ) {
        last->next = item;
    } else {
        Lexem *p = stack_last.back();
        p->children[ p->eq( "[" ) ] = item;
        item->parent = p;
    }
    item->prev = last;
    last = item;

    if ( grp >= 0 ) {
        item->sibling = sibling[ grp ];
        sibling[ grp ] = item;
    } else
        item->sibling = 0;

    // coutn << String( beg, end ) << " " << grp << " " << type;
    if ( delim == '(' or delim == '[' ) {
        stack_last << item;
        last = 0;
    }

    return end - beg;
}

bool LexemMaker::assemble_rarg( Lexem *o ) {
    if ( not o->next )
        return err( o, "Operator needs a right expression." );
    if ( o->next->next )
        o->next->next->prev = o;

    o->children[ 0 ] = o->next;
    o->children[ 0 ]->parent = o;
    o->next = o->next->next;

    o->children[ 0 ]->prev = NULL;
    o->children[ 0 ]->next = NULL;
    return true;
}

bool LexemMaker::assemble_larg( Lexem *o ) {
    if ( not o->prev )
        return err( o, "Operator needs a left expression." );
    if ( o->prev->parent ) {
        o->parent = o->prev->parent;
        if (o->prev->parent->children[ 0 ] == o->prev)
            o->prev->parent->children[ 0 ] = o;
        else
            o->prev->parent->children[ 1 ] = o;
    }
    if ( o->prev->prev )
        o->prev->prev->next = o;

    o->children[ 0 ] = o->prev;
    o->children[ 0 ]->parent = o;
    o->prev = o->prev->prev;

    o->children[ 0 ]->next = NULL;
    o->children[ 0 ]->prev = NULL;
    return true;
}

bool LexemMaker::assemble_barg( Lexem *o, int need_left, int need_right ) {
    if ( need_right and not o->next )
        return err( o, "Operator needs a right expression." );
    if ( need_left and not o->prev )
        return err( o, "Operator needs a left expression." );
    // prev
    if ( o->prev->parent ) {
        o->parent = o->prev->parent;
        if (o->prev->parent->children[ 0 ] == o->prev)
            o->prev->parent->children[ 0 ] = o;
        else
            o->prev->parent->children[ 1 ] = o;
    }
    if ( o->prev->prev )
        o->prev->prev->next = o;

    o->children[ 0 ] = o->prev;
    o->children[ 0 ]->parent = o;
    o->prev = o->prev->prev;

    o->children[ 0 ]->next = NULL;
    o->children[ 0 ]->prev = NULL;

    // next
    if ( o->next->next )
        o->next->next->prev = o;

    o->children[ 1 ] = o->next;
    o->children[ 1 ]->parent = o;
    o->next = o->next->next;

    o->children[ 1 ]->prev = NULL;
    o->children[ 1 ]->next = NULL;

    return true;
}
