#version 320 es
precision mediump float;

uniform vec4 color1;
uniform vec4 color2;

in vec4 pos;
in vec2 uv;
out vec4 out_color;

void
main()
{
  out_color=mix(color1,color2,uv.y*2.);
}
