project('libpcanbasic', 'c', version: 'latest')
cc = meson.get_compiler('c')

libpcanbasic_root = 'PCAN-Basic API'

if host_machine.cpu_family() == 'x86'
    libpcanbasic = cc.find_library('PCANBasic',
        dirs: meson.current_source_dir() / libpcanbasic_root / 'Win32',
    )
elif host_machine.cpu_family() == 'x86_64'
    libpcanbasic = cc.find_library('PCANBasic',
        dirs: meson.current_source_dir() / libpcanbasic_root / 'x64',
    )
else
    error('Invalid CPU family \'@0@\''.format(host_machine.cpu_family()))
endif

libpcanbasic_dep = declare_dependency(
    dependencies: [libpcanbasic],
    include_directories: include_directories(libpcanbasic_root / 'Include'),
)