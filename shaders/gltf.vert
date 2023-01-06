#version 320 es

uniform mat4 zMVP;

uniform mat4 local_model;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 texcoord;

out vec3 in_normal;
out vec2 in_uv;

void
main()
{
  gl_Position = zMVP * local_model * vec4(position, 1);
  in_normal = norm;
  in_uv = texcoord;
}
