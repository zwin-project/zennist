#include <zukou.h>

#include <cstring>
#include <iostream>
#include <string>

#include "floor-edge.h"
#include "floor.h"
#include "jpeg-texture.h"
#include "landscape.h"
#include "sphere.h"

namespace zennist {

enum class Theme {
  Oka,   // hills
  Nami,  // waves
  Kumo   // clouds
};

glm::vec4
RgbColor(float r, float g, float b)
{
  return glm::vec4(r / 255, g / 255, b / 255, 1);
}

class Application final : public zukou::IExpansiveDelegate
{
 public:
  DISABLE_MOVE_AND_COPY(Application);
  Application(Theme theme)
      : theme_(theme),
        space_(&system_, this),
        landscape1_(&system_, &space_),
        landscape2_(&system_, &space_),
        landscape3_(&system_, &space_),
        floor_(&system_, &space_, 3.f),
        floorUnder_(&system_, &space_, 4.5f),
        floorEdge_(&system_, &space_),
        floorEdgeUnder_(&system_, &space_),
        bg_(&system_, &space_, 8){};

  std::string GetTexturePath(const char* name)
  {
    std::string str = ZENNIST_ASSET_DIR;
    switch (theme_) {
      case Theme::Oka:
        str += "/green_";
        break;
      case Theme::Nami:
        str += "/blue_";
        break;
      case Theme::Kumo:
        str += "/white_";
        break;
      default:
        str += "/green_";
        break;
    }
    str += name;
    str += ".jpg";
    return str;
  }

  bool Init()
  {
    if (!system_.Init()) return false;
    if (!space_.Init()) return false;
    if (!landscape1_.Render(.3f,
            glm::translate(glm::mat4(1), glm::vec3(0, -.4f, 0)),
            GetTexturePath("bg_1").c_str(), 100.0f))
      return false;
    if (!landscape2_.Render(1.f,
            glm::translate(
                glm::rotate(glm::mat4(1), (float)M_PI / 4, glm::vec3(0, 1, 0)),
                glm::vec3(0, -.7f, 0)),
            GetTexturePath("bg_2").c_str(), 200.0f))
      return false;
    if (!landscape3_.Render(2.f,
            glm::translate(
                glm::rotate(glm::mat4(1), (float)M_PI / 2, glm::vec3(0, 1, 0)),
                glm::vec3(0, -.9f, 0)),
            GetTexturePath("bg_3").c_str(), 300.0f))
      return false;

    if (!bg_.Render(990, glm::mat4(1), GetTexturePath("sky_shrink").c_str()))
      return false;
    if (!floor_.Render(1.f, glm::translate(glm::mat4(1), glm::vec3(0, 0, 0))))
      return false;
    if (!floorUnder_.Render(
            1.f, glm::translate(glm::mat4(1), glm::vec3(0, -.2f, 0))))
      return false;
    if (!floorEdge_.Render(
            3.f, glm::translate(glm::mat4(1), glm::vec3(0, .01f, 0))))
      return false;
    if (!floorEdgeUnder_.Render(
            4.5f, glm::translate(glm::mat4(1), glm::vec3(0, -.19f, 0))))
      return false;

    space_.Commit();

    return true;
  }

  int Run() { return system_.Run(); }

  void Shutdown() override { system_.Terminate(EXIT_SUCCESS); }

 private:
  Theme theme_;

  zukou::System system_;
  zukou::Expansive space_;

  Landscape landscape1_;
  Landscape landscape2_;
  Landscape landscape3_;
  Floor floor_;
  Floor floorUnder_;
  FloorEdge floorEdge_;
  FloorEdge floorEdgeUnder_;
  Sphere bg_;
};

}  // namespace zennist

int
main(int argc, char* argv[])
{
  auto theme = zennist::Theme::Nami;
  if (argc >= 2) {
    if (std::strcmp(argv[1], "Oka")) {
      theme = zennist::Theme::Oka;
    }
    if (std::strcmp(argv[1], "Kumo")) {
      theme = zennist::Theme::Kumo;
    }
  }
  zennist::Application app(theme);

  if (!app.Init()) return EXIT_FAILURE;

  return app.Run();
}
