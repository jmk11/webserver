#add_project_arguments(['-Wall', '-Wextra', '-pedantic', '-g'], language : 'c')
subdir('common')
subdir('hashtable')

hashtable = declare_dependency(link_with : hashtable_lib_static, include_directories : 'hashtable')
common = declare_dependency(link_with : common_lib_static, include_directories : 'common')

webserver_sources = ['custom.c', 'custom.h', 'main.c', 'ssl.c', 'ssl.h', 'uid.h', 'contenttype.c', 'contenttype.h']
crypto = dependency('libcrypto')
openssl = dependency('libssl')
executable('webserver', webserver_sources,
            dependencies: [crypto, openssl, hashtable, common],
            install: true)

executable('http', 
            'http.c',
            dependencies: common,
            install: true)


#hashtable = dependency('hashtabled')
'''contenttype_lib_static = static_library('contenttype',
                                        ['contenttype.c', 'contenttype.h'],
                                        dependencies: [hashtable, common])'''
