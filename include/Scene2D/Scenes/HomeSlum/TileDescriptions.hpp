#pragma once
#include "OpenSiv3D/Siv3D/include/Siv3D.hpp"
namespace GAME::Scene2D::Scenes::HomeSlum {
void write_tile_descriptions(HashTable<String,String> tile_descriptions);
HashTable<String,String> load_tile_descriptions();
}
