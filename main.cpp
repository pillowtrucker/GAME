#include "OpenSiv3D/Siv3D/include/Siv3D.hpp"
#include "Minigames/TheracSimulator/TheracMinigame.hpp"
#include "Scene2D/Scenes/HomeSlum/HomeSlum.hpp"
#include "Scene3D/Scenes/RadiationLab/RadiationLab.hpp"
#include "LDtkLoader/include/LDtkLoader/Project.hpp"
void Main() {

  int32 fps;
  auto main_monitor = System::GetCurrentMonitor();
  auto const mms = main_monitor.fullscreenResolution;
  Window::SetStyle(WindowStyle::Frameless);
  Window::Resize(mms);
  Window::Maximize();

  ldtk::Project ldtk_project;
  ldtk_project.loadFromFile("resources/ldtk_test/Therac2D.ldtk");
  auto const & world = ldtk_project.getWorld(ldtk::IID{"cf261870-8990-11ee-bcab-25349806fcb2"});
//  auto const & world = ldtk_project.allWorlds()[0];

  std::cout << world.getName() << std::endl;
  std::cout << world.iid << std::endl;
  
  
  auto const & level0 = world.getLevel("Level_0");


  auto const & bg_layer = level0.getLayer("Static_Tile_Ents");

//  std::cout << bg_layer.getTileset().path << std::endl;
  for (const auto& tile : bg_layer.allTiles()) {
//      std::cout << tile.tileId << std::endl;
  }
        
  while (System::Update()) {
      ClearPrint();
      

    
    
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
