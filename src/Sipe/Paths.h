#ifndef PATHS_H
#define PATHS_H

#include "Vec.h"


/**
*/
template<class T>
struct Paths {
    struct Item {
        Item() : op_id( 0 ) {}
        void get_children_rec( int cur_op_id, Vec<const Item *> &res ) const {
            if ( op_id == cur_op_id )
                return;
            op_id = cur_op_id;

            res << this;
            for( int i = 0; i < next.size(); ++i )
                next[ i ]->get_children_rec( cur_op_id, res );
        }

        T           data;
        Vec<Item *> prev;
        Vec<Item *> next;
        mutable Item *op_mp;
        mutable int op_id;
    };

    Paths() {
        cur_op_id = 0;
    }

    Paths( const Paths &p ) {
        cur_op_id = 0;

        Vec<const Item *> ch = p.get_children();
        for( int i = 0; i < ch.size(); ++i ) {
            ch[ i ]->op_mp = new Item;
            ch[ i ]->op_mp->data = ch[ i ]->data;
        }

        for( int i = 0; i < ch.size(); ++i ) {
            for( int j = 0; j < ch[ i ]->next.size(); ++j )
                ch[ i ]->op_mp->next << ch[ i ]->next[ j ]->op_mp;
            for( int j = 0; j < ch[ i ]->prev.size(); ++j )
                ch[ i ]->op_mp->prev << ch[ i ]->prev[ j ]->op_mp;
        }

        for( int i = 0; i < p.begs.size(); ++i )
            begs << p.begs[ i ]->op_mp;
        for( int i = 0; i < p.ends.size(); ++i )
            ends << p.ends[ i ]->op_mp;
    }

    Vec<const Item *> get_children() const {
        Vec<const Item *> res;
        ++cur_op_id;
        for( int i = 0; i < begs.size(); ++i )
            begs[ i ]->get_children_rec( cur_op_id, res );
        return res;
    }

    void init( T data ) {
        Item *res = new Item;
        res->data = data;
        to_del << res;
        begs << res;
        ends << res;
    }

    void next( int index, const Vec<T> &n ) {
        Item *e = ends[ index ];
        ends.remove( index );
        for( int i = 0; i < n.size(); ++i ) {
            Item *r = new Item;
            r->prev << e;
            e->next << r;
            r->data = n[ i ];
            ends.insert( index + i, r );
        }
    }

    void remove( int index ) {
        ends.remove( index );
    }

    Vec<Item *> to_del;
    Vec<Item *> begs;
    Vec<Item *> ends;
    mutable int cur_op_id;
};


#endif // PATHS_H
