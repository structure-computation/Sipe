#include "FuncParm.h"

bool FuncParm::operator==( const FuncParm &fp ) const {
    return u_params == fp.u_params and n_params == fp.n_params;
}

String FuncParm::operator[]( String key ) const {
    for( int i = 0; i < n_params.size(); ++i )
        if ( n_params[ i ].first == key )
            return n_params[ i ].second;
    return String();
}

void FuncParm::remove( String key ) {
    for( int i = 0; i < n_params.size(); ++i )
        if ( n_params[ i ].first == key )
            n_params.remove( i-- );
}

std::ostream &operator<<( std::ostream &os, const FuncParm &func_parm ) {
    for( int i = 0; i < func_parm.u_params.size(); ++i )
        os << ( i ? "," : "" ) << func_parm.u_params[ i ];
    for( int i = 0; i < func_parm.n_params.size(); ++i )
        os << ( i + func_parm.u_params.size() ? "," : "" ) << func_parm.n_params[ i ].first << "=" << func_parm.n_params[ i ].second;
    return os;
}
