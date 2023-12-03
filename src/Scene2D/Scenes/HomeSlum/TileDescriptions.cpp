#include "OpenSiv3D/Siv3D/src/ThirdParty/nlohmann/json.hpp"
#include <Scene2D/Scenes/HomeSlum/TileDescriptions.hpp>

namespace GAME::Scene2D::Scenes::HomeSlum {
void write_tile_descriptions(HashTable<String,String> tile_descriptions) {
std::map<std::string,std::string> ok;
  
  for(auto [k,v]: tile_descriptions) {
      ok[k.toUTF8()] = v.toUTF8();
  }
  nlohmann::json ok2{ok};

  TextWriter w_tile_descs{U"resources/stage1/tile_descriptions.json"};
  w_tile_descs.writeUTF8(ok2.dump(4));
  w_tile_descs.close();
}

HashTable<String,String> load_tile_descriptions() {  
  TextReader r_tile_descs{U"resources/stage1/tile_descriptions.json"};
  nlohmann::json tile_descs_ = nlohmann::json::parse(r_tile_descs.readAll().toUTF8());

  std::map<std::string,std::string> tile_descs__ = tile_descs_[0];
  HashTable<String,String> tile_descs;
  for(auto [k,v]: tile_descs__) {
      tile_descs.emplace(Unicode::FromUTF8(k),Unicode::FromUTF8(v));
   }
  r_tile_descs.close();
  return tile_descs;
}
}
