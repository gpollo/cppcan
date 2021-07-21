subdir('grammar')

##############################################
# can::format::dbc::grammar::database test #
##############################################

test('can/format/dbc/database',
    executable('test_database', ['database.cpp'],
        include_directories: can_includes,
        dependencies: can_deps,
        link_with: can_static,
        cpp_args: cpp_flags,
    )
)