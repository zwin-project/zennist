#version 320 es
#define M_PI 3.1415
precision mediump float;

uniform vec4 color_base1;
uniform vec4 color_base2;
uniform vec4 color_stripe;
// uniform float freq;
// uniform float threshold;
uniform vec2 in_vec;// (freq, threshold)

in vec4 pos;
in vec2 uv;
out vec4 out_color;

void
main()
{
  vec4 base_color=mix(color_base1,color_base2,uv.x);
  float is_stripe=step(in_vec.y,sin(uv.x*M_PI*2.*in_vec.x));
  out_color=mix(base_color,color_stripe,is_stripe);
}
