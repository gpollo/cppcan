subdir('grammar')

##############################
# can::utils::crop_cast test #
##############################

test('can/utils/crop_cast',
    executable('test_crop_cast', ['crop_cast.cpp'],
        include_directories: libcan_includes,
        dependencies: libcan_deps,
        cpp_args: cpp_flags,
    )
)

#####################################
# can::utils::unique_owner_ptr test #
#####################################

test('can/utils/unique_owner_ptr',
    executable('test_unique_owner_ptr', ['unique_owner_ptr.cpp'],
        include_directories: libcan_includes,
        dependencies: libcan_deps,
        cpp_args: cpp_flags,
    )
)