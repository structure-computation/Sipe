#include "Language_C.h"
#include "StreamSep.h"
#include <algorithm>
#include <sstream>

Language_C::Language_C( bool cpp ) : cpp( cpp ) {
    struct_name = "SipeData";
    need_a_mark = false;
}

Language_C::~Language_C() {
}

void Language_C::write( std::ostream &os, const State *state, bool write_main ) {
    std::ostringstream code;
    write_code( code, state );

    // helpers
    StreamSepMaker<std::ostream> on( os );
    String f_suf = cpp ? "" : "_" + struct_name;

    //
    if ( write_main ) {
        on << "#include <fcntl.h>";
        on << "#include <stdio.h>";
        if ( cpp )
            on << "#include <iostream>";
    }

    // declarations
    on << "struct " << struct_name << ";";
    on << "void init" << f_suf << "( " << struct_name << " *sipe_data );";
    on << "void dest" << f_suf << "( " << struct_name << " *sipe_data );";

    // struct declaration
    on << "struct " << struct_name << " {";
    if ( cpp ) {
        on << "    " << struct_name << "() { init" << f_suf << "( this ); }";
        on << "    ~" << struct_name << "() { dest" << f_suf << "( this ); }";
        on << "";
    }
    on << "    void *_inp_cont;";
    if ( need_a_mark )
        on << "    const char *_mark;";
    for( int i = 0; i < attributes.size(); ++i )
        on << "    " << attributes[ i ].decl;
    on << "};";


    // init
    on << "void init" << f_suf << "( " << struct_name << " *sipe_data ) {";
    on << "    sipe_data->_inp_cont = 0;";
    if ( need_a_mark )
        on << "    sipe_data->_mark = 0;";
    for( int i = 0; i < attributes.size(); ++i )
        if ( attributes[ i ].init.size() )
            os << "    " << attributes[ i ].init << ";";
    on << "}";

    // dest
    on << "void dest" << f_suf << "( " << struct_name << " *sipe_data ) {";
    for( int i = 0; i < attributes.size(); ++i )
        if ( attributes[ i ].dest.size() )
            os << "    " << attributes[ i ].dest << ";";
    on << "}";

    // parse
    on << "int parse" << f_suf << "( " << struct_name << " *sipe_data, const char *data, const char *end ) {";
    on << "    if ( sipe_data->_inp_cont )";
    on << "        goto *sipe_data->_inp_cont;";
    os << code.str();
    on << "    return 0;";
    on << "}";

    if ( write_main ) {
        on << "int parse_file( int fd ) {";
        on << "    " << struct_name << " sd;";
        if ( not cpp )
            on << "    init( &sd );";

        on << "    char buffer[ 2048 ];";
        on << "    while ( true ) {";
        on << "        int r = read( fd, buffer, 2048 );";
        on << "        if ( r == 0 )";
        on << "            break;";
        // on << "        std::cout.write( buffer, r );";
        on << "        if ( parse" << f_suf << "( &sd, buffer, buffer + r ) == 0 )";
        on << "            break;";
        on << "    }";

        if ( not cpp )
            on << "    dest( &sd );";
        on << "    return 0;";
        on << "}";
        on << "int main( int argc, char **argv ) {";
        on << "    if ( argc == 1 )";
        on << "        return parse_file( 0 );";
        on << "    for( int i = 1; i < argc; ++i ) {";
        on << "        int fd = open( argv[ i ], O_RDONLY );";
        on << "        if ( fd < 0 )";
        on << "            perror( \"Opening file\" );";
        on << "        else";
        on << "            parse_file( fd );";
        on << "    }";
        on << "}";
    }
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

struct SortNext {
    bool operator()( const State::Next &a, const State::Next &b ) const {
        return a.freq > b.freq;
    }
};

void Language_C::write_code_rec( StreamSepMaker<std::ostream> &os, const State *state, const State *mark ) {
    written[ state ] = 1;

    //
    read_state( state );

    //
    int label = labels[ state ];
    if ( label >= 0 )
        os << "l_" << labels[ state ] << ":";

    // action with data ?
    if ( state->action_num >= 0 )
        state->instructions[ state->action_num ]->write_code( *os.stream, this );

    // action without data
    if ( state->action ) {
        state->action->write_code( *os.stream, this );
    }

    // end ?
    if ( state->end ) {
        os << "    sipe_data->_inp_cont = &&l_" << label << ";";
        os << "    return " << state->end << ";";
    }

    //
    if ( state->incc ) {
        os << "    if ( ++data >= end ) goto p_" << cnt.size() << ";";
        os << "c_" << cnt.size() << ":";

        Cnt c;
        c.mark = mark;
        c.label = label;
        cnt << c;
    }

    // next
    if ( state->next.size() ) {
        Vec<State::Next> next = state->next;
        std::sort( next.beg(), next.end(), SortNext() );

        Cond not_in; not_in << Cond::eof;
        for( int i = 0, o = 0; i < next.size(); ++i ) {
            if ( i + 1 < next.size() ) {
                if ( i )
                    os << "l_" << o << ":";

                o = i + 2 < next.size() ? nb_labels++ : labels[ next[ i + 1 ].s ];
                String cond = next[ i ].cond.ko_cpp( "*data", &not_in );
                os << "    if ( " << cond << " ) goto l_" << o << ";";
            }
            if ( written.count( next[ i ].s ) == 0 )
                write_code_rec( os, next[ i ].s, mark );
            else if ( i == 0 )
                os << "    goto l_" << labels[ next[ i ].s ] << ";";
            not_in |= next[ i ].cond;
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
    if ( state->end )
        will_need_a_label = true;

    // set label
    labels[ state ] = will_need_a_label ? nb_labels++ : -1;

    Vec<State::Next> next = state->next;
    std::sort( next.beg(), next.end(), SortNext() );

    for( int i = 0; i < next.size(); ++i )
        make_labels_rec( next[ i ].s, next.size() >= 2 and i + 1 == next.size() );
}
