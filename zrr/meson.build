_zrr_srcs = [
  'src/error.cc',
]

_zrr_inc = include_directories('include')

_zrr_lib = static_library(
  'zrr',
  _zrr_srcs,
  install: false,
  include_directories: _zrr_inc,
)

zrr_dep = declare_dependency(
  link_with: _zrr_lib,
  include_directories: _zrr_inc,
)
