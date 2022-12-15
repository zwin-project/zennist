#pragma once

#include <zukou.h>

#include <vector>

#include "jpeg-texture.h"

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
      float radius = 5.f);
  ~Floor();

  bool Render();

 private:
  bool Init();

  void ConstructVertices();

  bool initialized_ = false;

  zukou::VirtualObject* virtual_object_;

  const float radius_;

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
  JpegTexture texture_;
  zukou::RenderingUnit rendering_unit_;
  zukou::GlBaseTechnique base_technique_;

  std::vector<Vertex> vertices_;

  inline size_t vertex_buffer_size();
};

inline size_t
Floor::vertex_buffer_size()
{
  return sizeof(decltype(vertices_)::value_type) * vertices_.size();
}

}  // namespace zennist
