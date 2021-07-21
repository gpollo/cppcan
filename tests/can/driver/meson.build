###############################
# can::driver::socketcan test #
###############################

if enable_driver_socketcan
    executable('test_socketcan', ['socketcan.cpp'],
        include_directories: can_includes,
        dependencies: can_deps,
        link_with: can_static,
        cpp_args: cpp_flags,
    )
endif