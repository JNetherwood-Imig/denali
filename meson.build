project(
  'denali',
  'c',
  version: '0.0.1',
  default_options: [
    'c_std=c23',
    'warning_level=2'
  ]
)

denali_srcs = files()

subdir('src')

denali_includes = include_directories('include')

denali_lib = shared_library('denali', sources: denali_srcs, include_directories: denali_includes)

denali_dep = declare_dependency(link_with: denali_lib, include_directories: denali_includes)

subdir('examples')
