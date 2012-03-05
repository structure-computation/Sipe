#include "Instruction.h"
#include "DotOut.h"
#include <fstream>

int Instruction::cur_op_id = 0;

void init( Instruction *inst, const Lexem *lex, double freq ) {
    inst->freq  = freq;
    inst->lex   = lex;
    inst->op_id = 0;
    inst->incc  = 0;
    inst->end   = 0;
}

Instruction::Instruction( const Lexem *lex, double freq, EndType end ) {
    init( this, lex, freq );
    this->end = end;
}

Instruction::Instruction( const Lexem *lex, double freq, String code ) {
    init( this, lex, freq );
    this->code = code;
}

Instruction::Instruction( const Lexem *lex, double freq, const Func &func ) {
    init( this, lex, freq );
    this->func = func;
}

Instruction::Instruction( const Lexem *lex, double freq, const Cond &cond ) {
    init( this, lex, freq );
    this->cond = cond;
}

Instruction::Instruction( const Lexem *lex, double freq, int incc ) {
    init( this, lex, freq );
    this->incc = incc;
}

Instruction::Instruction( const Lexem *lex, double freq ) {
    init( this, lex, freq );
}


Instruction::~Instruction() {
}

int Instruction::display_dot( const char *f, const char *prg ) const {
    std::ofstream os( f );

    os << "digraph LexemMaker {\n";
    ++cur_op_id;
    _write_dot_rec( os );
    os << "}\n";

    os.close();

    return exec_dot( f, prg );
}

bool Instruction::surely_leads_to_the_end( const Cond *cond, int nb_incc_allowed ) const {
    ++cur_op_id;
    return _surely_leads_to_the_end_rec( cond, nb_incc_allowed );
}

std::ostream &Instruction::write_label( std::ostream &os ) const {
    if ( cond        ) return os << *cond;
    if ( incc        ) return os << "+" << incc;
    if ( code.size() ) return dot_out( os, code, 10 );
    if ( func        ) return dot_out( os, *func, 10 );
    if ( end         ) return os << ( end == 2 ? "KO" : "OK" );
    return os << ".";
}

bool Instruction::is_an_action() const {
    return cond or func;
}

int Instruction::ascii_val() const {
    return cond and cond->nz() == 1 ? cond->first_nz() : -1;
}

void Instruction::get_children( Vec<Instruction *> &vec ) {
    ++Instruction::cur_op_id;
    _get_children_rec( vec );
}

Instruction &Instruction::operator<<( Instruction *n ) {
    if ( n ) {
        this->next << n;
        n->prev << this;
    }
    return *this;
}

void Instruction::_write_dot_rec( std::ostream &os ) const {
    if ( op_id == cur_op_id )
        return;
    op_id = cur_op_id;

    os << "  node_" << this << " [label=\"";
    write_label( os );
    os << "\"];\n";

    for( int u = 0; u < next.size(); ++u ) {
        next[ u ]->_write_dot_rec( os );

        os << "  node_" << this << " -> node_" << next[ u ];
        if ( next.size() >= 2 )
            os << " [label=\"" << u << "\"]";
        os << ";\n";
    }
}

bool Instruction::_surely_leads_to_the_end_rec( const Cond *cond, int nb_incc_allowed ) const {
    if ( op_id == cur_op_id )
        return false;
    op_id = cur_op_id;

    if ( not next.size() )
        return true;

    for( int i = 0; i < next.size(); ++i )
        if ( next[ i ]->_surely_leads_to_the_end_rec( cond, nb_incc_allowed ) )
            return true;
    return false;
}

void Instruction::_get_children_rec( Vec<Instruction *> &vec ) {
    if ( op_id == cur_op_id )
        return;
    op_id = cur_op_id;

    vec << this;
    for( int i = 0; i < next.size(); ++i )
        next[ i ]->_get_children_rec( vec );
}

void del( Instruction *inst ) {
    Vec<Instruction *> vec;
    inst->get_children( vec );
    for( int i = 0; i < vec.size(); ++i )
        delete vec[ i ];
}

std::ostream &operator<<( std::ostream &os, const Instruction &inst ) {
    return inst.write_label( os );
}
