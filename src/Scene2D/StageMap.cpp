#include <Scene2D/StageMap.hpp>
#include <Siv3D.hpp>
StageMap::StageMap(String path, tson::Vector2i tileset_dimensions,
                   tson::Vector2i tile_dimensions)
    : m_base{path} {
  map_tile_size_x = tile_dimensions.x;
  map_tile_size_y = tile_dimensions.y;
  map_tileset_x = tileset_dimensions.x / map_tile_size_x;
  map_tileset_y = tileset_dimensions.y / map_tile_size_y;
}

TextureRegion StageMap::get(int32 tileIndex) const {
  auto tileAdjusted = tileIndex - 1;
  const int32 x = ((tileAdjusted) % (map_tileset_x)) * map_tile_size_x;
  const int32 y = ((tileAdjusted) / (map_tileset_x)) * map_tile_size_y;
  return m_base(x, y, map_tile_size_x);
}
