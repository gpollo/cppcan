subdir('grammar')

##############################################
# can::database::dbc::grammar::database test #
##############################################

test('can/database/dbc/database',
    executable('test_database', ['database.cpp'],
        include_directories: can_includes,
        dependencies: can_deps,
        link_with: can_static,
        cpp_args: cpp_flags,
    )
)