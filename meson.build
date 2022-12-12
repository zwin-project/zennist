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

glm_dep = dependency('glm')
zukou_dep = dependency('zukou')
jpeg_dep = dependency('libjpeg')

# texture_files = files(
#   'assets/bg_teal.jpg',
# )
# texture_install_dir = join_paths(datadir, 'textures', 'zennist')
# install_data(texture_files, install_dir: texture_install_dir)
# texture_bg_teal = join_paths(get_option('prefix'), texture_install_dir, 'bg_teal.jpg')
# cdata.set_quoted('TEXTURE_BG_TEAL', texture_bg_teal)
# configure_file(
#   output: 'constants.h',
#   configuration: cdata
# )

textify = files('tools/textify.sh')[0]

subdir('shaders')
subdir('src')
