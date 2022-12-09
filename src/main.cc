#include <zukou.h>

#include <iostream>

#include "floor.h"

namespace zennist {

class Application final : public zukou::ISpaceDelegate
{
 public:
  DISABLE_MOVE_AND_COPY(Application);
  Application() : space_(&system_, this), floor_(&system_, &space_){};

  bool Init()
  {
    if (!system_.Init()) return false;
    if (!space_.Init()) return false;
    if (!floor_.Render()) return false;

    space_.Commit();

    return true;
  }

  int Run() { return system_.Run(); }

  void Shutdown() override { system_.Terminate(EXIT_SUCCESS); }

 private:
  zukou::System system_;
  zukou::Space space_;

  Floor floor_;
};

}  // namespace zennist

int
main()
{
  zennist::Application app;

  if (!app.Init()) return EXIT_FAILURE;

  return app.Run();
}
