#include "Language_C.h"
#include "StreamSep.h"
#include <sstream>


Language_C::Language_C( bool cpp ) : cpp( cpp ) {
}

Language_C::~Language_C() {
}

void Language_C::write( std::ostream &os, const CodeParm &_cp, const State *state, bool write_main ) {
    // helpers
    cp = &_cp;
    StreamSepMaker<std::ostream> on( os );
    f_suf = cpp ? "" : "_" + cp->struct_name;

    // unfold and get label
    Block *block = _unfold( state );
    if ( db )
        _display_dot( block );
    _make_labels( block );

    // hum...
    if ( write_main ) {
        on << "#ifdef SIPE_MAIN";
        on << "#include <unistd.h>";
        on << "#include <stdio.h>";
        on << "#include <fcntl.h>";
        on << "#endif // SIPE_MAIN";
    }
    on << "#ifndef SIPE_CHARP";
    on << "#define SIPE_CHARP const char *";
    on << "#endif // SIPE_CHARP";

    //
    _write_preliminaries( os );

    _write_declarations( os );

    _write_parse_func( os );

    if ( not cpp ) {
        on << "void init" << f_suf << "( " << cp->struct_name << " *sipe_data ) {";
        _write_init_func( os, "    ", "sipe_data->" );
        on << "}";

        on << "void dest" << f_suf << "( " << cp->struct_name << " *sipe_data ) {";
        _write_dest_func( os, "    ", "sipe_data->" );
        on << "}";
    }

    on << "#ifdef SIPE_CLASS";
    on << "int parse( SIPE_CHARP beg, SIPE_CHARP end ) {";
    on << "    parse( &sipe_data, beg, end );";
    on << "}";
    on << "SipeData sipe_data;";
    on << "#endif // SIPE_CLASS";


    //
    if ( write_main ) {
        StreamSepMaker<std::ostream> on( os );
        on << "#ifdef SIPE_MAIN";
        _write_parse_file_func( os );
        _write_main_func( os );
        on << "#endif // SIPE_MAIN";
    }
}

void Language_C::_write_preliminaries( std::ostream &os ) {
    StreamSepMaker<std::ostream> on( os );

    // includes, ...
    for( int i = 0; i < cp->preliminaries.size(); ++i )
        on << cp->preliminaries[ i ];

    //
    if ( not cpp ) {
        on << "struct " << cp->struct_name << ";";
        on << "void init" << f_suf << "( " << cp->struct_name << " *sipe_data );";
        on << "void dest" << f_suf << "( " << cp->struct_name << " *sipe_data );";
    }
}

void Language_C::_write_declarations( std::ostream &os ) {
    StreamSepMaker<std::ostream> on( os );

    // struct
    on << "struct " << cp->struct_name << " {";
    if ( cpp ) {
        on << "    " << cp->struct_name << "() {";
        _write_init_func( os, "        ", "" );
        on << "    }";
        on << "    ~" << cp->struct_name << "() {";
        _write_dest_func( os, "        ", "" );
        on << "    }";
        on << "";
    }
    on << "    void *_inp_cont;";
    if ( nb_marks )
        on << "    SIPE_CHARP _mark[ " << nb_marks << " ];";
    for( int i = 0; i < cp->attributes.size(); ++i )
        on << "    " << cp->attributes[ i ].decl;
    on << "};";
}

void Language_C::_write_init_func( std::ostream &os, const char *sp, const char *sn ) {
    StreamSepMaker<std::ostream> on( os );
    on.beg = sp;

    on << sn << "_inp_cont = 0;";
    //if ( need_a_mark )
    //    on << sn << "_mark = 0;";

    for( int i = 0, a = 0; i < cp->attributes.size(); ++i ) {
        if ( cp->attributes[ i ].init.size() ) {
            if ( cpp and not a++ )
                on << cp->struct_name << " *sipe_data = this;";
            on << cp->attributes[ i ].init << ";";
        }
    }
}

void Language_C::_write_dest_func( std::ostream &os, const char *sp, const char *sn ) {
    StreamSepMaker<std::ostream> on( os );
    on.beg = sp;

    for( int i = 0, a = 0; i < cp->attributes.size(); ++i ) {
        if ( cp->attributes[ i ].dest.size() ) {
            if ( cpp and not a++ )
                on << cp->struct_name << " *sipe_data = this;";
            os << cp->attributes[ i ].dest << ";";
        }
    }
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
    os << "int parse" << f_suf << "( " << cp->struct_name << " *sipe_data, SIPE_CHARP data, SIPE_CHARP end ) {\n";
    on << "if ( sipe_data->_inp_cont )";
    on << "    goto *sipe_data->_inp_cont;";
    on << "";
    on << "#define INCR( N ) if ( ++data >= end ) goto p_##N; c_##N:";
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
            // action
            if ( b->state->action )
                b->state->action->write_code( on, this );

            // end ?
            if ( b->state->end ) {
                on << "sipe_data->_inp_cont = &&l_" << b->label << ";";
                on << "return " << b->state->end << ";";
            }

            //
            if ( b->state->set_mark ) {
                on << "sipe_data->_mark[ " << marks[ b->state ] << " ] = data;";
                //on << "    std::cout << \"set mark\" << std::endl;";
            }

            //
            if ( b->state->use_mark ) {
                on << "data = sipe_data->_mark[ " << marks[ b->state->use_mark ] << " ];";
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
    on << "    " << cp->struct_name << " sd;";
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
