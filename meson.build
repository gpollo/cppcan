# requires Flexible Array Member (FAM) compiler extension

project('can', ['cpp'],
    default_options: ['cpp_std=c++20'],
    version: 'latest',
)

##################
# static library #
##################

can_includes = [
    include_directories('include'),
]

can_sources = [
    'source/driver/socketcan.cpp',
    'source/frame.cpp',
]

can_static = static_library('can', can_sources,
    include_directories: can_includes,
)

subdir('tests')

#################
# format target #
#################

format_include = '`find include -type f -iname \'*.hpp\'`'
format_source = '`find source -type f -iname \'*.cpp\'`'
format_tests = '`find tests -type f -iname \'*.cpp\'`'
format_files = '@0@ @1@ @2@'.format(format_include, format_source, format_tests)

run_target('format',
    command: [
        'sh', '-c',
        'cd @0@ && clang-format --verbose -i @1@'.format(
            meson.source_root(),
            format_files,
        )
    ],
)

###############
# tidy target #
###############

tidy_include = '`find include -type f -iname \'*.hpp\'`'
tidy_source = '`find source -type f -iname \'*.cpp\'`'
tidy_tests = '`find source -type f -iname \'*.cpp\'`'
tidy_files = '@0@ @1@ @2@'.format(tidy_include, tidy_source, tidy_tests)

run_target('tidy',
    command: [
        'sh', '-c',
        'cd @0@ && clang-tidy -p=@1@ @2@'.format(
            meson.source_root(),
            meson.current_build_dir(),
            tidy_files,
        )
    ],
)
