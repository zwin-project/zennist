#version 320 es
#define M_PI 3.1415
precision mediump float;

uniform vec4 color_base1;
uniform vec4 color_base2;
uniform vec4 color_stripe;
uniform float freq;
uniform float threshold;

in vec4 pos;
in vec2 uv;
out vec4 out_color;

vec4 frag_stripe(float x);

void
main()
{
  out_color = frag_stripe(uv.x);
}

vec4
frag_stripe(float x)
{
  vec4 base_color = mix(color_base1, color_base2, x);
  float is_stripe = step(threshold, sin(x * M_PI * 2. * freq));
  return mix(base_color, color_stripe, is_stripe);
}
