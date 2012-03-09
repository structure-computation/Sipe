#include "FuncParm.h"

bool FuncParm::operator==( const FuncParm &fp ) const {
    return u_params == fp.u_params and n_params == fp.n_params;
}

void FuncParm::set( const String &key, const String &val ) {
    for( int i = 0; i < n_params.size(); ++i ) {
        if ( n_params[ i ].first == key ) {
            n_params[ i ].second = val;
        }
    }
    n_params << NamedP( key, val );
}

String FuncParm::operator[]( String key ) const {
    for( int i = 0; i < n_params.size(); ++i )
        if ( n_params[ i ].first == key )
            return n_params[ i ].second;
    return String();
}

bool FuncParm::has( String key ) const {
    for( int i = 0; i < n_params.size(); ++i )
        if ( n_params[ i ].first == key )
            return true;
    return false;
}

void FuncParm::remove( String key ) {
    for( int i = 0; i < n_params.size(); ++i )
        if ( n_params[ i ].first == key )
            n_params.remove( i-- );
}

void FuncParm::clear() {
    u_params.resize( 0 );
    n_params.resize( 0 );
}

std::ostream &operator<<( std::ostream &os, const FuncParm &func_parm ) {
    for( int i = 0; i < func_parm.u_params.size(); ++i )
        os << ( i ? "," : "" ) << func_parm.u_params[ i ];
    for( int i = 0; i < func_parm.n_params.size(); ++i )
        os << ( i + func_parm.u_params.size() ? "," : "" ) << func_parm.n_params[ i ].first << "=" << func_parm.n_params[ i ].second;
    return os;
}
