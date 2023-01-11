#pragma once

#include <zukou.h>

#include <vector>

#include "cuboid.h"
#include "png-texture.h"
#include "texture.h"

namespace zennist {

class IconBackground
{
  struct Vertex {
    Vertex(float x, float y, float z, float u, float v);
    float x, y, z;
    float u, v;
  };

 public:
  DISABLE_MOVE_AND_COPY(IconBackground);
  IconBackground() = delete;
  IconBackground(zukou::System* system, zukou::VirtualObject* virtual_object);
  ~IconBackground();

  bool Init(Cuboid& cuboid);
  void Show();
  void Hide();

 private:
  bool Setup();

  void ConstructVertices();
  void ConstructElements();

  bool initialized_ = false;
  bool shown_ = false;

  zukou::System* system_;
  zukou::VirtualObject* virtual_object_;

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
  zukou::RenderingUnit rendering_unit_;
  zukou::GlBaseTechnique base_technique_;

  std::vector<Vertex> vertices_;
  std::vector<ushort> elements_;

  Texture* icon_texture_;

  inline size_t vertex_buffer_size();
  inline size_t element_array_buffer_size();
  inline size_t pool_size();
};

inline size_t
IconBackground::vertex_buffer_size()
{
  return sizeof(decltype(vertices_)::value_type) * vertices_.size();
}

inline size_t
IconBackground::element_array_buffer_size()
{
  return sizeof(decltype(elements_)::value_type) * elements_.size();
}

inline size_t
IconBackground::pool_size()
{
  return vertex_buffer_size() + element_array_buffer_size();
}

}  // namespace zennist
