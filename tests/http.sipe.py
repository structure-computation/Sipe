p[ msg ] =
    _add_prel[ '#include <iostream>' ]
    { std::cout << "msg" << std::endl; }

i[ val ] =
    _add_prel[ '#include <iostream>' ]
    { std::cout << "val -> " << val << std::endl; }

d[ val = "*data" ] =
    _add_prel[ '#include <iostream>' ]
    { std::cout << val << std::endl; }

get_urls =
    ( '/file ' { coutn << "requesting /file"; } )

    #| ( { P(*data); } any )*
    # { beg_url = data + 1; } any** ' ' { coutn << "url='" << String( beg_url, data ) << "'"; }


content_length =
    eol 'Content-Length: '
    uint[ val = 'content_length' ]
    d[ 'sipe_data->content_length' ]

post_data =
    (
        content_length |
        #( eol eol @end_post_data ) |
        any
    )**
    # end_post_data:

get  = 'GET '  p[ get ] # get_urls
put  = 'PUT '  p[ put ]
post = 'POST ' p[ post ] post_data
e400 = p['ERROR 400: bad request type']

main =
    _add_prel[ '#define P(A) std::cout << #A << " -> " << A << std::endl;' ]
    _set_strn[ 'Http' ]

    (
      ( 'A' d ) |
      any
    )**

    #get  [freq=10] |
    #put  [freq= 1] |
    #post [freq= 6] #|
    #e400

#main =
#    'A'? 'B' p[ c ]
# eol 'A' p[content_length]
