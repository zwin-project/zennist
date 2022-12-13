#include <zukou.h>

#include <iostream>

#include "landscape.h"
#include "jpeg-texture.h"
#include "sphere.h"

namespace zennist {

class Application final : public zukou::IExpansiveDelegate
{
  glm::vec4 rgbColor(float r, float g, float b)
  {
    return glm::vec4(r / 255, g / 255, b / 255, 1);
  }

 public:
  DISABLE_MOVE_AND_COPY(Application);
  Application()
      : space_(&system_, this),
        landscape1_(&system_, &space_),
        landscape2_(&system_, &space_),
        landscape3_(&system_, &space_),
        bg_(&system_, &space_, 8, false)
        {};

  bool Init()
  {
    if (!system_.Init()) return false;
    if (!space_.Init()) return false;
    if (!landscape1_.Render(.3f, glm::mat4(1), rgbColor(40, 57, 64), rgbColor(37, 50, 66))) return false;
    if (!landscape2_.Render(.8f,
            glm::translate(
                glm::rotate(glm::mat4(1), (float)M_PI / 4, glm::vec3(0, 1, 0)),
                glm::vec3(0, -.1f, 0)),
            rgbColor(76, 115, 131),
            rgbColor(95, 113, 122))
    ) return false;
    if (!landscape3_.Render(1.5f,
            glm::translate(
                glm::rotate(glm::mat4(1), (float)M_PI / 2, glm::vec3(0, 1, 0)),
                glm::vec3(0, -.1f, 0)),
            rgbColor(148, 167, 156),
            rgbColor(123, 146, 147)))
      return false;

    if (!bg_.Render(990, glm::mat4(1))) return false;

    space_.Commit();

    return true;
  }

  int Run() { return system_.Run(); }

  void Shutdown() override { system_.Terminate(EXIT_SUCCESS); }

 private:
  zukou::System system_;
  zukou::Expansive space_;

  Landscape landscape1_;
  Landscape landscape2_;
  Landscape landscape3_;
  Sphere bg_;
};

}  // namespace zennist

int
main()
{
  zennist::Application app;

  if (!app.Init()) return EXIT_FAILURE;

  return app.Run();
}
