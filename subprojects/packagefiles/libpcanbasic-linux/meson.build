project('libpcanbasic-linux', ['c'], version: '4.5.4')

cflags = ['-DNO_RT']

################
# libpcanbasic #
################

libpcanbasic_root = 'PCAN-Basic_Linux-4.7.0.3/libpcanbasic/pcanbasic'

libpcanbasic_includes = [
    include_directories(libpcanbasic_root / 'include'),
    include_directories(libpcanbasic_root / 'src/pcan/driver'),
    include_directories(libpcanbasic_root / 'src/pcan/lib')
]

libpcanbasic_sources = [
    libpcanbasic_root / 'src/pcbtrace.c',
    libpcanbasic_root / 'src/pcblog.c',
    libpcanbasic_root / 'src/pcbcore.c',
    libpcanbasic_root / 'src/pcanlog.c',
    libpcanbasic_root / 'src/pcaninfo.c',
    libpcanbasic_root / 'src/pcan/lib/src/libpcanfd.c',
    libpcanbasic_root / 'src/libpcanbasic.c',
]

libpcanbasic = static_library('pcanbasic', libpcanbasic_sources,
    include_directories: libpcanbasic_includes,
    c_args: cflags,
)

libpcanbasic_dep = declare_dependency(
    link_with: libpcanbasic,
    include_directories: libpcanbasic_includes,
)