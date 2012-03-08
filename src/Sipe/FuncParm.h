#ifndef FUNCPARM_H
#define FUNCPARM_H

#include "Stream.h"
#include "Vec.h"

/**
*/
class FuncParm {
public:
    typedef std::pair<String,String> NamedP;

    void set( const String &key, const String &val );
    bool operator==( const FuncParm &fp ) const;
    String operator[]( String key ) const;
    bool has( String key ) const;
    void remove( String key );

    Vec<String> u_params;
    Vec<NamedP> n_params;
};

std::ostream &operator<<( std::ostream &os, const FuncParm &func_parm );

#endif // FUNCPARM_H
