#include <zukou.h>

#include <algorithm>
#include <cstring>
#include <cxxopts.hpp>
#include <iostream>
#include <string>

#include "config.h"
#include "desk.h"
#include "floor-edge.h"
#include "floor.h"
#include "jpeg-texture.h"
#include "landscape.h"
#include "launcher_icons.h"
#include "log.h"
#include "roof.h"
#include "sphere.h"

namespace {
constexpr char roofModelPath[] = ZENNIST_ASSET_DIR "/roof/zennist_roof.gltf";
constexpr char deskModelPath[] = ZENNIST_ASSET_DIR "/desk/desk.gltf";
}  // namespace

namespace zennist {

enum class Theme {
  Nami,  // waves
  Oka,   // hills
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
      case Theme::Nami:
        str += "/blue_";
        break;
      case Theme::Oka:
        str += "/green_";
        break;
      case Theme::Kumo:
        str += "/white_";
        break;
      default:
        str += "/blue_";
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

std::string
Lower(std::string& target)
{
  std::string lower_name = target;
  std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(),
      [](unsigned char c) { return std::tolower(c); });

  return lower_name;
}

bool
Parse(int argc, char* argv[], zennist::Theme* theme)
{
  std::unique_ptr<cxxopts::Options> allocated(
      new cxxopts::Options(argv[0], " - a zen's default desk Expansive"));
  auto& options = *allocated;

  options.add_options()         //
      ("h,help", "Print help")  //
      ("t,theme", "Set theme: Nami(default), Oka, Kumo",
          cxxopts::value<std::string>());

  cxxopts::ParseResult result;
  try {
    result = options.parse(argc, argv);
  } catch (const cxxopts::exceptions::exception& e) {
    std::cerr << "error parsing options: " << e.what() << std::endl;
    std::cerr << std::endl;

    std::cerr << options.help() << std::endl;
    return false;
  }

  if (result.unmatched().size() > 0) {
    std::cerr << "Unmatched options: ";
    for (const auto& option : result.unmatched()) {
      std::cerr << "'" << option << "' ";
    }
    std::cerr << std::endl;
    std::cerr << std::endl;

    std::cerr << options.help() << std::endl;
    return false;
  }

  if (result.count("help")) {
    std::cout << options.help() << std::endl;
    return false;
  }

  *theme = zennist::Theme::Nami;
  if (result.count("t")) {
    auto theme_name = result["t"].as<std::string>();
    auto lower_theme_name = Lower(theme_name);

    if (std::strcmp(lower_theme_name.c_str(), "oka") == 0)
      *theme = zennist::Theme::Oka;
    else if (std::strcmp(lower_theme_name.c_str(), "kumo") == 0)
      *theme = zennist::Theme::Kumo;
    else if (std::strcmp(lower_theme_name.c_str(), "nami") == 0)
      *theme = zennist::Theme::Nami;
    else {
      std::cerr << "Unknown theme is specified.: " << theme_name << std::endl;
      std::cerr << std::endl;

      std::cerr << options.help() << std::endl;
      return false;
    }
  }

  return true;
}

int
main(int argc, char* argv[])
{
  zennist::Config config;
  zennist::Theme theme;

  if (!Parse(argc, argv, &theme)) return EXIT_FAILURE;

  if (!config.Load()) {
    ZennistError("Failed to load config.");
    return EXIT_FAILURE;
  }

  zennist::Application app(theme, &config);

  if (!app.Init()) return EXIT_FAILURE;

  return app.Run();
}
