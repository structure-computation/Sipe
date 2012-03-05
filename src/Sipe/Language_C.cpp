#include "Language_C.h"
#include "StreamSep.h"
#include <sstream>

Language_C::Language_C( bool cpp ) : cpp( cpp ) {
}

Language_C::~Language_C() {
}

void Language_C::write( std::ostream &os, const State *state ) {
    std::ostringstream code;
    write_code( code, state );

    os << code.str();
}

void Language_C::write_code( std::ostream &code, const State *state ) {
    //
    nb_labels = 0;
    make_labels_rec( state, false );

    // preparation of main code
    StreamSepMaker<std::ostream> cn( code );
    write_code_rec( cn, state, 0 );

    // cnt
    for( int i = 0; i < cnt.size(); ++i ) {
        cn << "p_" << i << ": sipe_data->_inp_cont = &&c_" << i << "; return 0;";
    }
}

void Language_C::write_code_rec( StreamSepMaker<std::ostream> &os, const State *state, const State *mark ) {
    MS::iterator iter = written.find( state );
    if ( iter != written.end() ) {
        os << "    goto l_" << labels[ state ] << ";";
        return;
    }
    written[ state ] = 1;

    //
    int label = labels[ state ];
    if ( label >= 0 )
        os << "l_" << labels[ state ] << ":";

    //
    if ( state->action_num >= 0 ) {
        const Instruction *inst = state->instructions[ state->action_num ];
        inst->write_code( *os.stream, this );
        if ( int type = inst->is_an_end() ) {
            os << "    sipe_data->_inp_cont = &&l_" << label << ";";
            os << "    return " << type << ";";
        }
    }

    //
    if ( state->incc ) {
        os << "    if ( ++data >= end ) goto p_" << cnt.size();
        os << "c_" << cnt.size();

        Cnt c;
        c.mark = mark;
        c.label = label;
        cnt << c;
    }

    // next
    if ( state->next.size() ) {
        Vec<State::Next> next = state->next;

        Cond not_in; not_in << Cond::eof;
        for( int i = 0, o = 0; i < next.size(); ++i ) {
            if ( i + 1 < next.size() ) {
                if ( i )
                    os << "l_" << o << ":";

                o = i + 2 < next.size() ? nb_labels++ : labels[ next[ i + 1 ].s ];
                String cond = state->next[ i ].cond.ko_cpp( "*data", &not_in );
                os << "    if ( " << cond << " ) goto l_" << o << ";";
            }
            write_code_rec( os, state->next[ i ].s, mark );
            not_in |= state->next[ i ].cond;
        }
    }
}


void Language_C::make_labels_rec( const State *state, bool will_need_a_label ) {
    MS::iterator iter = labels.find( state );
    if ( iter != labels.end() ) {
        if ( iter->second < 0 )
            iter->second = nb_labels++;
        return;
    }

    // is an end ?
    if ( state->action_num >= 0 and state->instructions[ state->action_num ]->is_an_end() )
        will_need_a_label = true;

    // set label
    labels[ state ] = will_need_a_label ? nb_labels++ : -1;

    Vec<State::Next> next = state->next;

    for( int i = 0; i < next.size(); ++i )
        make_labels_rec( next[ i ].s, next.size() >= 2 and i + 1 == next.size() );
}
