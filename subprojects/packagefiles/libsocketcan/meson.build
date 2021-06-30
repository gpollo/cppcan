project('libsocketcan', ['c'], version: 'git')

cc = meson.get_compiler('c')
cc.check_header('arpa/inet.h',  required: true)
cc.check_header('limits.h',     required: true)
cc.check_header('netdb.h',      required: true)
cc.check_header('netinet/in.h', required: true)
cc.check_header('stddef.h',     required: true)
cc.check_header('stdlib.h',     required: true)
cc.check_header('string.h',     required: true)
cc.check_header('sys/param.h',  required: true)
cc.check_header('sys/socket.h', required: true)
cc.check_header('sys/time.h',   required: true)
cc.check_header('sys/un.h',     required: true)
cc.check_header('unistd.h',     required: true)
cc.check_header('utime.h',      required: true)
cc.check_header('net/if.h',     required: true)

libsocketcan_includes = [
    include_directories('include'),
]

libsocketcan_sources = [
    'src/libsocketcan.c',
]

libsocketcan_deps = [
    cc.find_library('netlink', required: true, static: true),
]

libsocketcan = static_library('socketcan', libsocketcan_sources,
    include_directories: libsocketcan_includes,
    dependencies: libsocketcan_deps,
)

libsocketcan_dep = declare_dependency(
    link_with: libsocketcan,
    include_directories: libsocketcan_includes,
)