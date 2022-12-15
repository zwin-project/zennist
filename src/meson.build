_sources = [
  'floor.cc',
  'jpeg-texture.cc',
  'main.cc',

  shaders['floor']['vert'],
  shaders['floor']['frag'],
]

_dependencies = [
  zukou_dep,
  jpeg_dep,
]

executable(
  'zennist',
  _sources,
  install: true,
  cpp_args: ['-include', 'constants.h'],
  dependencies: _dependencies,
)
