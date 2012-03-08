#ifndef LTATE_H
#define LTATE_H

#include "State.h"

/**
*/
class Ltate {
public:
    Ltate();

    Vec<Ltate *> next;
    const State *s;
    Cond verified;
    int num_cond;
};

#endif // LTATE_H
