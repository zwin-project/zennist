#include "floor.h"

#include <sys/mman.h>
#include <unistd.h>

#include <cstring>

#include "floor.frag.h"
#include "floor.vert.h"

namespace {

constexpr char kFloorTexturePath[] =
    ZENNIST_ASSET_DIR "/dark+chevron+parquet-512x512.jpeg";

}  // namespace

namespace zennist {

Floor::Floor(
    zukou::System* system, zukou::VirtualObject* virtual_object, float radius)
    : virtual_object_(virtual_object),
      radius_(radius),
      pool_(system),
      gl_vertex_buffer_(system),
      gl_element_array_buffer_(system),
      vertex_array_(system),
      vertex_shader_(system),
      fragment_shader_(system),
      program_(system),
      sampler_(system),
      texture_(system),
      rendering_unit_(system),
      base_technique_(system)
{
  ConstructVertices();
}

Floor::~Floor()
{
  if (fd_ != 0) {
    close(fd_);
  }
}

Floor::Vertex::Vertex(float x, float y, float z, float u, float v)
    : x(x), y(y), z(z), u(u), v(v)
{}

bool
Floor::Render()
{
  return initialized_ || Init() == true;
}

bool
Floor::Init()
{
  fd_ = zukou::Util::CreateAnonymousFile(vertex_buffer_size());
  if (!pool_.Init(fd_, vertex_buffer_size())) return false;
  if (!vertex_buffer_.Init(&pool_, 0, vertex_buffer_size())) return false;

  if (!gl_vertex_buffer_.Init()) return false;
  if (!vertex_array_.Init()) return false;
  if (!vertex_shader_.Init(GL_VERTEX_SHADER, floor_vert_shader_source))
    return false;
  if (!fragment_shader_.Init(GL_FRAGMENT_SHADER, floor_frag_shader_source))
    return false;
  if (!program_.Init()) return false;

  if (!texture_.Init() || !texture_.Load(kFloorTexturePath)) return false;
  if (!sampler_.Init()) return false;

  if (!rendering_unit_.Init(virtual_object_)) return false;
  if (!base_technique_.Init(&rendering_unit_)) return false;

  {
    auto vertex_buffer_data =
        mmap(nullptr, vertex_buffer_size(), PROT_WRITE, MAP_SHARED, fd_, 0);

    std::memcpy(vertex_buffer_data, vertices_.data(), vertex_buffer_size());

    munmap(vertex_buffer_data, vertex_buffer_size());
  }

  gl_vertex_buffer_.Data(GL_ARRAY_BUFFER, &vertex_buffer_, GL_STATIC_DRAW);

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

  sampler_.Parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  sampler_.Parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  base_technique_.Bind(0, "floor_texture", &texture_, GL_TEXTURE_2D, &sampler_);

  base_technique_.DrawArrays(GL_TRIANGLE_FAN, 0, vertices_.size());

  initialized_ = true;

  return true;
}

void
Floor::ConstructVertices()
{
  const static int resolution = 32;
  vertices_.emplace_back(0, 0, 0, 0, 0);
  for (float i = 0; i <= resolution; i++) {
    float theta = M_PI * 2.f * i / float(resolution);
    float x = radius_ * cosf(theta);
    float y = 0.2f;
    float z = radius_ * sinf(theta);
    vertices_.emplace_back(x, y, z, x, z);
  }
}

}  // namespace zennist