get_urls = 
    ( '/file ' { coutn << "requesting /file"; } )

    #| ( { P(*data); } any )*
    # { beg_url = data + 1; } any** ' ' { coutn << "url='" << String( beg_url, data ) << "'"; }


content_length =
    'Content-Length: '
    uint[ val = 'content_length' ]
    { std::cout << sipe_data->content_length << std::endl; }

post_data =
    (
        ( lf content_length ) |
        #( cr cr @end_post_data ) |
        any
    )**
    #:end_post_data

get  = 'GET ' { std::cout << "-> get\n"; } # get_urls
put  = 'PUT ' { std::cout << "-> put\n"; }
post = 'POST ' { std::cout << "-> post\n"; } post_data
e400 = { std::cout << "ERROR 400: bad request type\n"; }

#p[ msg ] = { std::cout << "msg" << std::endl; }

main =
    _add_prel[ '#define P(A) std::cout << #A << " -> " << A << std::endl;' ]
    _set_strn[ 'Http' ]
    # p[ pouet]
    'A'+ 'B' { P( "YOP" ); }

#    get  [freq=10] |
#    put  [freq= 1] |
#    post [freq= 6] |
#    e400
    
#main = uint[ val="toto" ] { std::cout << sipe_data->toto; }
