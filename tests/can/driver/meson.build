###############################
# can::driver::socketcan test #
###############################

if enable_driver_socketcan
    executable('test_socketcan', ['socketcan.cpp'],
        include_directories: libcan_includes,
        dependencies: libcan_deps,
        link_with: libcan_static,
        cpp_args: cpp_flags,
    )
endif