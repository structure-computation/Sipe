# main = '"' { beg = data + 1; } any** { std::cout.write( beg, data - beg ); coutn << "<-"; } '"' #
main = { C } 'A'
# any ** { P( "E" ); } '"'
# main = ( { P( "A" ); } 'A' ) | ( { P( "B" ); } 'B' )
