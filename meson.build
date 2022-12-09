project(
  'zennist', 'c', 'cpp',
  version: '0.0.1',
  license: 'MIT',
  meson_version: '>= 0.58.0',
  default_options: ['warning_level=3', 'werror=true', 'optimization=2', 'c_std=gnu11', 'cpp_std=gnu++17'],
)

cxx = meson.get_compiler('cpp')

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

glm_dep = dependency('glm')
zukou_dep = dependency('zukou')

public_inc = include_directories('include')

textify = files('tools/textify.sh')[0]

subdir('shaders')
subdir('src')
