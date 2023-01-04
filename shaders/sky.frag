#version 320 es
precision mediump float;

uniform sampler2D sky_texture;

in vec4 pos;
in vec2 uv;
out vec4 out_color;

void
main()
{
  out_color = texture(sky_texture, vec2(0f, uv.y));
}
