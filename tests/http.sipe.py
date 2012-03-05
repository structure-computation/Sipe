
get_urls = 
    ( '/file' { coutn << "requesting /file"; } )
    #| ( { P(*data); } any )*
    # { beg_url = data + 1; } any** ' ' { coutn << "url='" << String( beg_url, data ) << "'"; }


content_length = new_line 'Content-Length: ' unsigned_int_reader[ val = 'content_length' ]

post_data =
    (
        content_length |
        #( new_line new_line @end_post_data ) |
        any
    )**
    #:end_post_data

get  = 'GET ' #{ P( "get" ); } get_urls ' '
put  = 'PUT ' #{{ P( "put" ); }
post = 'POST ' #{ P( "post" ); } post_data 
e400 = { P( "ERROR 400: bad request type" ); }


main = 
    #_set_strn[ 'Http' ]
    'A' | e400
    #get | post | put | e400
    