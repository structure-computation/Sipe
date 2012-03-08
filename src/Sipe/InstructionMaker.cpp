#include "InstructionMaker.h"
#include "Instruction.h"
#include <stdlib.h>

static String repl_parm( String str, const FuncParm &params ) {
    for( int i = 0; i < params.n_params.size(); ++i ) {
        while ( true ) {
            int p = str.find( params.n_params[ i ].first );
            if ( p < 0 )
                break;
            str.replace( p, params.n_params[ i ].first.size(), params.n_params[ i ].second );
        }
    }
    return str;
}


InstructionMaker::InstructionMaker( LexemMaker &lexem_maker ) : lexem_maker( lexem_maker ) {
}

InstructionMaker::~InstructionMaker() {
    for( int i = 0; i < to_del.size(); ++i )
        del( to_del[ i ] );
}

Instruction *InstructionMaker::make( const char *name ) {
    // find $name
    const Lexem *lex = lexem_maker[ name ];
    if ( not lex ) {
        String msg = "Impossible to find a machine named '" + String( name ) + "'";
        lexem_maker.err( 0, msg.c_str() );
        return 0;
    }

    // graph from $name
    Instruction *res = new Instruction( 0, 0 );
    Instruction *end = app( res, lex, FuncParm(), 1 );

    // ok end
    end = app( end, new Instruction( 0, 1, Instruction::OK ) );

    // ko end
    end = app( res, new Instruction( 0, 1, Instruction::KO ) );

    to_del << res;
    return res;
}

Instruction *InstructionMaker::app( Instruction *src, const Lexem *lex, FuncParm params, double freq ) {
    String f = params["freq"];
    if ( f.size() )
        freq = atof( f.c_str() );
    params.remove( "freq" );

    //
    for( ; lex and not lex->eq( Lexem::OPERATOR, "=" ); lex = lex->next ) {
        if ( lex->type == Lexem::VARIABLE ) {
            if ( lex->beg[ 0 ] == '_' ) {
                // -> internal function call
                src = app( src, new Instruction( lex, freq, Func( lex->beg, lex->end, params ) ) );
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

                            if ( not params.has( key ) ) {
                                if ( num_parm < params.u_params.size() )
                                    params.set( key, params.u_params[ num_parm ] );
                                else
                                    params.set( key, String( c->beg, c->end ) );
                            }
                        } else {
                            String key( c->beg, c->end );
                            if ( nb_with_default_values ) {
                                lexem_maker.err( c, "Params without default value cannot exist after params with ones." );
                            } else if ( num_parm < params.u_params.size() ) {
                                if ( params.has( key ) )
                                    lexem_maker.err( lex, "Param is already defined." );
                                else
                                    params.set( key, params.u_params[ num_parm ] );
                            } else if ( not params.has( key ) ) {
                                lexem_maker.err( lex, "Not enough parameters." );
                            }
                        }

                        ++num_parm;
                    }

                }
                src = app( src, nex, params, freq );
            } else
                lexem_maker.err( lex, "Machine not found." );
        } else if ( lex->type == Lexem::STRING ) {
            for( const char *s = lex->beg; s != lex->end; ++s ) {
                src = app( src, new Instruction( lex, freq, 1 ) );
                src = app( src, new Instruction( lex, freq, Cond( *s ) ) );
            }
        } else if ( lex->type == Lexem::NUMBER ) {
            src = app( src, new Instruction( lex, freq, 1 ) );
            src = app( src, new Instruction( lex, freq, Cond( lex->to_int() ) ) );
        } else if ( lex->type == Lexem::CODE ) {
            src = app( src, new Instruction( lex, freq, repl_parm( String( lex->beg, lex->end ), params ) ) );
        } else if ( lex->type == Lexem::OPERATOR ) {
            if ( lex->eq( "|" ) ) {
                Instruction *beg = src;
                src = new Instruction( lex, freq );
                for( int i = 0; i < 2; ++i )
                    app( app( beg, lex->children[ i ], params, freq ), src );
            } else if ( lex->eq( "**" ) ) { // priority to exit from the loop
                src = app( src, new Instruction( lex, freq ) ); // because we want to loop at the beginning of this (not to the previous instruction)
                app( app( src, lex->children[ 0 ], params, freq ), src );
            } else if ( lex->eq( "*" ) ) { // priority to stay inside the loop
                Instruction *comm = app( src, new Instruction( lex, freq ) ); // because we want to loop at he beginning of this (not to the previous instruction)
                src = app( comm, new Instruction( lex, freq ) ); // first branch -> priority to exit from the loop
                app( app( comm, lex->children[ 0 ], params, freq ), comm );
            } else if ( lex->eq( "?" ) ) { // zero or one
                src = app( src, app( app( src, lex->children[ 0 ], params, freq ), new Instruction( lex, freq ) ) );
            } else if ( lex->eq( "+" ) ) { // one or more (priority to exit)
                src = app( src, lex->children[ 0 ], params, freq );

                Instruction *comm = app( src, new Instruction( lex, freq ) ); // because we want to loop at he beginning of this (not to the previous instruction)
                src = app( comm, new Instruction( lex, freq ) ); // first branch -> priority to exit from the loop
                app( app( comm, lex->children[ 0 ], params, freq ), comm );
            } else if ( lex->eq( "++" ) ) { // one or more (with priority to stay inside)
                src = app( src, lex->children[ 0 ], params, freq );

                src = app( src, new Instruction( lex, freq ) ); // because we want to loop at the beginning of this (not to the previous instruction)
                app( app( src, lex->children[ 0 ], params, freq ), src );
            } else if ( lex->eq( ".." ) ) {
                Instruction *t = new Instruction( lex, freq );
                Instruction *a = app( t, lex->children[ 0 ], params, freq );
                Instruction *b = app( t, lex->children[ 1 ], params, freq );
                int na = a->ascii_val();
                int nb = b->ascii_val();
                if ( na < 0 or nb < 0 )
                    lexem_maker.err( lex, ".. must be between two strings with only one char" );
                src = app( src, new Instruction( lex, freq, 1 ) );
                src = app( src, new Instruction( lex, freq, Cond( na, nb ) ) );
                del( t );
            } else if ( lex->eq( "(" ) ) {
                src = app( src, lex->children[ 0 ], params, freq );
            } else if ( lex->eq( "[" ) ) {
                FuncParm n_params;
                for( const Lexem *c = lex->children[ 1 ]; c; c = c->next ) {
                    if ( c->eq( Lexem::OPERATOR, "," ) )
                        continue;

                    if ( c->eq( Lexem::OPERATOR, "=" ) and c->next ) { // named param ?
                        String name( c->children[ 0 ]->beg, c->children[ 0 ]->end );
                        c = c->next;
                        n_params.n_params << FuncParm::NamedP( name, repl_parm( String( c->beg, c->end ), params ) );
                    } else
                        n_params.u_params << repl_parm( String( c->beg, c->end ), params );
                }
                src = app( src, lex->children[ 0 ], n_params, freq );
            }
        }
    }

    return src;
}

Instruction *InstructionMaker::app( Instruction *inst, Instruction *next ) {
    if ( next ) {
        inst->next << next;
        next->prev << inst;
        return next;
    }
    return inst;
}
