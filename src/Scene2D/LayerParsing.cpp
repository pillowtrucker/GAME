#include <Scene2D/LayerParsing.hpp>
namespace scene2d {
//namespace gq = GammeOntology;
void parse_layers(tson::Layer & l,gq::ActualLayerType lt, entt::registry& registry,tson::Tileset& tileset,HashTable<String,String>& tile_descriptions) {
      for(auto &tileobj : l.getObjects()) {
          
          Vec2 coords{tileobj.getPosition().x,tileobj.getPosition().y};
          auto entity = registry.create();
          String tilename = Unicode::FromUTF8(tileset.getTile(tileobj.getGid())->getType());
          String unique_tile_name  = Unicode::FromUTF8(tileobj.getName());
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
}
