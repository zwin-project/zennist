#include "landscape.h"

#include <sys/mman.h>
#include <unistd.h>

#include <cstring>

#include "default.vert.h"
#include "landscape.frag.h"

namespace zennist {

Landscape::Landscape(zukou::System* system, zukou::VirtualObject* virtual_object,
    float length, uint32_t count_x, uint32_t count_z)
    : virtual_object_(virtual_object),
      length_(length),
      count_x_(count_x),
      count_z_(count_z),
      pool_(system),
      gl_vertex_buffer_(system),
      gl_element_array_buffer_(system),
      vertex_array_(system),
      vertex_shader_(system),
      fragment_shader_(system),
      program_(system),
      sampler_(system),
      rendering_unit_(system),
      base_technique_(system)
{}

Landscape::~Landscape()
{
  if (fd_ != 0) {
    close(fd_);
  }
}

Landscape::Vertex::Vertex(float x, float y, float z, float u, float v)
    : x(x), y(y), z(z), u(u), v(v)
{}

bool
Landscape::Render(float radius, glm::mat4 transform, glm::vec4 color1, glm::vec4 color2, glm::vec4 colorstripe, float freq, float threshold)
{
  if (!initialized_ && Init() == false) return true;

  auto local_model = glm::scale(transform, glm::vec3(radius));
  base_technique_.Uniform(0, "local_model", local_model);
  base_technique_.Uniform(0, "color_base1", color1);
  base_technique_.Uniform(0, "color_base2", color2);
  base_technique_.Uniform(0, "color_stripe", colorstripe);
  base_technique_.Uniform(0, "in_vec", glm::vec2(freq, threshold));

  return true;
}

bool
Landscape::Init()
{
  ConstructVertices();
  ConstructElements();

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
  if (!fragment_shader_.Init(GL_FRAGMENT_SHADER, landscape_frag_shader_source))
    return false;
  if (!program_.Init()) return false;
  if (!sampler_.Init()) return false;

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
Landscape::ConstructVertices()
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
      float u = (float)j / count_x_ / 2 + .5f;
      float v = (float)i / count_z_ / 2 + .5f;
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

      vertices_.emplace_back(x, y, z, u, v);  // (x1, y, z1), (x2, y, z1), ...
    }
  }
}

void
Landscape::ConstructElements()
{
  for (int32_t z = 0; z <= 2 * count_z_ - 1; z++) {
    for (int32_t x = 0; x <= 2 * count_x_ - 1; x++) {
      ushort A = x + z * (2 * count_z_ + 1);
      ushort B = (x + 1) + z * (2 * count_z_ + 1);
      ushort C = x + (z + 1) * (2 * count_z_ + 1);
      ushort D = (x + 1) + (z + 1) * (2 * count_z_ + 1);

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
