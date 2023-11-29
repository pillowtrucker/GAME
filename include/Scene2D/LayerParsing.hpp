#pragma once
#include "tileson/tileson.hpp"
#include <GammeOntology.h>

namespace scene2d {
namespace gq = GammeOntology;
void parse_layers(tson::Layer & l,gq::ActualLayerType lt, entt::registry& registry,tson::Tileset& tileset,HashTable<String,String>& tile_descriptions);
}
