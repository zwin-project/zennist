_zennist_srcs = [
  'application.cc',
  'config.cc',
  'main.cc',
  'monitor.cc',
]

_zennist_deps = [
  gtk_layer_shell_dep,
  gtkmm_dep,
  tomlplusplus_dep,
  zrr_dep,
]

executable(
  'zennist',
  _zennist_srcs,
  install: true,
  cpp_args: ['-include', '../pch/pch.h'],
  include_directories: zennist_inc,
  dependencies: _zennist_deps,
  cpp_pch: '../pch/pch.h',
)
