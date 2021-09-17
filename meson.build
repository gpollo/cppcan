# requires Flexible Array Member (FAM) compiler extension

project('can', ['cpp'],
    default_options: ['cpp_std=c++20'],
    version: 'latest',
)

cpp = meson.get_compiler('cpp')
cpp_flags = ['-Wno-unused-local-typedefs', '-Wno-parentheses']

#########################
# drivers configuration #
#########################

if host_machine.system() == 'linux'
    cpp_flags += '-DBUILD_LINUX'
    enable_driver_socketcan   = true
    enable_driver_pcan        = true
    enable_driver_candlelight = false
    libpcanbasic_subproject   = subproject('libpcanbasic-linux')
    libsocketcan_subproject   = subproject('libsocketcan')
    winusb_dep                = []
    setupapi_dep              = []
elif host_machine.system() == 'windows'
    cpp_flags += '-DBUILD_WINDOWS'
    enable_driver_socketcan   = false
    enable_driver_pcan        = true
    enable_driver_candlelight = true
    libpcanbasic_subproject   = subproject('libpcanbasic-windows')
    libcandleapi_subproject   = subproject('libcandleapi')
    winusb_dep                = cpp.find_library('winusb', required: true)
    setupapi_dep              = cpp.find_library('setupapi', required: true)
endif

cpp_flags += (enable_driver_socketcan)   ? '-DENABLE_DRIVER_SOCKETCAN'  : []
cpp_flags += (enable_driver_pcan)        ? '-DENABLE_DRIVER_PCAN'       : []
cpp_flags += (enable_driver_candlelight) ? '-DENABLE_DRIVER_CANDLELIGHT': []

fmt_subproject = subproject('fmt')
lexy_subproject = subproject('lexy')
spdlog_subproject = subproject('spdlog')

##################
# static library #
##################

libcan_includes = [
    include_directories('include'),
]

libcan_sources = [
    (enable_driver_socketcan)   ? 'source/can/driver/socketcan.cpp'   : [],
    (enable_driver_pcan)        ? 'source/can/driver/pcan.cpp'        : [],
    (enable_driver_candlelight) ? 'source/can/driver/candlelight.cpp' : [],
    'source/can/database.cpp',
    'source/can/databases.cpp',
    'source/can/format/dbc/ast/attribute_definition.cpp',
    'source/can/format/dbc/ast/database.cpp',
    'source/can/format/dbc/ast/message.cpp',
    'source/can/format/dbc/ast/signal.cpp',
    'source/can/format/dbc/database.cpp',
    'source/can/format/dbc/message.cpp',
    'source/can/format/dbc/node.cpp',
    'source/can/format/dbc/object.cpp',
    'source/can/format/dbc/signal.cpp',
    'source/can/frame.cpp',
    'source/can/listener.cpp',
    'source/can/log.cpp',
    'source/can/transceiver.cpp',
    'source/can/utils/quark.cpp',
]

libcan_deps = [
    (enable_driver_socketcan)   ? libsocketcan_subproject.get_variable('libsocketcan_dep') : [],
    (enable_driver_pcan)        ? libpcanbasic_subproject.get_variable('libpcanbasic_dep') : [],
    (enable_driver_candlelight) ? libcandleapi_subproject.get_variable('libcandleapi_dep') : [],
    lexy_subproject.get_variable('lexy_dep'),
    spdlog_subproject.get_variable('spdlog_dep'),
    setupapi_dep,
    winusb_dep,
]

libcan_static = library('can', libcan_sources,
    include_directories: libcan_includes,
    dependencies: libcan_deps,
    cpp_args: cpp_flags,
)

libcan_dep = declare_dependency(
    include_directories: libcan_includes,
    link_with: libcan_static,
)

subdir('tests')

###########
# candump #
###########

candump_includes = [
    include_directories('include'),
]

candump_sources = [
    'source/candump/main.cpp'
]

candump_deps = [
    fmt_subproject.get_variable('fmt_dep'),
    libcan_dep,
]

candump = executable('candump', candump_sources,
    include_directories: candump_includes,
    dependencies: candump_deps,
    cpp_args: cpp_flags,
)

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
