#pragma once
#include <Siv3D.hpp>
#include <tileson/tileson.hpp>
namespace GAME::Scene2D {
class StageMap {
public:
  int32 map_tile_size_x;
  int32 map_tile_size_y;
  int32 map_tileset_x;
  int32 map_tileset_y;

  StageMap(String path, tson::Vector2i tileset_dimensions,
           tson::Vector2i tile_dimensions = tson::Vector2i{16, 16});
    TextureRegion get(int32 tileIndex) const;
    
private:
  Texture m_base;
};
} // namespace GAME::Scene2D
