#include "texture-factory.h"

#include <algorithm>
#include <iostream>

#include "jpeg-texture.h"
#include "png-texture.h"
#include "svg-texture.h"

namespace {
std::vector<std::string> kJpegExtensionList = {"jpg", "jpeg"};
std::vector<std::string> kPngExtensionList = {"png"};
std::vector<std::string> kSvgExtensionList = {"svg"};
}  // namespace

namespace zennist {

Texture *
TextureFactory::Create(zukou::System *system, const char *filename)
{
  Texture *texture = nullptr;

  auto extension = ExtractExtensionAsLower(filename);
  if (MatchAny(extension, kJpegExtensionList))
    texture = new JpegTexture(system);
  else if (MatchAny(extension, kPngExtensionList))
    texture = new PngTexture(system);
  else if (MatchAny(extension, kSvgExtensionList))
    texture = new SvgTexture(system);

  return texture;
}

std::string
TextureFactory::ExtractExtensionAsLower(const char *filename)
{
  std::string extension("");
  std::string name(filename);

  for (int i = name.size() - 2; i >= 0; --i) {
    if (filename[i] == '.') {
      extension = name.substr(i + 1);
      break;
    }
  }

  std::transform(extension.begin(), extension.end(), extension.begin(),
      [](unsigned char c) { return std::tolower(c); });

  return extension;
}

bool
TextureFactory::MatchAny(std::string &str, std::vector<std::string> &targets)
{
  for (auto &target : targets)
    if (str.compare(target) == 0) return true;

  return false;
}

}  // namespace zennist
