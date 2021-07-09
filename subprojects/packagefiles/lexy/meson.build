project('lexy', ['cpp'], version: 'git')

lexy_includes = [
    include_directories('include'),
]

lexy_sources = [
    'src/input/file.cpp'
]

lexy = static_library('lexy', lexy_sources,
    include_directories: lexy_includes,
    cpp_args: ['-Wno-unused-local-typedefs'],
)

lexy_dep = declare_dependency(
    include_directories: lexy_includes,
    link_with: lexy,
)