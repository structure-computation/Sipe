// must be sorted by length (descending order)
OPERATOR( "..", need_barg, 5 );
OPERATOR( "**", need_larg, 4 );
OPERATOR( "*" , need_larg, 4 );
OPERATOR( "=" , need_larg, 0 );
OPERATOR( "," , need_none, 1 );
OPERATOR( ":" , need_barg, 2 );
OPERATOR( "|" , need_barg, 3 );
OPERATOR( "[" , need_larg, 6 );
