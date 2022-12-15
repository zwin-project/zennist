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

void
main()
{
  vec4 base_color = mix(color_base1, color_base2, uv.x);
  float is_stripe = step(threshold, sin(uv.x * M_PI * 2. * freq));
  out_color = mix(base_color, color_stripe, is_stripe);
}
