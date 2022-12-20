#pragma once

#include <zukou.h>

#include <vector>

namespace zennist {

class FloorEdge
{
  struct Vertex {
    Vertex(float x, float y, float z, float u, float v);
    float x, y, z;
    float u, v;
  };

 public:
  DISABLE_MOVE_AND_COPY(FloorEdge);
  FloorEdge() = delete;
  FloorEdge(zukou::System* system, zukou::VirtualObject* virtual_object);
  ~FloorEdge();

  bool Render(float radius, glm::mat4 transform);
  glm::vec4 rgbColor(float r, float g, float b)
  {
    return glm::vec4(r / 255, g / 255, b / 255, 1);
  }

 private:
  bool Init();

  void ConstructVertices(
      int resolution, int radial_resolution, float inner_ratio);
  void ConstructElements(int resolution, int radial_resolution);

  bool initialized_ = false;

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
  zukou::RenderingUnit rendering_unit_;
  zukou::GlBaseTechnique base_technique_;

  std::vector<Vertex> vertices_;
  std::vector<ushort> elements_;

  inline size_t vertex_buffer_size();
  inline size_t element_array_buffer_size();
  inline size_t pool_size();
};

inline size_t
FloorEdge::vertex_buffer_size()
{
  return sizeof(decltype(vertices_)::value_type) * vertices_.size();
}

inline size_t
FloorEdge::element_array_buffer_size()
{
  return sizeof(decltype(elements_)::value_type) * elements_.size();
}

inline size_t
FloorEdge::pool_size()
{
  return vertex_buffer_size() + element_array_buffer_size();
}

}  // namespace zennist
