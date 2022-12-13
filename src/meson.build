_sources = [
  'floor.cc',
  'simple-object.cc',
  'main.cc',

  shaders['floor']['vert'],
  shaders['floor']['frag'],
]

_dependencies = [
  zukou_dep,
]

executable(
  'zennist',
  _sources,
  install: true,
  dependencies: _dependencies,
)
