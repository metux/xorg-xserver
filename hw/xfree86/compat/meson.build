srcs_xorg_compat = [
    'clientexception.c',
    'log.c',
]

xorg_compat = static_library('xorg_compat',
    srcs_xorg_compat,
    dependencies: common_dep,
    include_directories: [inc, xorg_inc, top_dir_inc],
)
