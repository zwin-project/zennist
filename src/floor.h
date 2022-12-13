#pragma once

#include <zukou.h>

#include <vector>

#include "simple-object.h"

namespace zennist {

class Floor final : public SimpleObject
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
      float length = 5.f, uint32_t count_x = 100, uint32_t count_z = 100);
  ~Floor() = default;

  bool Render();

 protected:
  void FillFile(int fd) override;

  void SetVertexArray(zukou::GlVertexArray* vertex_array,
      zukou::GlBuffer* gl_vertex_buffer) override;

  void Draw(zukou::GlBaseTechnique* base_technique,
      zukou::GlBuffer* gl_element_array_buffer) override;

  size_t vertex_buffer_size() override;

  size_t element_array_buffer_size() override;

  const std::string vertex_shader_source() override;

  const std::string fragment_shader_source() override;

 private:
  void ConstructVertices();
  void ConstructElements();

  const float length_;     // of a single square
  const int32_t count_x_;  // Number of squares in x direction
  const int32_t count_z_;  // Number of squares in z direction

  std::vector<Vertex> vertices_;
  std::vector<ushort> elements_;
};

}  // namespace zennist
