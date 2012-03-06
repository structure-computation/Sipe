
get_urls = 
    ( '/file ' { coutn << "requesting /file"; } )

    #| ( { P(*data); } any )*
    # { beg_url = data + 1; } any** ' ' { coutn << "url='" << String( beg_url, data ) << "'"; }


content_length = 'Content-Length: ' uint[ val = 'content_length' ] { std::cout << sipe_data->content_length; }

post_data =
    (
        ( nl content_length ) |
        #( new_line new_line @end_post_data ) |
        any
    )**
    #:end_post_data

get  = 'GET ' { std::cout << "-> get\n"; } # get_urls
put  = 'PUT ' { std::cout << "-> put\n"; } #{{ P( "put" ); }
post = 'POST ' { std::cout << "-> post\n"; } post_data
e400 = { std::cout << "ERROR 400: bad request type\n"; }


main =
    #_set_strn[ 'Http' ]
    get  [freq=10] |
    put  [freq= 1] |
    post [freq= 6] |
    e400
    #get | post | put | e400
    
#main = uint[ val="toto" ] { std::cout << sipe_data->toto; }

