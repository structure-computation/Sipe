#include "Language.h"
#include "DotOut.h"

Language::Language() {
    nb_marks    = 0;
    nb_labels   = 0;
    cur_op_id   = 1;
}

Language::~Language() {
    for( std::map<String,Block *>::iterator iter = created.begin(); iter != created.end(); ++iter )
        delete iter->second;
}

void Language::_internal_state_read( const State *state ) {
}

Language::Block *Language::_unfold( const State *state, const State *mark, int num_next, const Cond &not_in ) {
    if ( num_next == state->next.size() )
        return 0;
    if ( num_next >= 0 and num_next + 1 == state->next.size() )
        return _unfold( state->next[ num_next ].s, mark );

    String bid = _bid( state, num_next, not_in );
    if ( created.count( bid ) )
        return created[ bid ];

    if ( num_next < 0 ) {
        _internal_state_read( state );
        if ( state->set_mark )
            mark = state;
        if ( state->use_mark or state->rem_mark )
            mark = 0;
    }

    Block *res = new Block;
    created[ bid ] = res;

    res->op_id = 0;
    res->label = -1;
    res->write = true;
    res->t_ok  = false;
    res->write_goto = 0;
    res->not_in = not_in;
    if ( num_next < 0 ) {
        res->state = state;
        res->ok = _unfold( state, mark, 0 );
        res->ko = 0;
    } else {
        res->state = 0;
        res->cond = state->next[ num_next ].cond;
        res->ok = _unfold( state->next[ num_next ].s, mark );
        res->ko = _unfold( state, mark, num_next + 1, not_in | res->cond );
    }

    return res;
}

String Language::_bid( const State *state, int num_next, const Cond &not_in ) const {
    std::ostringstream ss;
    if ( num_next >= 0 ) {
        for( int i = num_next; i < state->next.size(); ++i )
            ss << " " << state->next[ i ].s << " " << state->next[ i ].cond;
        ss << " " << not_in;
    } else
        ss << state << " " << num_next << " " << not_in;
    return ss.str();
}


void Language::_make_labels_rec( Block *block ) {
    if ( block->op_id == cur_op_id )
        return;
    block->op_id = cur_op_id;
    block_seq << block;

    // particular case
    if ( block->state and block->state->end )
        _assign_label( block );

    // prepare jumps
    if ( block->ok ) {
        if ( block->ok->op_id == cur_op_id ) { // if ok already written
            if ( block->ko ) {
                if ( block->ko->op_id == cur_op_id ) { // if ko already written
                    _assign_label( block->ok ); // if ( not cond ) goto ko; goto ok;
                    _assign_label( block->ko );
                    block->write_goto = block->ok;
                } else {
                    block->t_ok = true; // if ( cond ) goto ok; [ ko; ]
                    _assign_label( block->ok );
                    _make_labels_rec( block->ko );
                }
            } else {
                _assign_label( block->ok ); // goto ok
                block->write_goto = block->ok;
            }
        } else { // ok not written
            if ( block->ko )
                _assign_label( block->ko ); // if ( not cond ) goto ko; [ ok; ]

            _make_labels_rec( block->ok );

            if ( block->ko and block->ko->op_id != cur_op_id )
                _make_labels_rec( block->ko );
        }
    }
}

int Language::_simplify_label( Block *block, int take_label_if_not_present ) {
    if ( block->label < 0 and take_label_if_not_present >= 0 )
        block->label = take_label_if_not_present;

    if ( ( block->state == 0 or not block->state->will_write_something() ) and block->label >= 0 and block->ko == 0 ) {
        block->write = false;
        if ( block->write_goto )
            return ( block->label = _simplify_label( block->write_goto, block->label ) );
        if ( block->ok )
            return ( block->label = _simplify_label( block->ok, block->label ) );
    }

    return block->label;
}

void Language::_make_labels( Block *block ) {
    ++cur_op_id;
    int os = block_seq.size();
    _make_labels_rec( block );

    //
    for( int i = os; i < block_seq.size(); ++i )
        _simplify_label( block_seq[ i ], -1 );

    //
    for( int i = 0; i < block_seq.size(); ++i )
        if ( block_seq[ i ]->state and block_seq[ i ]->state->set_mark )
            _get_mark( block_seq[ i ] );
}

void Language::_get_used_marks_rec( std::set<int> &used_marks, Block *block, const State *set_mark ) {
    if ( block == 0 or block->op_id == cur_op_id )
        return;
    block->op_id = cur_op_id;

    used_marks.insert( block->used_marks.begin(), block->used_marks.end() );

    if ( block->state and ( block->state->rem_mark == set_mark or block->state->use_mark == set_mark ) )
        return;

    _get_used_marks_rec( used_marks, block->ok, set_mark );
    _get_used_marks_rec( used_marks, block->ko, set_mark );
}

void Language::_get_mark( Block *b ) {
    ++cur_op_id;
    std::set<int> used_marks;
    _get_used_marks_rec( used_marks, b, b->state );

    for( int i = 0; ; ++i ) {
        if ( used_marks.count( i ) == 0 ) {
            if ( nb_marks <= i )
                nb_marks = i + 1;
            marks[ b->state ] = i;
            break;
        }
    }
}

void Language::_write_dot_rec( std::ostream &os, Block *block ) {
    if ( block->op_id == cur_op_id )
        return;
    block->op_id = cur_op_id;

    os << "    node_" << block << " [label=\"";
    if ( block->state )
        block->state->write_label( os, 10 );
    if ( block->ko )
       os << block->cond;
    os << "\"]\n";

    if ( block->ok ) {
        os << "    node_" << block << " -> node_" << block->ok << ";";
        _write_dot_rec( os, block->ok );
    }
    if ( block->ko ) {
        os << "    node_" << block << " -> node_" << block->ko << " [color=\"red\"];";
        _write_dot_rec( os, block->ko );
    }
}

int Language::_display_dot( Block *block, const char *f ) {
    std::ofstream os( f );
    os << "digraph LexemMaker {\n";
    ++cur_op_id;
    _write_dot_rec( os, block );
    os << "}\n";
    os.close();

    return exec_dot( f );
}

void Language::_assign_label( Block *block ) {
    if ( block->label < 0 )
        block->label = nb_labels++;
}
