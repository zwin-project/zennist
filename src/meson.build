_sources = [
  'floor.cc',
  'sphere.cc',
  'jpeg-texture.cc',
  'main.cc',

  shaders['floor']['vert'],
  shaders['floor']['frag'],
  shaders['color']['frag'],
  shaders['texture']['frag'],
  shaders['default']['vert'],
]

_dependencies = [
  zukou_dep,
  jpeg_dep,
]

executable(
  'zennist',
  _sources,
  install: true,
  dependencies: _dependencies,
)
