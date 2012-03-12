#ifndef CODEPARM_H
#define CODEPARM_H

#include "Stream.h"
#include "Vec.h"

/**
  Parameters for code creation
*/
class CodeParm {
public:
    CodeParm();

    struct Attr {
        String decl;
        String init;
        String dest;
    };

    Vec<String> preliminaries;
    Vec<Attr> attributes;
    String struct_name;
};

#endif // CODEPARM_H
