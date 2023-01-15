#pragma once

#include <zukou.h>

#include <string>
#include <vector>

#include "texture.h"

namespace zennist {

class TextureFactory
{
 public:
  static Texture *Create(zukou::System *system, const char *filename);

 private:
  static std::string ExtractExtensionAsLower(const char *filename);
  static bool MatchAny(std::string &str, std::vector<std::string> &targets);
};

}  // namespace zennist
