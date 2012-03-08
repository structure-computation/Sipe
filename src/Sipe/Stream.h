#ifndef STREAM_H
#define STREAM_H

#include "StreamSep.h"
#include <iostream>
#include <string>

#ifdef METIL_COMP_DIRECTIVE
#pragma cpp_flag -Wreorder
#endif // METIL_COMP_DIRECTIVE

extern StreamSepMaker<std::ostream> coutn;
extern StreamSepMaker<std::ostream> cerrn;

#define P( A ) coutn << #A << " -> " << ( A )

typedef std::string String;

#endif // STREAM_H
