sources = ['request/headerfns.c', 'request/headerfns.h', 'request/requestheaders.c', 'request/requestheaders.h',
            'response/contenttype.c', 'response/contenttype.h', 'response/custom.c', 'response/custom.h', 'response/headers.c', 'response/headers.h',
            'helpers.c', 'helpers.h',
            'response/response.c', 'response/response.h', 'statuscodes.h']
http_lib_static = static_library('http',
                                    sources,
                                    link_with: [hashtable_lib_static, strings_lib_static],
                                    )