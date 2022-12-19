//
// Created by arrayJY on 2022/12/08.
//

#ifndef SPARROWENGINE_ENGINE_H
#define SPARROWENGINE_ENGINE_H

namespace Sparrow {
class Engine {
 public:
  void startEngine();
  void tick(float deltaTime);
  void shutdown();

 private:
  float calcOneFrameDeltaTime() const;
  void mainLoop();

  void logicalTick(float deltaTime);
  void renderTick(float deltaTime);
};

}  // namespace Sparrow

#endif