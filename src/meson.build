#subdir('bool')
subdir('hashtable') # hashtable_lib_static
subdir('wrappers')  # wrappers_dep
subdir('strings')   # strings_lib_static
subdir('helpers')   # helpers_lib_static
subdir('headers')   # headers_lib_static
#subdir('common')
openssl = dependency('libssl')
crypto = dependency('libcrypto')
subdir('ssl') # ssl_lib_static

common_sources = ['common.c', 'common.h', 'constants.h', 'uid.h']
common_lib_static = static_library('common',
                                    common_sources,
                                    dependencies: wrappers_dep)

webserver_sources = ['main.c', 'connection.c', 'connection.h', 'files.c', 'files.h']
executable('webserver', webserver_sources,
            link_with: [hashtable_lib_static, common_lib_static, ssl_lib_static, strings_lib_static, helpers_lib_static, headers_lib_static],
            dependencies: [wrappers_dep, openssl],
            install: true)

executable('http', 
            'http.c',
            link_with: [common_lib_static, helpers_lib_static],
            dependencies: wrappers_dep, # wrappers_dep is only here for error numbers, bad
            install: true)

#add_project_arguments(['-Wall', '-Wextra', '-pedantic', '-g'], language : 'c')
#hashtable = declare_dependency(link_with : hashtable_lib_static, include_directories : 'hashtable')
#common = declare_dependency(link_with : common_lib_static, include_directories : 'common')
#hashtable = dependency('hashtabled')
'''contenttype_lib_static = static_library('contenttype',
                                        ['contenttype.c', 'contenttype.h'],
                                        dependencies: [hashtable, common])'''
