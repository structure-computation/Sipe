#include <Sipe/Stream.h>
#include <fstream>
using namespace std;

int usage( const char *prg, const char *msg, int res ) {
    if ( msg )
        cerrn << msg;
    cerrn << "Usage:";
    cerrn << "  " << prg << " output_file input_file cpp_var_name";
    return res;
}

int main( int argc, char **argv ) {
    if ( argc != 4 )
        return usage( argv[ 0 ], "nb args", 1 );
    std::ofstream fo( argv[ 1 ] );
    std::ifstream fi( argv[ 2 ] );
    if ( not fi or not fo )
        return 2;

    fo << "char " << argv[ 3 ] << "[] = {";
    for( int i = 0; ; ++i ) {
        int c = fi.get();
        if ( fi.eof() )
            c = 0;

        if ( i )
            fo << ", ";
        if ( i % 16 == 0 )
            fo << "\n    ";
        fo << c;

        if ( fi.eof() ) {
            if ( i )
                fo << "\n";
            break;
        }
    }
    fo << "};\n";

    return 0;
}
