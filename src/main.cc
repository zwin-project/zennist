#include <zukou.h>

#include <cstring>
#include <iostream>
#include <string>

#include "config.h"
#include "desk.h"
#include "floor-edge.h"
#include "floor.h"
#include "jpeg-texture.h"
#include "landscape.h"
#include "launcher_icons.h"
#include "roof.h"
#include "sphere.h"

namespace {
constexpr char roofModelPath[] = ZENNIST_ASSET_DIR "/roof/zennist_roof.gltf";
// constexpr char deskModelPath[] = ZENNIST_ASSET_DIR "/desk/desk.gltf";
constexpr char deskModelPath[] =
    ZENNIST_ASSET_DIR "/desk_without_icons/desk.gltf";
}  // namespace

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

class Application final : public zukou::IExpansiveDelegate,
                          public zukou::ISystemDelegate
{
 public:
  DISABLE_MOVE_AND_COPY(Application);
  Application(Theme theme, Config* config)
      : theme_(theme),
        config_(config),
        system_(this),
        space_(&system_, this),
        landscape1_(&system_, &space_),
        landscape2_(&system_, &space_),
        landscape3_(&system_, &space_),
        floor_(&system_, &space_, 3.f),
        floorUnder_(&system_, &space_, 4.5f),
        floorEdge_(&system_, &space_),
        floorEdgeUnder_(&system_, &space_),
        bg_(&system_, &space_, 8),
        roof_(&system_, &space_),
        desk_(&system_, &space_),
        launcher_icons_(&system_, &space_){};

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
    if (!roof_.Render(1.f,
            glm::translate(
                glm::rotate(glm::mat4(1), (float)M_PI / 2, glm::vec3(0, 1, 0)),
                glm::vec3(0, -0.5, 0)),
            roofModelPath))
      return false;
    if (!desk_.Render(1.f,
            glm::translate(
                glm::rotate(glm::mat4(1), (float)-M_PI / 2, glm::vec3(0, 1, 0)),
                glm::vec3(0, -0.2, 0)),
            deskModelPath))
      return false;
    if (!launcher_icons_.Render(config_)) return false;

    space_.Commit();

    return true;
  }

  int Run() { return system_.Run(); }

  void Shutdown() override { system_.Terminate(EXIT_SUCCESS); }

  void RayEnter(uint32_t /*serial*/, zukou::VirtualObject* /*virtual_object*/,
      glm::vec3 /*origin*/, glm::vec3 /*direction*/) override
  {}

  void RayLeave(
      uint32_t /*serial*/, zukou::VirtualObject* /*virtual_object*/) override
  {
    launcher_icons_.RayLeave();
  }

  void RayMotion(
      uint32_t /*time*/, glm::vec3 origin, glm::vec3 direction) override
  {
    launcher_icons_.RayMotion(origin, direction);
  }

  void RayButton(uint32_t /*serial*/, uint32_t /*time*/, uint32_t button,
      bool pressed) override
  {
    launcher_icons_.RayButton(button, pressed);
  }

  void RayAxisFrame(const zukou::RayAxisEvent& /*event*/) override{};

 private:
  Theme theme_;
  Config* config_;

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
  Roof roof_;
  Desk desk_;
  LauncherIcons launcher_icons_;
};

}  // namespace zennist

int
main(int argc, char* argv[])
{
  zennist::Config config;

  auto theme = zennist::Theme::Nami;
  if (argc >= 2) {
    if (std::strcmp(argv[1], "Oka")) {
      theme = zennist::Theme::Oka;
    }
    if (std::strcmp(argv[1], "Kumo")) {
      theme = zennist::Theme::Kumo;
    }
  }

  if (!config.Load()) {
    std::cerr << "Failed to load config." << std::endl;
    return EXIT_FAILURE;
  }

  zennist::Application app(theme, &config);

  if (!app.Init()) return EXIT_FAILURE;

  return app.Run();
}
