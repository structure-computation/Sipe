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

    virtual void write( std::ostream &os, const State *state, bool write_main = false );

protected:
    void write_code( std::ostream &os, const State *state );
    void write_code_rec( StreamSepMaker<std::ostream> &os, const State *state, const State *mark );
    void make_labels_rec( const State *state, bool will_need_a_label );

    struct Cnt {
        const State *mark;
        int label;
    };
    typedef std::map<const State *,int> MS;
    String struct_name;
    bool need_a_mark;
    int nb_labels;
    Vec<Cnt> cnt;
    MS written;
    MS labels;
    bool cpp;
};

#endif // LANGUAGE_C_H
