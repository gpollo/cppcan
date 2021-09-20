project('libcandleapi', ['c'], version: 'git')
cflags = ['-Wno-incompatible-pointer-types']

libcandleapi_includes = [
    include_directories('.'),
]

libcandleapi_sources = [
    'candle_ctrl_req.c',
    'candle.c',
]

libcandleapi = static_library('candleapi', libcandleapi_sources,
    include_directories: libcandleapi_includes,
    c_args: cflags,
)

libcandleapi_dep = declare_dependency(
    link_with: libcandleapi,
    include_directories: libcandleapi_includes,
)