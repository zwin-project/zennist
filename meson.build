project(
  'zennist', 'c', 'cpp',
  version: '0.0.1',
  license: 'MIT',
  meson_version: '>= 0.58.0',
  default_options: ['warning_level=3', 'werror=true', 'optimization=2', 'c_std=gnu11', 'cpp_std=gnu++17'],
)

cxx = meson.get_compiler('cpp')
cdata = configuration_data()

global_args = []
global_args_maybe = [
  '-D_GNU_SOURCE',
  '-fvisibility=hidden',
]

foreach arg : global_args_maybe
  if cxx.has_argument(arg)
    global_args += arg
  endif
endforeach
add_project_arguments(global_args, language: 'cpp')

datadir = get_option('datadir')

zukou_req = '0.0.3'

glm_dep = dependency('glm')
zukou_dep = dependency('zukou', version: zukou_req)
cairo_dep = dependency('cairo')
jpeg_dep = dependency('libjpeg')
png_dep = dependency('libpng')
rsvg_dep = dependency('librsvg-2.0')
toml_dep = dependency('libtoml')

textify = files('tools/textify.sh')[0]

public_inc = include_directories('include')

install_subdir('assets', install_dir: get_option('datadir') / meson.project_name())
cdata.set_quoted('ZENNIST_ASSET_DIR', get_option('prefix') / get_option('datadir') / meson.project_name() / 'assets')

configure_file(
  output: 'constants.h',
  configuration: cdata,
)

subdir('shaders')
subdir('src')
