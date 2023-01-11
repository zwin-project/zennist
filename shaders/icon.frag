#version 320 es
precision mediump float;

uniform sampler2D in_icon_texture;

in vec2 uv;
out vec4 out_color;

void
main()
{
  vec4 color = texture(in_icon_texture, uv);

  if (color.a < 0.5) discard;
  out_color = color;
}
