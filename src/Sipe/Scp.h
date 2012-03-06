#ifndef SCP_H
#define SCP_H

#include "State.h"

/**
  StateCloner params
*/
class Scp {
public:
    Scp( State *state, State *dst );

    String bid() const;

    State *state;
    State *dst;
    State *set_mark;
    State *use_mark;
    bool   display_steps;
    String display_prefix;
};

#endif // SCP_H
