#ifndef LANGUAGE_C_H
#define LANGUAGE_C_H

#include "Language.h"
#include <map>

/**
*/
class Language_C : public Language {
public:
    Language_C( bool cpp );
    virtual ~Language_C();

    virtual void write( std::ostream &os, const CodeParm &cp, const State *state, bool write_main = false );

    const char *buffer_length;
protected:
    void _write_init_func( std::ostream &os, const char *sp, const char *sn );
    void _write_dest_func( std::ostream &os, const char *sp, const char *sn );
    void _write_parse_file_func( std::ostream &os );
    void _write_preliminaries( std::ostream &os );
    void _write_declarations( std::ostream &os );
    void _write_parse_func( std::ostream &os );
    void _write_main_func( std::ostream &os );
    void _write_code( std::ostream &os, const State *mark );

    struct Cnt {
        Vec<State *> active_marks;
        Block *block;
        int label;
    };

    const CodeParm *cp;
    Vec<Cnt> cnt;
    String f_suf;
    bool cpp;
};

#endif // LANGUAGE_C_H
