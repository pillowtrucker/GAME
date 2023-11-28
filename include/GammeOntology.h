#include <entt/entt.hpp>
#include <Siv3D.hpp>
namespace GammeOntology {
// eg wall, floor, every static tile
struct Fungible {
  Array<std::variant<std::monostate, Vec2, Float3>>
      instance_coords; // where in the world things like this reside
    // because Tiled is an unbelievably bad program this is in pixels
    // If I stored them in a "tile layer" then I would have to MIX pixels and positions in grid, but there's no good reason to use the "tile layer" mode other than being naive
};
struct Object {
    uint32_t const unique_gid;
    String const name; // instead of storing aliases use other properties ?
    Optional<String const>
    descr; // basic description to be expanded by other properties
};
struct Container {
    std::stack<entt::entity> contents;
};
struct Inanimate{};
struct Inventory {
  Array<entt::entity> contents;
};
enum ActorController {
    ControllerAI,
    ControllerHuman
};
struct Actor {
    ActorController controller;
};
struct Coords2D {
  Vec2 c;
};
struct Coords3D {
  Float3 c;
};
enum ActualLayerType {
    LayerStatic,
    LayerInteractive,
    LayerActors
};
} // namespace GammeOntology
