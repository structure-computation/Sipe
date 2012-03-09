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
    uint[ content_length ]
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

pc = { putchar(*data); putchar('\n'); }
pd = { putchar('+'); putchar('\n'); }



co =
    10 # 'C'
    '1'++
    #uint[ val = 'content_length' ]
    #d[ 'sipe_data->content_length' ]
    pd

po_data =
    (
        co |
        #( eol eol @end_post_data ) |
        any
    )**
    # end_post_data:

po = 'P' p[ post ] po_data


main =
    #_add_prel[ '#define P(A) std::cout << #A << " -> " << A << std::endl;' ]
    #_set_strn[ 'Http' ]
    #'Z'

    # ( '1' pc ( '1' pc )** pd # p["pouet"]

    #(
    #   ( 'AB' uint[content_length] d[ 'sipe_data->content_length' ] # '1' pc ( digit pc )** pd # p["pouet"]
    #   ) | any
    #)**
    #get  [freq=10] |
    #put  [freq= 1] |
    post #|
    #e400

#main =
#    'A'? 'B' p[ c ]
# eol 'A' p[content_length]
