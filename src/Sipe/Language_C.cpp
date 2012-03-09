#include "Language_C.h"
#include "StreamSep.h"
#include <sstream>


Language_C::Language_C( bool cpp ) : cpp( cpp ) {
}

Language_C::~Language_C() {
}

void Language_C::write( std::ostream &os, const State *state, bool write_main ) {
    // helpers
    f_suf = cpp ? "" : "_" + struct_name;

    // unfold and get label
    Block *block = _unfold( state );
    // _display_dot( block );
    _make_labels( block );

    // hum...
    if ( write_main ) {
        StreamSepMaker<std::ostream> on( os );
        on << "#ifdef SIPE_MAIN";
        on << "#include <stdio.h>";
        on << "#endif // SIPE_MAIN";
    }

    //
    _write_preliminaries( os );
    _write_declarations( os );
    _write_parse_func( os );
    _write_init_func( os );
    _write_dest_func( os );

    //
    if ( write_main ) {
        StreamSepMaker<std::ostream> on( os );
        on << "#ifdef SIPE_MAIN";
        on << "#include <unistd.h>";
        on << "#include <fcntl.h>";
        // on << "#include <stdio.h>";
        _write_parse_file_func( os );
        _write_main_func( os );
        on << "#endif // SIPE_MAIN";
    }
}

void Language_C::_write_preliminaries( std::ostream &os ) {
    StreamSepMaker<std::ostream> on( os );

    // preliminaries
    for( int i = 0; i < preliminaries.size(); ++i )
        on << preliminaries[ i ];
}

void Language_C::_write_declarations( std::ostream &os ) {
    StreamSepMaker<std::ostream> on( os );

    on << "struct " << struct_name << ";";
    on << "void init" << f_suf << "( " << struct_name << " *sipe_data );";
    on << "void dest" << f_suf << "( " << struct_name << " *sipe_data );";

    // struct
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
}

void Language_C::_write_init_func( std::ostream &os ) {
    StreamSepMaker<std::ostream> on( os );
    // init
    on << "void init" << f_suf << "( " << struct_name << " *sipe_data ) {";
    on << "    sipe_data->_inp_cont = 0;";
    if ( need_a_mark )
        on << "    sipe_data->_mark = 0;";

    for( int i = 0; i < attributes.size(); ++i )
        if ( attributes[ i ].init.size() )
            os << "    " << attributes[ i ].init << ";";
    on << "}";
}

void Language_C::_write_dest_func( std::ostream &os ) {
    StreamSepMaker<std::ostream> on( os );

    // dest
    on << "void dest" << f_suf << "( " << struct_name << " *sipe_data ) {";
    for( int i = 0; i < attributes.size(); ++i )
        if ( attributes[ i ].dest.size() )
            os << "    " << attributes[ i ].dest << ";";
    on << "}";
}

static int nb_digits( int val ) {
    if ( val < 0 ) return 1 + nb_digits( -val );
    if ( val < 10 ) return 1;
    return 1 + nb_digits( val / 10 );
}

void Language_C::_write_parse_func( std::ostream &os ) {
    StreamSepMaker<std::ostream> on( os );
    int nb_spaces = 5;
    String sp( nb_spaces, ' ' );
    on.beg = sp.c_str();

    // parse
    os << "int parse" << f_suf << "( " << struct_name << " *sipe_data, const char *data, const char *end ) {\n";
    on << "#define INCR( N ) if ( ++data >= end ) goto p_##N; c_##N:";
    on << "if ( sipe_data->_inp_cont )";
    on << "    goto *sipe_data->_inp_cont;";
    on << "";

    // blocks
    for( int i = 0; i < block_seq.size(); ++i ) {
        Block *b = block_seq[ i ];
        if ( not b->write )
            continue;

        //
        if ( b->label >= 0 ) {
            os << "l_" << b->label << ":";
            on.first_beg = on.beg + std::min( nb_spaces, 3 + nb_digits( b->label ) );
        }

        //
        if ( b->state ) {
            // action with data ?
            if ( b->state->action_num >= 0 )
                b->state->instructions[ b->state->action_num ]->write_code( on, this );

            // action without data
            if ( b->state->action )
                b->state->action->write_code( on, this );

            // end ?
            if ( b->state->end ) {
                on << "sipe_data->_inp_cont = &&l_" << b->label << ";";
                on << "return " << b->state->end << ";";
            }

            //
            if ( b->state->set_mark ) {
                on << "sipe_data->_mark = data;";
                //on << "    std::cout << \"set mark\" << std::endl;";
            }

            //
            if ( b->state->use_mark ) {
                on << "data = sipe_data->_mark;";
                //on << "    std::cout << \"use mark\" << std::endl;";
            }

            //
            if ( b->state->rem_mark ) {
                //on << "    std::cout << \"rem mark\" << std::endl;";
            }

            //
            if ( b->state->incc ) {
                on << "INCR( " << cnt.size() << " )";

                Cnt c;
                c.mark = b->mark;
                c.label = b->label;
                cnt << c;
            }
        }

        //
        if ( b->ko ) {
            if ( b->t_ok ) { // if ( cond ) goto ok;
                String cond = b->cond.ok_cpp( "*data", &b->not_in );
                on << "if ( " << cond << " ) goto l_" << b->ok->label << ";";
            } else { // if ( not cond ) goto ko;
                String cond = b->cond.ko_cpp( "*data", &b->not_in );
                on << "if ( " << cond << " ) goto l_" << b->ko->label << ";";
            }
        }

        //
        if ( b->write_goto )
            on << "goto l_" << b->write_goto->label << ";";
    }

    // cnt
    for( int i = 0; i < cnt.size(); ++i ) {
        os << "p_" << i << ":";
        on.first_beg = on.beg + std::min( nb_spaces, 3 + nb_digits( i ) );
        on << "sipe_data->_inp_cont = &&c_" << i << "; return 0;";
    }
    os << "}\n";
}

void Language_C::_write_parse_file_func( std::ostream &os ) {
    StreamSepMaker<std::ostream> on( os );

    // function for execution
    on << "int parse_file( int fd ) {";
    on << "    " << struct_name << " sd;";
    if ( not cpp )
        on << "    init( &sd );";

    on << "    char buffer[ 2048 ];";
    on << "    while ( true ) {";
    on << "        int r = read( fd, buffer, 2048 );";
    // on << "        printf( \"r=%i\\n\", r );";
    on << "        if ( r == 0 )";
    on << "            return 0;";
    on << "        if ( int res = parse" << f_suf << "( &sd, buffer, buffer + r ) )";
    on << "            return res;";
    on << "    }";

    if ( not cpp )
        on << "    dest( &sd );";
    on << "    return 0;";
    on << "}";
}

void Language_C::_write_main_func( std::ostream &os ) {
    StreamSepMaker<std::ostream> on( os );
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
