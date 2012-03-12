#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "Lexem.h"
#include "Cond.h"
#include "Func.h"
#include "Ctr.h"
#include "Vec.h"
#include <set>

class Language;

/**
*/
class Instruction {
public:
    typedef enum { OK = 1, KO = 2 } EndType;

    Instruction( const Lexem *lex, double freq, const Cond &cond );
    Instruction( const Lexem *lex, double freq, EndType end );
    Instruction( const Lexem *lex, double freq, String code );
    Instruction( const Lexem *lex, double freq, int incc );
    Instruction( const Lexem *lex, double freq );
    virtual ~Instruction();

    bool can_lead_to( const Instruction *dst, const std::set<const Instruction *> &allowed ) const;
    bool get_next_conds( Vec<const Instruction *> &conds, int nb_incc_allowed ) const; ///< return true if all the branches have an accessible cond
    int display_dot( const char *f = ".inst.dot", const char *prg = "okular" ) const;
    bool surely_leads_to_the_end( const Cond *cond, int nb_incc_allowed ) const;
    void write_code( StreamSepMaker<std::ostream> &os, Language *l ) const; ///<
    std::ostream &write_label( std::ostream &os ) const; ///<
    void get_children( Vec<Instruction *> &vec );
    Instruction &operator<<( Instruction *n ); ///< add to next
    bool branching_only() const;
    bool is_an_action() const;
    bool needs_data() const; ///< true if data is needed for the action
    int ascii_val() const;

    static bool can_lead_to_an_incc( const Vec<const Instruction *> &lst );

    //
    Vec<Instruction *> next;
    Vec<Instruction *> prev;
    const Lexem *lex;

    //
    Ctr<Cond> cond;
    String code;
    double freq;
    int incc;
    int end;


protected:
    bool _can_lead_to_rec( const Instruction *dst, const std::set<const Instruction *> &allowed ) const;
    bool _get_next_conds_rec( Vec<const Instruction *> &conds, int nb_incc_allowed ) const;
    bool _surely_leads_to_the_end_rec( const Cond *cond, int nb_incc_allowed ) const;
    void _get_children_rec( Vec<Instruction *> &vec );
    void _write_dot_rec( std::ostream &os ) const;
    bool _can_lead_to_an_incc_rec() const;

    mutable Instruction *op_mp;
    static int cur_op_id;
    mutable int op_id;
    mutable int tmp;

    friend void init( Instruction *inst, const Lexem *lex, double freq );
    friend void del( Instruction *inst );
};

std::ostream &operator<<( std::ostream &os, const Instruction &inst );

#endif // INSTRUCTION_H
