#pragma once

#include <zukou.h>

#include <vector>

#include "texture.h"

namespace zennist {

class Floor
{
  struct Vertex {
    Vertex(float x, float y, float z, float u, float v);
    float x, y, z;
    float u, v;
  };

 public:
  DISABLE_MOVE_AND_COPY(Floor);
  Floor() = delete;
  Floor(zukou::System* system, zukou::VirtualObject* virtual_object,
      float radius);
  ~Floor();

  bool Render(float scale, glm::mat4 transform);

 private:
  bool Init();

  void ConstructVertices(int resolution, int radial_resolution);
  void ConstructElements(int resolution, int radial_resolution);

  bool initialized_ = false;

  zukou::System* system_;
  zukou::VirtualObject* virtual_object_;

  float radius_;

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
Floor::vertex_buffer_size()
{
  return sizeof(decltype(vertices_)::value_type) * vertices_.size();
}

inline size_t
Floor::element_array_buffer_size()
{
  return sizeof(decltype(elements_)::value_type) * elements_.size();
}

inline size_t
Floor::pool_size()
{
  return vertex_buffer_size() + element_array_buffer_size();
}

}  // namespace zennist
