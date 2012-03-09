#ifndef VEC_H
#define VEC_H

#include <vector>

/**
*/
template<class T>
class Vec {
public:
    Vec() {}

    template<class F>
    Vec &operator=( const Vec<F> &vec ) {
        data.resize( 0 );
        for( int i = 0; i < vec.size(); ++i )
            data.push_back( vec[ i ] );
        return *this;
    }

    int size() const { return data.size(); }

    T operator[]( int i ) const { return data[ i ]; }
    T &operator[]( int i ) { return data[ i ]; }

    const T &back() const { return data[ size() - 1 ]; }
    T &back() { return data[ size() - 1 ]; }

    const T *beg() const { return data.data(); }
    T *beg() { return data.data(); }

    const T *end() const { return data.data() + data.size(); }
    T *end() { return data.data() + data.size(); }

    T pop() { T res = back(); data.resize( size() - 1 ); return res; }

    Vec &operator<<( const T &val ) { data.push_back( val ); return *this; }

    template<class F>
    Vec &operator<<( const Vec<F> &val ) { data.insert( data.end(), val.data.begin(), val.data.end() ); return *this; }

    void resize( int s ) { data.resize( s ); }
    void reserve( int s ) { data.reserve( s ); }

    bool erase( const T &val ) {
        for( int i = 0; i < size(); ++i ) {
            if ( data[ i ] == val ) {
                remove( i );
                return true;
            }
        }
        return false;
    }

    void insert( int index, const T &val ) {
        data.insert( data.begin() + index, 1, val );
    }

    bool contains( const T &val ) const {
        for( int i = 0; i < size(); ++i )
            if ( data[ i ] == val )
                return true;
        return false;
    }

    bool any() const {
        for( int i = 0; i < size(); ++i )
            if ( data[ i ] )
                return true;
        return false;
    }

    void remove( int i ) {
        data.erase( data.begin() + i );
    }

    template<class F>
    void replace( unsigned ind, const Vec<F> &vec ) { ///< replace index i by items of vec
        std::vector<T> old = data;
        resize( 0 );
        for( unsigned i = 0; i < old.size(); ++i )
            if ( i == ind )
                operator<<( vec );
            else
                operator<<( old[ i ] );
    }

    void replace( const T &a, const T &b ) { ///< replace index i by items of vec
        for( int i = 0; i < size(); ++i )
            if ( data[ i ] == a )
                data[ i ] = b;
    }

    bool operator==( const Vec &vec ) const {
        if ( vec.size() != size() )
            return false;
        for( int i = 0; i < size(); ++i )
            if ( vec[ i ] != data[ i ] )
                return false;
        return true;
    }

    std::vector<T> data;
};

template<class T>
std::ostream &operator<<( std::ostream &os, const Vec<T> &vec ) {
    for( int i = 0; i < vec.size(); ++i )
        os << ( i ? " " : "" ) << vec[ i ];
    return os;
}

template<class T>
std::ostream &operator<<( std::ostream &os, const Vec<T *> &vec ) {
    for( int i = 0; i < vec.size(); ++i )
        os << ( i ? " " : "" ) << *vec[ i ];
    return os;
}

#endif // VEC_H
