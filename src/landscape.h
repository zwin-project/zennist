#pragma once

#include <zukou.h>

#include <vector>

#include "texture.h"

namespace zennist {

class Landscape
{
  struct Vertex {
    Vertex(float x, float y, float z, float u, float v);
    float x, y, z;
    float u, v;
  };

 public:
  DISABLE_MOVE_AND_COPY(Landscape);
  Landscape() = delete;
  Landscape(zukou::System* system, zukou::VirtualObject* virtual_object,
      float length_ = 5.f, uint32_t count_x_ = 50, uint32_t count_z_ = 50);
  ~Landscape();

  bool Render(
      float radius, glm::mat4 transform, const char* texturePath, float repeat);

 private:
  bool Init(const char* texturePath, float repeat);

  void ConstructVertices(float repeat);
  void ConstructElements();

  bool initialized_ = false;

  zukou::System* system_;
  zukou::VirtualObject* virtual_object_;

  const float length_;     // of a single square
  const int32_t count_x_;  // Number of squares in x direction
  const int32_t count_z_;  // Number of squares in z direction

  int fd_ = 0;
  zukou::ShmPool pool_;
  zukou::Buffer vertex_buffer_;
  zukou::Buffer element_array_buffer_;

  zukou::GlBuffer gl_vertex_buffer_;
  zukou::GlBuffer gl_element_array_buffer_;
  zukou::GlVertexArray vertex_array_;
  zukou::GlShader vertex_shader_;
  zukou::GlShader fragment_shader_;
  zukou::GlProgram program_;
  zukou::GlSampler sampler_;
  Texture* texture_;

  zukou::RenderingUnit rendering_unit_;
  zukou::GlBaseTechnique base_technique_;

  std::vector<Vertex> vertices_;
  std::vector<ushort> elements_;

  inline size_t vertex_buffer_size();

  inline size_t element_array_buffer_size();

  inline size_t pool_size();
};

inline size_t
Landscape::vertex_buffer_size()
{
  return sizeof(decltype(vertices_)::value_type) * vertices_.size();
}

inline size_t
Landscape::element_array_buffer_size()
{
  return sizeof(decltype(elements_)::value_type) * elements_.size();
}

inline size_t
Landscape::pool_size()
{
  return vertex_buffer_size() + element_array_buffer_size();
}

}  // namespace zennist
