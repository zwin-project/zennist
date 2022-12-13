#version 320 es
precision mediump float;

uniform sampler2D floor_texture;

in vec2 uv;
out vec4 out_color;

void
main()
{
  out_color = texture(floor_texture, uv);
}
