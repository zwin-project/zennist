#include "simple-object.h"

#include <sys/mman.h>
#include <unistd.h>

namespace zennist {

SimpleObject::SimpleObject(
    zukou::System* system, zukou::VirtualObject* virtual_object)
    : virtual_object_(virtual_object),
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

SimpleObject::~SimpleObject()
{
  if (fd_ != 0) {
    close(fd_);
  }
}

bool
SimpleObject::Init()
{
  bool use_element = element_array_buffer_size() > 0;

  fd_ = zukou::Util::CreateAnonymousFile(pool_size());
  if (!pool_.Init(fd_, pool_size())) return false;
  if (!vertex_buffer_.Init(&pool_, 0, vertex_buffer_size())) return false;
  if (use_element) {
    if (!element_array_buffer_.Init(
            &pool_, vertex_buffer_size(), element_array_buffer_size()))
      return false;
  }

  if (!gl_vertex_buffer_.Init()) return false;
  if (use_element) {
    if (!gl_element_array_buffer_.Init()) return false;
  }

  if (!vertex_array_.Init()) return false;
  if (!vertex_shader_.Init(GL_VERTEX_SHADER, vertex_shader_source()))
    return false;
  if (!fragment_shader_.Init(GL_FRAGMENT_SHADER, fragment_shader_source()))
    return false;
  if (!program_.Init()) return false;
  if (!sampler_.Init()) return false;

  if (!rendering_unit_.Init(virtual_object_)) return false;
  if (!base_technique_.Init(&rendering_unit_)) return false;

  FillFile(fd_);

  gl_vertex_buffer_.Data(GL_ARRAY_BUFFER, &vertex_buffer_, GL_STATIC_DRAW);
  if (use_element) {
    gl_element_array_buffer_.Data(
        GL_ELEMENT_ARRAY_BUFFER, &element_array_buffer_, GL_STATIC_DRAW);
  }

  program_.AttachShader(&vertex_shader_);
  program_.AttachShader(&fragment_shader_);
  program_.Link();

  SetVertexArray(&vertex_array_, &gl_vertex_buffer_);

  base_technique_.Bind(&vertex_array_);
  base_technique_.Bind(&program_);

  Draw(&base_technique_, use_element ? &gl_element_array_buffer_ : nullptr);

  initialized_ = true;

  return true;
}

}  // namespace zennist
