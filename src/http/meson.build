sources = ['headers/request/headerfns.c', 'headers/request/headerfns.h', 'headers/request/requestheaders.c', 'headers/request/requestheaders.h', 'headers/request/requestheadersbase.h',
            'headers/response/contenttype.c', 'headers/response/contenttype.h', 'headers/response/custom.c', 'headers/response/custom.h', 'headers/response/headers.c', 'headers/response/headers.h', 'headers/response/responseheadersbase.h',
            'headers/helpers.c', 'headers/helpers.h',
            'response.c', 'response.h', 'statuscodes.h']
http_lib_static = static_library('http',
                                    sources,
                                    link_with: [hashtable_lib_static, strings_lib_static, files_lib_static],
                                    )