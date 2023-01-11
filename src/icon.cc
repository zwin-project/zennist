#include "icon.h"

#include <sys/mman.h>
#include <unistd.h>

#include <cstring>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

#include "icon.frag.h"
#include "icon.vert.h"
#include "texture-factory.h"

namespace zennist {

Icon::Icon(zukou::System* system, zukou::VirtualObject* virtual_object)
    : system_(system),
      virtual_object_(virtual_object),
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
{
  ConstructVertices();
  ConstructElements();
}

Icon::~Icon()
{
  if (fd_ != 0) {
    close(fd_);
  }
}

Icon::Vertex::Vertex(float x, float y, float z, float u, float v)
    : x(x), y(y), z(z), u(u), v(v)
{}

bool
Icon::Render(const char* icon_texture_path, Cuboid& cuboid)
{
  if (!initialized_ && Init(icon_texture_path) == false) {
    return false;
  }

  glm::mat4 TRS = glm::translate(glm::mat4(1), cuboid.center) *
                  glm::toMat4(cuboid.quaternion) *
                  glm::scale(glm::mat4(1), cuboid.half_size);
  base_technique_.Uniform(0, "local_model", TRS);

  return true;
}

bool
Icon::Init(const char* icon_texture_path)
{
  icon_texture_ = TextureFactory::Create(system_, icon_texture_path);
  if (icon_texture_ == nullptr || !icon_texture_->Init() ||
      !icon_texture_->Load(icon_texture_path))
    return false;

  fd_ = zukou::Util::CreateAnonymousFile(pool_size());
  if (!pool_.Init(fd_, pool_size())) return false;
  if (!vertex_buffer_.Init(&pool_, 0, vertex_buffer_size())) return false;
  if (!element_array_buffer_.Init(
          &pool_, vertex_buffer_size(), element_array_buffer_size()))
    return false;

  if (!gl_vertex_buffer_.Init()) return false;
  if (!gl_element_array_buffer_.Init()) return false;
  if (!vertex_array_.Init()) return false;
  if (!vertex_shader_.Init(GL_VERTEX_SHADER, icon_vert_shader_source))
    return false;
  if (!fragment_shader_.Init(GL_FRAGMENT_SHADER, icon_frag_shader_source))
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

  icon_texture_->GenerateMipmap(GL_TEXTURE_2D);
  sampler_.Parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  sampler_.Parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  base_technique_.Bind(
      0, "in_icon_texture", icon_texture_, GL_TEXTURE_2D, &sampler_);

  base_technique_.DrawElements(GL_TRIANGLES, elements_.size(),
      GL_UNSIGNED_SHORT, 0, &gl_element_array_buffer_);

  initialized_ = true;

  return true;
}

void
Icon::ConstructVertices()
{
  for (float x = -0.5; x <= 0.5; x += 1.0) {
    for (float y = -0.5; y <= 0.5; y += 1.0) {
      for (float z = -0.5; z <= 0.5; z += 1.0) {
        vertices_.emplace_back(x, y, z, z + 0.5, 0.5 - y);
        // vertices_.emplace_back(x, y, z, y + 0.5, z + 0.5);
      }
    }
  }
}

void
Icon::ConstructElements()
{
  std::vector<ushort> values = {
      0, 1, 2,  //
      1, 2, 3,  //
      4, 5, 6,  //
      5, 6, 7,  //
      0, 1, 4,  //
      1, 4, 5,  //
      2, 3, 6,  //
      3, 6, 7,  //
      0, 2, 6,  //
      0, 4, 6,  //
      1, 3, 7,  //
      1, 5, 7,  //
  };

  elements_.swap(values);
}

}  // namespace zennist
