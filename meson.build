project(
  'better_wayland',
  'c',
  version: '0.0.1',
  default_options: [
    'c_std=c23',
    'warning_level=2'
  ]
)

wl_srcs = files()

subdir('src')

wl_priv_includes = include_directories('include')
wl_pub_includes = include_directories('include/wl')

lib = shared_library('wl', sources: wl_srcs, include_directories: [wl_priv_includes, wl_pub_includes])

wl_dep = declare_dependency(link_with: lib, include_directories: wl_pub_includes)

subdir('examples')
