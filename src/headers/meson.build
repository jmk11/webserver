sources = ['contenttype.c', 'contenttype.h', 'custom.c', 'custom.h', 'headerfns.c', 'headerfns.h', 'headers.c', 'headers.h', 'headersbase.h']
headers_lib_static = static_library('headers',
                                    sources,
                                    link_with: [hashtable_lib_static, strings_lib_static, helpers_lib_static],
                                    include_directories: '..')