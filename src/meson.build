subdir('helpers')   # hashtable_lib_static, strings_lib_static
subdir('wrappers')  # wrappers_lib_static
subdir('files')     # files_lib_static
subdir('http')      # http_lib_static
#subdir('common')
openssl = dependency('libssl')
crypto = dependency('libcrypto')
subdir('ssl') # ssl_lib_static

common_sources = ['common.c', 'common.h', 'constants.h', 'uid.h']
common_lib_static = static_library('common',
                                    common_sources,
                                    link_with: wrappers_lib_static)

webserver_sources = ['main.c', 'connection.c', 'connection.h', 'logging.c', 'logging.h']
executable('webserver', webserver_sources,
            link_with: [hashtable_lib_static, common_lib_static, ssl_lib_static, strings_lib_static, http_lib_static, wrappers_lib_static],
            dependencies: [openssl],
            install: true)

executable('httpredirect', 
            'httpredirect.c',
            link_with: [common_lib_static],
            install: true)
            #dependencies: wrappers_dep, # wrappers_dep is only here for error numbers, bad


#add_project_arguments(['-Wall', '-Wextra', '-pedantic', '-g'], language : 'c')
#hashtable = declare_dependency(link_with : hashtable_lib_static, include_directories : 'hashtable')
#common = declare_dependency(link_with : common_lib_static, include_directories : 'common')
#hashtable = dependency('hashtabled')
'''contenttype_lib_static = static_library('contenttype',
                                        ['contenttype.c', 'contenttype.h'],
                                        dependencies: [hashtable, common])'''
