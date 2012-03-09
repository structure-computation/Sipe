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


bool Instruction::can_lead_to( const Instruction *dst, const std::set<const Instruction *> &allowed ) const {
    ++cur_op_id;
    return _can_lead_to_rec( dst, allowed );
}

bool Instruction::get_next_conds( Vec<const Instruction *> &conds, int nb_incc_allowed ) const {
    ++cur_op_id;
    return _get_next_conds_rec( conds, nb_incc_allowed );
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
    // os << freq << ",";
    if ( cond        ) return os << *cond;
    if ( incc        ) return os << "+" << incc;
    if ( code.size() ) return dot_out( os, code, 20 );
    if ( func        ) return dot_out( os, *func, 20 );
    if ( end         ) return os << ( end == 2 ? "KO" : "OK" );
    return os << ".";
}

void Instruction::write_code( StreamSepMaker<std::ostream> &os, Language *l ) const {
    if ( code.size() )
        os << code;
}

bool Instruction::immediate_execution() const {
    if ( not is_an_action() )
        return false;
    if ( func and func->name == "_add_attr" ) return true;
    if ( func and func->name == "_add_prel" ) return true;
    if ( func and func->name == "_set_strn" ) return true;
    return false;
}

bool Instruction::branching_only() const {
    return not cond and not incc and not code.size() and not func;
}


bool Instruction::is_an_action() const {
    return code.size() or func;
}

static inline bool lower ( char a ) { return a >= 'a' and a <= 'z'; }
static inline bool upper ( char a ) { return a >= 'A' and a <= 'Z'; }
static inline bool number( char a ) { return a >= '0' and a <= '9'; }
static inline bool letter( char a ) { return lower ( a ) or upper ( a ); }
static inline bool begvar( char a ) { return letter( a ) or a == '_'  ; }
static inline bool cntvar( char a ) { return begvar( a ) or number( a ); }

bool Instruction::needs_data() const {
    if ( code.size() ) {
        for( int res = code.find( "data" ); res >= 0; res = code.find( "data", res + 4 ) )
            if ( ( res == 0 or not cntvar( code[ res - 1 ] ) ) and ( unsigned( res + 4 ) == code.size() or not cntvar( code[ res + 4 ] ) ) )
                return true;
    }
    //    if ( func ) {
    //        if ( func->name == "_add_attr" ) return false;
    //    }
    return false;
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


bool Instruction::_can_lead_to_rec( const Instruction *dst, const std::set<const Instruction *> &allowed ) const {
    if ( op_id == cur_op_id )
        return false;
    op_id = cur_op_id;

    if ( this == dst )
        return true;

    for( int i = 0; i < next.size(); ++i ) {
        if ( next[ i ] == dst )
            return true;
        if ( not allowed.count( next[ i ] ) )
            continue;
        if ( next[ i ]->_can_lead_to_rec( dst, allowed ) )
            return true;
    }
    return false;
}

bool Instruction::can_lead_to_an_incc( const Vec<const Instruction *> &lst ) {
    ++cur_op_id;
    for( int i = 0; i < lst.size(); ++i )
        if ( lst[ i ]->_can_lead_to_an_incc_rec() )
            return true;
    return false;
}

bool Instruction::_can_lead_to_an_incc_rec() const {
    if ( op_id == cur_op_id )
        return false;
    op_id = cur_op_id;

    if ( incc )
        return true;

    for( int i = 0; i < next.size(); ++i )
        if ( next[ i ]->_can_lead_to_an_incc_rec() )
            return true;
    return false;
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

bool Instruction::_get_next_conds_rec( Vec<const Instruction *> &conds, int nb_incc_allowed ) const {
    if ( op_id == cur_op_id )
        return tmp;
    op_id = cur_op_id;

    if ( incc and nb_incc_allowed-- == 0 )
        return ( tmp = false );

    if ( cond ) {
        conds << this;
        return ( tmp = true );
    } else if ( next.size() ) {
        bool ok = true;
        for( int i = 0; i < next.size(); ++i )
            ok &= next[ i ]->_get_next_conds_rec( conds, nb_incc_allowed );
        return ( tmp = ok );
    }

    return ( tmp = false );
}

bool Instruction::_surely_leads_to_the_end_rec( const Cond *_cond, int nb_incc_allowed ) const {
    if ( op_id == cur_op_id )
        return false;
    op_id = cur_op_id;

    if ( incc ) {
         if ( nb_incc_allowed )
             return next[ 0 ]->_surely_leads_to_the_end_rec( _cond, nb_incc_allowed - 1 );
         return next[ 0 ]->_surely_leads_to_the_end_rec( 0, nb_incc_allowed );
    }

    if ( cond and ( not _cond or not _cond->included_in( *cond ) ) )
        return false;

    if ( not next.size() )
        return true;

    for( int i = 0; i < next.size(); ++i )
        if ( next[ i ]->_surely_leads_to_the_end_rec( _cond, nb_incc_allowed ) )
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
