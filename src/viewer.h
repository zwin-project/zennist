#pragma once

#include <linux/input.h>
#include <sys/mman.h>
#include <zukou.h>

#include <cstring>
#include <deque>
#include <filesystem>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <unordered_map>
#include <utility>
#include <vector>

#include "texture.h"
#include "tiny_gltf.h"

namespace zennist {

class Viewer : public zukou::IBoundedDelegate, public zukou::ISystemDelegate
{
 public:
  DISABLE_MOVE_AND_COPY(Viewer);
  Viewer() = delete;
  Viewer(zukou::System *system, zukou::VirtualObject *virtual_object);
  ~Viewer();

  bool Render(float radius, glm::mat4 transform, const char *model_path);

 private:
  bool Load(const char *model_path);
  bool Setup();

  bool RenderScene();
  bool RenderNode(const tinygltf::Node &node);
  bool RenderMesh(const tinygltf::Mesh &mesh);

  void SetInitialPosition(float radius, glm::mat4 transform);
  glm::mat4 CalculateLocalModel();

  tinygltf::Model *model_;
  std::filesystem::path parent_dir_;

  zukou::System *system_;
  zukou::VirtualObject *virtual_object_;

  int fd_ = 0;
  zukou::ShmPool pool_;
  zukou::GlShader vertex_shader_;
  zukou::GlShader texture_fragment_shader_;
  zukou::GlShader color_fragment_shader_;
  zukou::GlSampler sampler_;

  std::unordered_map<std::string, zukou::RenderingUnit *> rendering_unit_map_;
  std::unordered_map<std::string, zukou::GlBaseTechnique *> base_technique_map_;
  std::unordered_map<int, zukou::GlBuffer *> gl_vertex_buffer_map_;
  std::unordered_map<int, zukou::GlTexture *> texture_map_;
  std::unordered_map<std::string, zukou::GlProgram *> program_map_;

  std::deque<glm::mat4> matrix_stack_;
};

}  // namespace zennist
