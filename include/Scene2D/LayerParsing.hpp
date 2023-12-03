#pragma once
#include "tileson/tileson.hpp"
#include "Common/Ontology/Ontology.hpp"

namespace GAME::Scene2D {
namespace gq = GAME::Common::Ontology;
void parse_layers(tson::Layer & l,gq::ActualLayerType lt, entt::registry& registry,tson::Tileset& tileset,HashTable<String,String>& tile_descriptions);
}
