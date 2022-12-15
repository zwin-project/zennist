#version 320 es

uniform mat4 zMVP;
layout(location = 0) in vec4 position;
layout(location = 1) in vec2 uv_in;

out vec2 uv;

void
main()
{
  gl_Position = zMVP * position;
  uv = uv_in;
}
