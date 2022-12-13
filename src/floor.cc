#include "floor.h"

#include <sys/mman.h>

#include <cstring>

#include "floor.frag.h"
#include "floor.vert.h"

namespace zennist {

Floor::Floor(zukou::System* system, zukou::VirtualObject* virtual_object,
    float length, uint32_t count_x, uint32_t count_z)
    : SimpleObject(system, virtual_object),
      length_(length),
      count_x_(count_x),
      count_z_(count_z)
{
  ConstructVertices();
  ConstructElements();
}

Floor::Vertex::Vertex(float x, float y, float z, float u, float v)
    : x(x), y(y), z(z), u(u), v(v)
{}

bool
Floor::Render()
{
  return initialized() || Init() == true;
}

void
Floor::ConstructVertices()
{
  float mountains[][4] = {
      // {x, z, height, flatness}
      {200, -200, 60, 250.f},
      {-300, -100, 40, 300.f},
  };

  for (int32_t i = -count_z_; i <= count_z_; i++) {
    for (int32_t j = -count_x_; j <= count_x_; j++) {
      float x = (float)j * length_;
      float z = (float)i * length_;
      float y = 0;

      for (uint32_t k = 0; k < sizeof(mountains) / sizeof(mountains[0]); k++) {
        float x0 = mountains[k][0];
        float z0 = mountains[k][1];
        float h = mountains[k][2];
        float flatness = mountains[k][3];
        float r = sqrtf(powf(x - x0, 2) + powf(z - z0, 2));
        if (r > flatness) continue;
        y += h * (cosf(M_PI * r / flatness) + 1.f) / 2.f;
      }

      vertices_.emplace_back(x, y, z, 0, 0);
    }
  }
}

void
Floor::ConstructElements()
{
  // x-axis
  for (int32_t z = 0; z <= 2 * count_z_; z++) {
    for (int32_t x = 0; x < 2 * count_x_; x++) {
      elements_.push_back(x + z * (2 * count_x_ + 1));
      elements_.push_back((x + 1) + z * (2 * count_x_ + 1));
    }
  }

  // z-axis
  for (int32_t x = 0; x <= 2 * count_x_; x++) {
    for (int32_t z = 0; z < 2 * count_z_; z++) {
      elements_.push_back(x + z * (2 * count_x_ + 1));
      elements_.push_back(x + (z + 1) * (2 * count_x_ + 1));
    }
  }
}

void
Floor::FillFile(int fd)
{
  auto vertex_buffer_data = static_cast<char*>(
      mmap(nullptr, pool_size(), PROT_WRITE, MAP_SHARED, fd, 0));
  auto element_array_buffer_data = vertex_buffer_data + vertex_buffer_size();

  std::memcpy(vertex_buffer_data, vertices_.data(), vertex_buffer_size());
  std::memcpy(
      element_array_buffer_data, elements_.data(), element_array_buffer_size());

  munmap(vertex_buffer_data, pool_size());
}

void
Floor::SetVertexArray(
    zukou::GlVertexArray* vertex_array, zukou::GlBuffer* gl_vertex_buffer)
{
  vertex_array->Enable(0);
  vertex_array->VertexAttribPointer(
      0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0, gl_vertex_buffer);
  vertex_array->Enable(1);
  vertex_array->VertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
      offsetof(Vertex, u), gl_vertex_buffer);
}

void
Floor::Draw(zukou::GlBaseTechnique* base_technique,
    zukou::GlBuffer* gl_element_array_buffer)
{
  base_technique->DrawElements(GL_LINES, elements_.size(), GL_UNSIGNED_SHORT, 0,
      gl_element_array_buffer);
}

size_t
Floor::vertex_buffer_size()
{
  return sizeof(decltype(vertices_)::value_type) * vertices_.size();
}

size_t
Floor::element_array_buffer_size()
{
  return sizeof(decltype(elements_)::value_type) * elements_.size();
}

const std::string
Floor::vertex_shader_source()
{
  return std::string(floor_vert_shader_source);
}

const std::string
Floor::fragment_shader_source()
{
  return std::string(floor_frag_shader_source);
}

}  // namespace zennist
