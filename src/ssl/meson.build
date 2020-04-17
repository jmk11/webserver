sources = ['ssl.c', 'ssl.h']
ssl_lib_static = static_library('ssl',
                                sources,
                                link_with: strings_lib_static,
                                dependencies: [helpers_dep, crypto, openssl]
                                )