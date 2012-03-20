#include "InstructionMaker.h"
#include "Instruction.h"
#include <string.h>
#include <stdlib.h>

static String repl_parm( String str, const FuncParm &params ) {
    for( int i = 0; i < params.n_params.size(); ++i ) {
        if ( params.n_params[ i ].first == params.n_params[ i ].second )
            continue;

        while ( true ) {
            int p = str.find( params.n_params[ i ].first );
            if ( p < 0 )
                break;
            str.replace( p, params.n_params[ i ].first.size(), params.n_params[ i ].second );
        }
    }
    return str;
}


InstructionMaker::InstructionMaker( CodeParm &code_parm, LexemMaker &lexem_maker ) : lexem_maker( lexem_maker ), code_parm( code_parm ) {
}

InstructionMaker::~InstructionMaker() {
    for( int i = 0; i < to_del.size(); ++i )
        del( to_del[ i ] );
}

Instruction *InstructionMaker::make( const char *name ) {
    Source sm( "internal", "main", 0, false);
    Lexem main( Lexem::VARIABLE, &sm, name, name + strlen( name ) );

    // graph from $name
    Par par; par.freq = 1;
    Instruction *res = new Instruction( 0, 1 );
    Instruction *end = app( res, &main, par );

    // ok end
    end = app( end, new Instruction( 0, 1, Instruction::OK ) );

    // ko end
    end = app( res, new Instruction( 0, 1, Instruction::KO ) );

    to_del << res;
    return res;
}

void InstructionMaker::_get_labels_rec( std::map<String,Instruction *> &labels, const Lexem *lex ) {
    if ( lex->eq( Lexem::OPERATOR, "=" ) )
        return;

    if ( lex->eq( Lexem::OPERATOR, "<-" ) ) {
        String label( lex->children[ 0 ]->beg, lex->children[ 0 ]->end );
        if ( labels.count( label ) )
            lexem_maker.err( lex->children[ 0 ], "This label has already been defined." );
        else
            labels[ label ] = new Instruction( lex, 1 );
    }
    if ( lex->children[ 0 ] ) _get_labels_rec( labels, lex->children[ 0 ] );
    if ( lex->children[ 1 ] ) _get_labels_rec( labels, lex->children[ 1 ] );
    if ( lex->next          ) _get_labels_rec( labels, lex->next          );
}

Instruction *InstructionMaker::app( Instruction *src, const Lexem *lex, Par par ) {
    String f = par.params[ "freq" ];
    if ( f.size() ) {
        par.freq = atof( f.c_str() );
    }
    par.params.remove( "freq" );

    //
    for( ; lex and not lex->eq( Lexem::OPERATOR, "=" ); lex = lex->next ) {
        if ( lex->type == Lexem::VARIABLE ) {
            if ( lex->eq( "add_attr" ) ) {
                CodeParm::Attr a;
                a.decl = 0 < par.params.u_params.size() ? par.params.u_params[ 0 ] : "";
                a.init = 1 < par.params.u_params.size() ? par.params.u_params[ 1 ] : "";
                a.dest = 2 < par.params.u_params.size() ? par.params.u_params[ 2 ] : "";
                for( int i = 0; ; ++i ) {
                    if ( i == code_parm.attributes.size() ) {
                        code_parm.attributes << a;
                        break;
                    }
                    if ( code_parm.attributes[ i ].decl == a.decl )
                        break;
                }
            } else if ( lex->eq( "add_prel" ) ) {
                for( int j = 0; j < par.params.u_params.size(); ++j ) {
                    String prel = par.params.u_params[ j ];
                    if ( not code_parm.preliminaries.contains( prel ) )
                        code_parm.preliminaries << prel;
                }
            } else if ( lex->eq( "str_name" ) ) {
                if ( par.params.u_params.size() == 1 )
                    code_parm.struct_name = par.params.u_params[ 0 ];
                else
                    cerrn << "str_name must have exactly one argument";
            } else if ( const Lexem *nex = lexem_maker( lex->beg, lex->end ) ) {
                // -> machine call
                if ( nex->prev and nex->prev->eq( Lexem::OPERATOR, "=" ) and nex->prev->children[ 0 ]->eq( Lexem::OPERATOR, "[" ) ) {
                    // -> with arguments
                    int num_parm = 0, nb_with_default_values = 0;
                    for( const Lexem *c = nex->prev->children[ 0 ]->children[ 1 ]; c; c = c->next ) {
                        if ( c->eq( Lexem::OPERATOR, "," ) )
                            continue;

                         // with a default value ?
                        if ( c->eq( Lexem::OPERATOR, "=" ) and c->next ) {
                            String key( c->children[ 0 ]->beg, c->children[ 0 ]->end );
                            ++nb_with_default_values;
                            c = c->next;

                            if ( not par.params.has( key ) ) {
                                if ( num_parm < par.params.u_params.size() )
                                    par.params.set( key, par.params.u_params[ num_parm ] );
                                else
                                    par.params.set( key, String( c->beg, c->end ) );
                            }
                        } else {
                            String key( c->beg, c->end );
                            if ( nb_with_default_values ) {
                                lexem_maker.err( c, "Params without default value cannot exist after params with ones." );
                            } else if ( num_parm < par.params.u_params.size() ) {
                                if ( par.params.has( key ) )
                                    lexem_maker.err( lex, "Param is already defined." );
                                else
                                    par.params.set( key, par.params.u_params[ num_parm ] );
                            } else if ( not par.params.has( key ) ) {
                                lexem_maker.err( lex, "Not enough parameters." );
                            }
                        }

                        ++num_parm;
                    }

                }


                Par n_par = par;
                std::map<String,Instruction *> labels;
                n_par.labels = &labels;
                _get_labels_rec( labels, nex );

                src = app( src, nex, n_par );
            } else
                lexem_maker.err( lex, "Machine not found." );
        } else if ( lex->type == Lexem::STRING ) {
            for( const char *s = lex->beg; s != lex->end; ++s ) {
                src = app( src, new Instruction( lex, par.freq, 1 ) );
                src = app( src, new Instruction( lex, par.freq, Cond( *s ) ) );
            }
        } else if ( lex->type == Lexem::NUMBER ) {
            src = app( src, new Instruction( lex, par.freq, 1 ) );
            src = app( src, new Instruction( lex, par.freq, Cond( lex->to_int() ) ) );
        } else if ( lex->type == Lexem::CODE ) {
            src = app( src, new Instruction( lex, par.freq, repl_parm( String( lex->beg, lex->end ), par.params ) ) );
        } else if ( lex->type == Lexem::OPERATOR ) {
            if ( lex->eq( "|" ) ) {
                Instruction *beg = src;
                src = new Instruction( lex, par.freq );
                for( int i = 0; i < 2; ++i )
                    app( app( beg, lex->children[ i ], par ), src );
            } else if ( lex->eq( "**" ) ) { // priority to exit from the loop
                src = app( src, new Instruction( lex, par.freq ) ); // because we want to loop at the beginning of this (not to the previous instruction)
                app( app( src, lex->children[ 0 ], par ), src );
            } else if ( lex->eq( "*" ) ) { // priority to stay inside the loop
                Instruction *comm = app( src, new Instruction( lex, par.freq ) ); // because we want to loop at he beginning of this (not to the previous instruction)
                src = app( comm, new Instruction( lex, par.freq ) ); // first branch -> priority to exit from the loop
                app( app( comm, lex->children[ 0 ], par ), comm );
            } else if ( lex->eq( "??" ) ) { // zero or one. prefer to go into
                src = app( src, app( app( src, lex->children[ 0 ], par ), new Instruction( lex, par.freq ) ) );
            } else if ( lex->eq( "?" ) ) { // zero or one. prefer to avoid
                Instruction *old = src;
                src = app( src, new Instruction( lex, par.freq ) );
                app( app( old, lex->children[ 0 ], par ), src );
            } else if ( lex->eq( "+" ) ) { // one or more (priority to exit)
                src = app( src, lex->children[ 0 ], par );

                Instruction *comm = app( src, new Instruction( lex, par.freq ) ); // because we want to loop at he beginning of this (not to the previous instruction)
                src = app( comm, new Instruction( lex, par.freq ) ); // first branch -> priority to exit from the loop
                app( app( comm, lex->children[ 0 ], par ), comm );
            } else if ( lex->eq( "++" ) ) { // one or more (with priority to stay inside)
                src = app( src, lex->children[ 0 ], par );

                src = app( src, new Instruction( lex, par.freq ) ); // because we want to loop at the beginning of this (not to the previous instruction)
                app( app( src, lex->children[ 0 ], par ), src );
            } else if ( lex->eq( ".." ) ) {
                Instruction *t = new Instruction( lex, par.freq );
                Instruction *a = app( t, lex->children[ 0 ], par );
                Instruction *b = app( t, lex->children[ 1 ], par );
                int na = a->ascii_val();
                int nb = b->ascii_val();
                if ( na < 0 or nb < 0 )
                    lexem_maker.err( lex, ".. must be between two strings with only one char" );
                src = app( src, new Instruction( lex, par.freq, 1 ) );
                src = app( src, new Instruction( lex, par.freq, Cond( na, nb ) ) );
                del( t );
            } else if ( lex->eq( "->" ) ) { // goto
                String label( lex->children[ 0 ]->beg, lex->children[ 0 ]->end );
                app( src, (*par.labels)[ label ] );
                src = 0;
            } else if ( lex->eq( "<-" ) ) { // label
                String label( lex->children[ 0 ]->beg, lex->children[ 0 ]->end );
                Instruction *inst = (*par.labels)[ label ];
                src = src ? app( src, inst ) : inst;
            } else if ( lex->eq( "(" ) ) {
                src = app( src, lex->children[ 0 ], par );
            } else if ( lex->eq( "[" ) ) {
                Par n_par = par;
                n_par.params.clear();
                for( const Lexem *c = lex->children[ 1 ]; c; c = c->next ) {
                    if ( c->eq( Lexem::OPERATOR, "," ) )
                        continue;

                    if ( c->eq( Lexem::OPERATOR, "=" ) and c->next ) { // named param ?
                        String name( c->children[ 0 ]->beg, c->children[ 0 ]->end );
                        c = c->next;
                        n_par.params.n_params << FuncParm::NamedP( name, repl_parm( String( c->beg, c->end ), par.params ) );
                    } else
                        n_par.params.u_params << repl_parm( String( c->beg, c->end ), par.params );
                }
                src = app( src, lex->children[ 0 ], n_par );
            }
        }
    }

    return src;
}

Instruction *InstructionMaker::app( Instruction *inst, Instruction *next ) {
    if ( not inst )
        return 0;
    if ( inst and next ) {
        inst->next << next;
        next->prev << inst;
        return next;
    }
    return next;
}
