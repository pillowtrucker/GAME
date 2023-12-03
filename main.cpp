#include "OpenSiv3D/Siv3D/include/Siv3D.hpp"
#include "Minigames/TheracSimulator/TheracMinigame.hpp"
#include "Scene2D/Scenes/HomeSlum/HomeSlum.hpp"
#include "Scene3D/Scenes/RadiationLab/RadiationLab.hpp"
void Main() {

  int32 fps;
  auto main_monitor = System::GetCurrentMonitor();
  auto const mms = main_monitor.fullscreenResolution;
  Window::SetStyle(WindowStyle::Frameless);
  Window::Resize(mms);
  Window::Maximize();
  while (System::Update()) {
    if (KeyF5.up())
      goto TheracMinigame;
    if (KeyF6.up())
      goto Therac3D;
    if (KeyF7.up())
      goto Therac2D;
    fps = Profiler::FPS();
    Print(fps);
  }

Therac2D:
  GAME::Scene2D::HomeSlum::HomeSlum();
Therac3D :
  GAME::Scene3D::RadiationLab::RadiationLab();
TheracMinigame:
  GAME::Minigames::TheracSimulator::TheracMinigame();
}
