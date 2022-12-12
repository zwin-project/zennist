#include <zukou.h>

#include <iostream>

#include "floor.h"
#include "jpeg-texture.h"
#include "sphere.h"

namespace zennist {

class Application final : public zukou::ISpaceDelegate
{
  glm::vec4 rgbColor(float r, float g, float b)
  {
    return glm::vec4(r / 255, g / 255, b / 255, 1);
  }

 public:
  DISABLE_MOVE_AND_COPY(Application);
  Application()
      : space_(&system_, this),
        floor1_(&system_, &space_),
        floor2_(&system_, &space_),
        floor3_(&system_, &space_),
        bg_(&system_, &space_, 8, false){};

  bool Init()
  {
    if (!system_.Init()) return false;
    if (!space_.Init()) return false;
    if (!floor1_.Render(.3f, glm::mat4(1), rgbColor(49, 69, 95))) return false;
    if (!floor2_.Render(.8f,
            glm::translate(
                glm::rotate(glm::mat4(1), (float)M_PI / 4, glm::vec3(0, 1, 0)),
                glm::vec3(0, -.1f, 0)),
            rgbColor(87, 132, 150)))
      return false;
    if (!floor3_.Render(1.5f,
            glm::translate(
                glm::rotate(glm::mat4(1), (float)M_PI / 2, glm::vec3(0, 1, 0)),
                glm::vec3(0, -.1f, 0)),
            rgbColor(147, 166, 156)))
      return false;

    // auto jpeg_texture = std::make_unique<JpegTexture>(&system_);
    // if (!jpeg_texture->Init()) return false;
    // if (!jpeg_texture->Load(bg_texture_path_)) return false;
    // bg_.Bind(std::move(jpeg_texture));
    // fprintf(stderr, "texture bound\n\n");
    if (!bg_.Render(1000, glm::mat4(1))) return false;

    space_.Commit();

    return true;
  }

  int Run() { return system_.Run(); }

  void Shutdown() override { system_.Terminate(EXIT_SUCCESS); }

 private:
  zukou::System system_;
  zukou::Space space_;

  Floor floor1_;
  Floor floor2_;
  Floor floor3_;
  Sphere bg_;

  // const char* bg_texture_path_ =
  // "/home/cyber/reigo/zennist/assets/bg_teal.jpg";
};

}  // namespace zennist

int
main()
{
  zennist::Application app;

  if (!app.Init()) return EXIT_FAILURE;

  return app.Run();
}
