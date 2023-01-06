#version 320 es

precision mediump float;

uniform vec4 in_base_color;

in vec3 in_normal;
in vec2 in_uv;

out vec4 outputColor;

void
main()
{
  outputColor = in_base_color;
}
