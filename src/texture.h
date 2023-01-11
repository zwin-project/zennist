#pragma once

#include <zukou.h>

#include <string>
#include <vector>

namespace zennist {

class Texture : public zukou::GlTexture
{
 protected:
  Texture(zukou::System *system) : zukou::GlTexture(system) {}

 public:
  Texture() = delete;
  virtual ~Texture(){};

  bool Init() { return zukou::GlTexture::Init(); }

  virtual bool Load(const char * /*filename*/) { return false; };
};

}  // namespace zennist
