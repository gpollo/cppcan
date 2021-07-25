subdir('grammar')

##############################################
# can::format::dbc::grammar::database test #
##############################################

test('can/format/dbc/database',
    executable('test_database', ['database.cpp'],
        include_directories: libcan_includes,
        dependencies: libcan_deps,
        link_with: libcan_static,
        cpp_args: cpp_flags,
    )
)