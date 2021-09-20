project('lexy', ['cpp'], version: 'git')

cpp = meson.get_compiler('cpp')
cpp_args = []

###########################
# compiler specific flags #
###########################

if cpp.get_id() == 'gcc'
    cpp_args = ['-Wno-unused-local-typedefs']
endif

########
# lexy #
########

lexy_includes = [
    include_directories('include'),
]

lexy_sources = [
    'src/input/file.cpp'
]

lexy = static_library('lexy', lexy_sources,
    include_directories: lexy_includes,
    cpp_args: cpp_args,
)

lexy_dep = declare_dependency(
    include_directories: lexy_includes,
    link_with: lexy,
)