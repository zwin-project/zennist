_shaders = {
  'landscape': ['frag', 'vert'],
  'color': ['frag'],
  'texture': ['frag'],
  'default': ['vert'],
  'sky': ['frag'],
  'floor': ['frag', 'vert'],
  'icon': ['frag', 'vert'],
  'gltf': ['vert'],
  'gltf_color': ['frag'],
  'gltf_texture': ['frag'],
}

shaders = {}
# shaders[name][type]
# ex) shaders['landscape']['vert']

foreach name, types : _shaders
  info = {}
  foreach type : types
    input = '@0@.@1@'.format(name, type)
    output = '@0@.@1@.h'.format(name, type)
    variable_name = '@0@_@1@_shader_source'.format(name, type)

    glsl = custom_target(
      output,
      command: [textify, variable_name, '@INPUT@', '@OUTPUT@'],
      input: input,
      output: output,
    )

    info += {type: glsl}
  endforeach

  shaders += {name: info}
endforeach
