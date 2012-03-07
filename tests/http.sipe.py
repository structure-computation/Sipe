
get_urls = 
    ( '/file ' { coutn << "requesting /file"; } )

    #| ( { P(*data); } any )*
    # { beg_url = data + 1; } any** ' ' { coutn << "url='" << String( beg_url, data ) << "'"; }


content_length = 'Content-Length: ' uint[ val = 'content_length' ] { std::cout << sipe_data->content_length << std::endl; }

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

p_data =
    (
        ( 'C' uint[ val = 'content_length' ] {P(sipe_data->content_length);} ) |
        # {P("C");}
        #( new_line new_line @end_post_data ) |
        any
    )**
    #:end_post_data

pot = 'P' p_data

main =
    _add_prel[ '#define P(A) std::cout << #A << " -> " << A << std::endl;' ]
    #'C' uint[ val = 'content_length' ] {P(sipe_data->content_length);}
    pot

    #_set_strn[ 'Http' ]
    #get  [freq=10] |
    #put  [freq= 1] |
    #post [freq= 6] |
    #e400
    #get | post | put | e400
    
#main = uint[ val="toto" ] { std::cout << sipe_data->toto; }

