#include "roof.h"

namespace zennist {

Roof::Roof(zukou::System* system, zukou::VirtualObject* virtual_object)
    : viewer_(system, virtual_object)
{}

bool
Roof::Render(float radius, glm::mat4 transform, const char* model_path)
{
  return viewer_.Render(radius, transform, model_path);
}

}  // namespace zennist
