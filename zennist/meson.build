_zennist_srcs = [
  'application.cc',
  'config.cc',
  'main.cc',
  'monitor.cc',
  'xr-system.cc',

  'ui/footer.cc',
  'ui/wallpaper.cc',
  'ui/xr-system-button.cc',

  protocols_code['zen-desktop'],
  protocols_client_header['zen-desktop'],
]

_zennist_deps = [
  gtk_layer_shell_dep,
  gtkmm_dep,
  tomlplusplus_dep,
  wayland_client_dep,
  zrr_dep,
]

executable(
  'zennist',
  _zennist_srcs,
  install: true,
  cpp_args: ['-include', '../pch/pch.h', '-include', 'constants.h'],
  include_directories: zennist_inc,
  dependencies: _zennist_deps,
  cpp_pch: '../pch/pch.h',
)
