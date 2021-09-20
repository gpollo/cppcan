project('libcandleapi', ['c'], version: 'git')

cc = meson.get_compiler('cpp')
c_args = []

###########################
# compiler specific flags #
###########################

if cc.get_id() == 'gcc'
    c_args = ['-Wno-incompatible-pointer-types']
endif

################
# libcandleapi #
################

libcandleapi_includes = [
    include_directories('.'),
]

libcandleapi_sources = [
    'candle_ctrl_req.c',
    'candle.c',
]

libcandleapi = static_library('candleapi', libcandleapi_sources,
    include_directories: libcandleapi_includes,
    c_args: c_args,
)

libcandleapi_dep = declare_dependency(
    link_with: libcandleapi,
    include_directories: libcandleapi_includes,
)