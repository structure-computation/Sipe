#ifndef SOURCE_H
#define SOURCE_H

/**
*/
class Source {
public:
    Source( const char *filename, Source *prev );
    Source( const char *filename, const char *text, Source *prev, bool need_cp = true );
    ~Source();

    const char *provenance;
    const char *data;

    Source *prev;
    char *rese;
};

#endif // SOURCE_H
