#ifndef STATECLONER_H
#define STATECLONER_H

#include "Scp.h"
#include <map>

/**
*/
class StateCloner {
public:
    StateCloner( Vec<State *> &to_del, Vec<State *> &use_mark_stack );

    State *make( State *src, State *dst );

protected:
    State *new_State( Scp &p );
    State *make( Scp &p, const char *msg );

    typedef std::map<String,State *> TC;
    Vec<State *> &use_mark_stack;
    Vec<State *> &to_del;
    TC created;
};

#endif // STATECLONER_H
