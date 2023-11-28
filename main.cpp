#include "OpenSiv3D/Siv3D/src/ThirdParty/nlohmann/json.hpp"
#include "Therac3DScene.h"
#include "TheracMinigame.h"
#include "tileson/tileson.hpp"
#include <Siv3D.hpp>
#include <charconv>
#include <codecvt>
#include <entt/entt.hpp>
#include <GammeOntology.h>
#include <Scene2D/StageMap.hpp>
#include "ww898/cp_utf32.hpp"
#include "ww898/cp_utf8.hpp"
#include "ww898/utf_selector.hpp"
#include "ww898/utf_converters.hpp"
#include "TheracConfig.h"
//#include <Siv3D/ThirdParty/parallel_hashmap/phmap_dump.h>
void Main() {
  int32 fps;
  entt::registry registry;
  
  HashTable<String, String> tile_descriptions{
       {U"cheap-carpeting",U"the cheapest fitted carpeting available"}
      ,{U"thin-wall",U"you're not sure whether the building code allows load-bearing plaster walls"}
      ,{U"coin-door",U"a door with a simple coin-operated locking mechanism"}
      ,{U"broken-coin-door",U"the remains of a coin-operated door"}
      ,{U"cheap-mattress",U"an inexpensive, barely-used second-hand mattress"}
      ,{U"toilet",U"a toilet"}
      ,{U"shower",U"a showerhead nozzle embedded in the ceiling"}
      ,{U"the-player",U"You"}
      ,{U"the-bus-stop",U"the bus runs between the hospital and your hovel every 12 hours"}
      ,{U"player-coin-door",U"your very own door with a simple coin-operated locking mechanism"}
      
  };
  
  //tile_descriptions.phmap_dump(U"resources/stage1/tile_descriptions.json");
  //  nlohmann::json tile_descriptions_save{tile_descriptions};
  //  JSON tile_descriptions_save{tile_descriptions};
  std::map<std::string,std::string> ok;
  std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> conv;  
  
  for(auto [k,v]: tile_descriptions) {
      ok[k.toUTF8()] = v.toUTF8();
  }
  nlohmann::json ok2{ok};
  //  JSON ok3{ok};
  //ok2.save(U"resources/stage1/tile_descriptions.json");
  TextWriter w_tile_descs{U"resources/stage1/tile_descriptions.json"};
  w_tile_descs.writeUTF8(ok2.dump(4));
  Deserializer<BinaryReader> reader(U"resources/therac_ui.bin");
  Array<Array<std::pair<String, TheracConfig::TheracTextType>>> ui_widgets;
  reader(ui_widgets);
  
  auto ui_widgets_ = ui_widgets.map([](Array<std::pair<String,TheracConfig::TheracTextType>> v) -> Array<std::pair<std::string,TheracConfig::TheracTextType>>{
      return v.map([](std::pair<String,TheracConfig::TheracTextType> v_){
          return std::pair<std::string,TheracConfig::TheracTextType>{std::get<0>(v_).toUTF8(),std::get<1>(v_)};
      });
  });
  nlohmann::json ok4{ui_widgets_};
  TextWriter w_ui_widgets{U"resources/stage1/therac_ui.json"};
  w_ui_widgets.writeUTF8(ok4.dump(4));
  //  std::cout << ok4.dump();
  //std::cout << tile_descriptions_save.dump(true);
  //tile_descriptions_save.save(U"resources/stage1/tile_descriptions.json");
  //  tile_descriptions_save.save(U"resources/stage1/tile_descriptions.json");
  //JSON tile_descriptions_load{U"resources/stage1/tile_descriptions.json"};
  //HashTable<String,String> tile_descriptions = tile_descriptions_load.get<HashTable<String,String>>();
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

  auto parse_layers = [&registry,&conv,&stage1tileset,&tile_descriptions] (tson::Layer & l,gq::ActualLayerType lt) {
      for(auto &tileobj : l.getObjects()) {
          
          Vec2 coords{tileobj.getPosition().x,tileobj.getPosition().y};
          auto entity = registry.create();
          String tilename = conv.from_bytes(stage1tileset.getTile(tileobj.getGid())->getType());
          String unique_tile_name  = conv.from_bytes(tileobj.getName());
          Optional<String const> tiledesc = none;
          if(!(tile_descriptions.find(tilename) == tile_descriptions.end())){
              tiledesc.emplace(tile_descriptions[tilename]);
          }
          if(unique_tile_name.length() > 0){
              tilename = unique_tile_name;
          }
          if(!(tile_descriptions.find(unique_tile_name) == tile_descriptions.end())){
              tiledesc.emplace(tile_descriptions[unique_tile_name]);
          }
          registry.emplace<gq::Object>(entity,gq::Object{.unique_gid = tileobj.getGid(),.name = tilename, .descr = tiledesc});
          std::variant<std::monostate, Vec2, Float3> extra_coords;
          auto contr = gq::ControllerAI;
          switch(lt) {
          case gq::LayerStatic:
              extra_coords = coords;
              registry.emplace<gq::Fungible>(
                  entity, gq::Fungible{.instance_coords = Array{extra_coords}});
              for (auto [ent, fung, obj] : registry.view<gq::Fungible,gq::Object>().each()) {
                  if (obj.unique_gid == tileobj.getGid() && ent != entity) {
                      fung.instance_coords.push_back(extra_coords);
                      registry.destroy(entity);
      }
    }
              break;

          case gq::LayerActors:
              if (tilename == U"the-player")
                  contr = gq::ControllerHuman;
              registry.emplace<gq::Actor>(entity, contr);
              registry.emplace<gq::Inventory>(entity, Array<entt::entity>{});
              registry.emplace<gq::Coords2D>(entity, coords);
              break;
          case gq::LayerInteractive:
              registry.emplace<gq::Inanimate>(entity);
              registry.emplace<gq::Coords2D>(entity, coords);
              break;
          }
      }
  };

  parse_layers(*stage1->getLayer("Static Tile Ents"), gq::LayerStatic);
  parse_layers(*stage1->getLayer("Interactive Ents"), gq::LayerInteractive);
  parse_layers(*stage1->getLayer("Actor Ents"), gq::LayerActors);
  
  const auto scaling_factor = 4;
  while (System::Update()) {
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
    Window::SetTitle(fps);
  }
Therac3D:
  t3d::Therac3DScene();
Therac:
  TheracMinigame();
}
