#pragma once

#include <zukou.h>

namespace zennist {

class SimpleObject
{
 public:
  DISABLE_MOVE_AND_COPY(SimpleObject);
  SimpleObject() = delete;
  SimpleObject(zukou::System* system, zukou::VirtualObject* virtual_object);
  virtual ~SimpleObject();

 protected:
  /**
   * Expected file format:
   * 0                                        pool_size
   * |   vertex buffer   |  element array buffer  |
   */
  virtual void FillFile(int fd) = 0;

  virtual void SetVertexArray(zukou::GlVertexArray* vertex_array,
      zukou::GlBuffer* gl_vertex_buffer) = 0;

  /**
   * @param element_array_buffer is null when element_array_buffer_size() <= 0
   */
  virtual void Draw(zukou::GlBaseTechnique* base_technique,
      zukou::GlBuffer* gl_element_array_buffer) = 0;

  virtual size_t vertex_buffer_size() = 0;

  virtual size_t element_array_buffer_size() = 0;

  virtual const std::string vertex_shader_source() = 0;

  virtual const std::string fragment_shader_source() = 0;

  inline size_t pool_size();

  inline bool initialized();

  bool Init();

 private:
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
  std::unique_ptr<zukou::GlTexture> texture_;
  zukou::GlSampler sampler_;

  zukou::RenderingUnit rendering_unit_;
  zukou::GlBaseTechnique base_technique_;
};

inline size_t
SimpleObject::pool_size()
{
  return vertex_buffer_size() + element_array_buffer_size();
}

inline bool
SimpleObject::initialized()
{
  return initialized_;
}

}  // namespace zennist
