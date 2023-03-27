project(
  'zennist', 'c', 'cpp',
  version: '0.1.2-dev',
  license: 'MIT',
  meson_version: '>= 0.58.0',
  default_options: ['warning_level=3', 'werror=true', 'optimization=2', 'c_std=gnu11', 'cpp_std=gnu++17'],
)

################################
##        global args         ##
################################

cxx = meson.get_compiler('cpp')
cc = meson.get_compiler('c')
cdata = configuration_data()

global_args = []
global_args_maybe = [
  '-D_GNU_SOURCE',
  '-DTOML_EXCEPTIONS=0',
  '-DTOML_HEADER_ONLY=1',
  '-fvisibility=hidden',
  '-Wno-gnu-zero-variadic-macro-arguments',
  '-Wno-gnu-empty-initializer',
  '-Wno-zero-length-array',
]

foreach arg : global_args_maybe
  if cxx.has_argument(arg)
    global_args += arg
  endif
endforeach
add_project_arguments(global_args, language: 'cpp')

global_args = []
foreach arg : global_args_maybe
  if cc.has_argument(arg)
    global_args += arg
  endif
endforeach
add_project_arguments(global_args, language: 'c')



################################
##        dependencies        ##
################################

wayland_req = '>= 1.18.0'
zen_protocols_req = '0.1.0'

tomlplusplus_proj = subproject(
  'tomlplusplus',
  default_options: [
    'compile_library=false',
  ]
)

gtkmm_dep = dependency('gtkmm-3.0')
gtk_layer_shell_dep = dependency('gtk-layer-shell-0')
tomlplusplus_dep = tomlplusplus_proj.get_variable('tomlplusplus_dep')
wayland_client_dep = dependency('wayland-client', version: wayland_req)
wayland_scanner_dep = dependency('wayland-scanner', native: true)
zen_protocols_dep = dependency('zen-protocols', version: zen_protocols_req)


################################
##            data            ##
################################

datadir = get_option('datadir')
project_name = meson.project_name()

subdir('assets/wallpapers')

install_subdir(
  'assets',
  install_dir: datadir / project_name,
  exclude_directories: [ 'wallpapers' ],
)

cdata.set_quoted(
  'ZENNIST_ASSET_DIR',
  get_option('prefix') / datadir / project_name / 'assets'
)

install_subdir(
  'zennist/css',
  install_dir: datadir / project_name,
)

cdata.set_quoted(
  'ZENNIST_CSS_DIR',
  get_option('prefix') / datadir / project_name / 'css'
)

configure_file(
  output: 'constants.h',
  configuration: cdata,
)


################################
##           build            ##
################################

textify = files('tools/textify.sh')[0]
zennist_inc = include_directories('zennist')

subdir('protocol')
subdir('zrr')
subdir('shaders')
subdir('zennist')
