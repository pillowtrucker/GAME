#include "OpenSiv3D/Siv3D/src/ThirdParty/nlohmann/json.hpp"
#include "Therac3DScene.h"
#include "TheracMinigame.h"
#include "tileson/tileson.hpp"
#include <Siv3D.hpp>
#include <charconv>

#include <entt/entt.hpp>
#include <GammeOntology.h>
#include <Scene2D/StageMap.hpp>
#include "TheracConfig.h"
#include <Scene2D/TileDescriptions.hpp>
#include <Scene2D/LayerParsing.hpp>

void Main() {
  int32 fps;
  entt::registry registry;

  auto tile_descriptions = scene2d::load_tile_descriptions();
  auto main_monitor = System::GetCurrentMonitor();
  auto const mms = main_monitor.fullscreenResolution;
  Window::SetStyle(WindowStyle::Frameless);
  Window::Resize(mms);
  Window::Maximize();

  tson::Tileson tileson_nlohmann{std::make_unique<tson::NlohmannJson>()};
  auto stage1 = tileson_nlohmann.parse("resources/stage1/stage1.tmj");

  if (stage1->getStatus() == tson::ParseStatus::OK) {

  } else
    System::Exit();
  auto stage1_tile_size = stage1->getTilesets()[0].getTileSize();
  auto stage1_tileset_size = stage1->getTilesets()[0].getImageSize();

  auto stage1tilespath =
      String{stage1->getTilesets()[0].getFullImagePath().generic_u32string()};

  StageMap stage1map{U"resources/{}"_fmt(stage1tilespath), stage1_tileset_size,
                     stage1_tile_size};
  namespace gq = GammeOntology;
  auto stage1tileset = stage1->getTilesets()[0];

  

  scene2d::parse_layers(*stage1->getLayer("Static Tile Ents"), gq::LayerStatic,registry,stage1tileset,tile_descriptions);
  scene2d::parse_layers(*stage1->getLayer("Interactive Ents"), gq::LayerInteractive,registry,stage1tileset,tile_descriptions);
  scene2d::parse_layers(*stage1->getLayer("Actor Ents"), gq::LayerActors,registry,stage1tileset,tile_descriptions);
  
  const auto scaling_factor = 4;
  while (System::Update()) {
    ClearPrint();
    const ScopedRenderStates2D sampler{SamplerState::ClampNearest};
    for (auto [ent, f_d,o_d] : registry.view<gq::Fungible,gq::Object>().each()) {
      for (auto coords : f_d.instance_coords) {
        auto the_coords = std::get<Vec2>(coords);
        stage1map.get(o_d.unique_gid)
            .scaled(scaling_factor)
            .drawAt(the_coords.x * scaling_factor,
                    the_coords.y * scaling_factor);
      }
    }
    for (auto [ent, c_d,o_d] : registry.view<gq::Coords2D,gq::Object,gq::Inanimate>().each()) {
        auto the_coords = c_d.c;
        stage1map.get(o_d.unique_gid)
            .scaled(scaling_factor)
            .drawAt(the_coords.x * scaling_factor,
                    the_coords.y * scaling_factor);
    }
    for (auto [ent, c_d,o_d,a_d] : registry.view<gq::Coords2D,gq::Object,gq::Actor>().each()) {
        auto the_coords = c_d.c;
        auto draw_highlighted = [&stage1map,&o_d,&the_coords](ColorF color){
            ScopedColorAdd2D highlight{color};
            stage1map.get(o_d.unique_gid)
                .scaled(scaling_factor)
                .drawAt(the_coords.x * scaling_factor,
                        the_coords.y * scaling_factor);    
        };
        if(a_d.controller == gq::ControllerHuman)
            draw_highlighted(Palette::Green);
        else
            draw_highlighted(Palette::Blue);
    }
    if (KeyF5.up())
      goto Therac;
    if (KeyF6.up())
      goto Therac3D;
    fps = Profiler::FPS();
    Print(fps);
  }
Therac3D:
  t3d::Therac3DScene();
Therac:
  TheracMinigame();
}
