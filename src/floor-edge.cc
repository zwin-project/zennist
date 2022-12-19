#include "floor-edge.h"

#include <sys/mman.h>
#include <unistd.h>

#include <cstring>
#include <iostream>

#include "color.frag.h"
#include "default.vert.h"

namespace zennist {

FloorEdge::FloorEdge(
    zukou::System* system, zukou::VirtualObject* virtual_object)
    : virtual_object_(virtual_object),
      pool_(system),
      gl_vertex_buffer_(system),
      gl_element_array_buffer_(system),
      vertex_array_(system),
      vertex_shader_(system),
      fragment_shader_(system),
      program_(system),
      rendering_unit_(system),
      base_technique_(system)
{
  ConstructVertices();
  ConstructElements();
}

FloorEdge::~FloorEdge()
{
  if (fd_ != 0) {
    close(fd_);
  }
}

FloorEdge::Vertex::Vertex(float x, float y, float z, float u, float v)
    : x(x), y(y), z(z), u(u), v(v)
{}

bool
FloorEdge::Render(float radius, glm::mat4 transform)
{
  if (!initialized_ && Init() == false) {
    return false;
  }
  auto local_model = glm::scale(transform, glm::vec3(radius));
  base_technique_.Uniform(0, "local_model", local_model);
  base_technique_.Uniform(0, "color", rgbColor(35, 35, 35));

  return true;
}

bool
FloorEdge::Init()
{
  fd_ = zukou::Util::CreateAnonymousFile(pool_size());
  if (!pool_.Init(fd_, pool_size())) return false;
  if (!vertex_buffer_.Init(&pool_, 0, vertex_buffer_size())) return false;
  if (!element_array_buffer_.Init(
          &pool_, vertex_buffer_size(), element_array_buffer_size()))
    return false;

  if (!gl_vertex_buffer_.Init()) return false;
  if (!gl_element_array_buffer_.Init()) return false;
  if (!vertex_array_.Init()) return false;
  if (!vertex_shader_.Init(GL_VERTEX_SHADER, default_vert_shader_source))
    return false;
  if (!fragment_shader_.Init(GL_FRAGMENT_SHADER, color_frag_shader_source))
    return false;
  if (!program_.Init()) return false;

  if (!rendering_unit_.Init(virtual_object_)) return false;
  if (!base_technique_.Init(&rendering_unit_)) return false;

  {
    auto vertex_buffer_data = static_cast<char*>(
        mmap(nullptr, pool_size(), PROT_WRITE, MAP_SHARED, fd_, 0));
    auto element_array_buffer_data = vertex_buffer_data + vertex_buffer_size();

    std::memcpy(vertex_buffer_data, vertices_.data(), vertex_buffer_size());
    std::memcpy(element_array_buffer_data, elements_.data(),
        element_array_buffer_size());

    munmap(vertex_buffer_data, pool_size());
  }

  gl_vertex_buffer_.Data(GL_ARRAY_BUFFER, &vertex_buffer_, GL_STATIC_DRAW);
  gl_element_array_buffer_.Data(
      GL_ELEMENT_ARRAY_BUFFER, &element_array_buffer_, GL_STATIC_DRAW);

  program_.AttachShader(&vertex_shader_);
  program_.AttachShader(&fragment_shader_);
  program_.Link();

  vertex_array_.Enable(0);
  vertex_array_.VertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0, &gl_vertex_buffer_);
  vertex_array_.Enable(1);
  vertex_array_.VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      offsetof(Vertex, u), &gl_vertex_buffer_);

  base_technique_.Bind(&vertex_array_);
  base_technique_.Bind(&program_);

  base_technique_.DrawElements(GL_TRIANGLES, elements_.size(),
      GL_UNSIGNED_SHORT, 0, &gl_element_array_buffer_);

  initialized_ = true;

  return true;
}

void
FloorEdge::ConstructVertices()
{
  const static int resolution = 32;
  const static int radial_resolution = 2;
  const static float inner_ratio = 0.95f;
  for (float r = 0; r <= radial_resolution; r++) {
    float this_radius =
        1.f * inner_ratio + 1.f * (1.f - inner_ratio) / radial_resolution * r;
    for (float i = 0; i <= resolution; i++) {
      float theta = M_PI * 2.f * i / float(resolution);
      float x = this_radius * cosf(theta);
      float y = 0;
      float z = this_radius * sinf(theta);
      vertices_.emplace_back(x, y, z, x, z);
    }
  }
}

void
FloorEdge::ConstructElements()
{
  const static int resolution = 32;
  const static int radial_resolution = 2;

  // around
  for (int32_t r = 0; r <= radial_resolution - 1; r++) {
    for (int32_t i = 0; i <= resolution; i++) {
      ushort A = r * (resolution + 1) + i;
      ushort B = r * (resolution + 1) + i + 1;
      ushort C = (r + 1) * (resolution + 1) + i;
      ushort D = (r + 1) * (resolution + 1) + i + 1;

      elements_.push_back(A);
      elements_.push_back(B);
      elements_.push_back(C);

      elements_.push_back(B);
      elements_.push_back(C);
      elements_.push_back(D);
    }
  }
}

}  // namespace zennist
