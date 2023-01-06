_sources = [
  '../include/tiny_gltf.cc',
  'landscape.cc',
  'sphere.cc',
  'floor.cc',
  'floor-edge.cc',
  'roof.cc',
  'jpeg-texture.cc',
  'png-texture.cc',
  'viewer.cc',
  'main.cc',

  shaders['landscape']['vert'],
  shaders['landscape']['frag'],
  shaders['color']['frag'],
  shaders['sky']['frag'],
  shaders['texture']['frag'],
  shaders['default']['vert'],
  shaders['floor']['vert'],
  shaders['floor']['frag'],
  shaders['gltf']['vert'],
  shaders['gltf_color']['frag'],
  shaders['gltf_texture']['frag'],
]

_dependencies = [
  zukou_dep,
  jpeg_dep,
  png_dep,
]

executable(
  'zennist',
  _sources,
  install: true,
  cpp_args: ['-include', 'constants.h'],
  dependencies: _dependencies,
  include_directories: public_inc,
)
