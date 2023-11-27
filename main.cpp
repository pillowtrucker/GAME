# include <Siv3D.hpp>
#include "TheracMinigame.h"
#include "Therac3DScene.h"
#include "OpenSiv3D/Siv3D/src/ThirdParty/nlohmann/json.hpp"
#include "tileson/tileson.hpp"
#include <entt/entt.hpp>

class StageMap
{
public:

	
	int32 map_tile_size_x;
    int32 map_tile_size_y;
    int32 map_tileset_x;
    int32 map_tileset_y;

	StageMap(String path, tson::Vector2i tileset_dimensions, tson::Vector2i tile_dimensions = tson::Vector2i{16,16})
		: m_base{ path}
    {
        map_tile_size_x = tile_dimensions.x;
        map_tile_size_y = tile_dimensions.y;
        map_tileset_x = tileset_dimensions.x/map_tile_size_x;
        map_tileset_y = tileset_dimensions.y/map_tile_size_y;
    }

	
    	TextureRegion get(int32 tileIndex) const
	{
        auto tileAdjusted = tileIndex -1;
		const int32 x = ((tileAdjusted) % (map_tileset_x)) * map_tile_size_x;
		const int32 y = ((tileAdjusted) / (map_tileset_x)) * map_tile_size_y;
       
		return m_base(x, y,map_tile_size_x);
	}
    
private:

	Texture m_base;
};

namespace GammeOntology
{
// eg wall, floor, every static tile
struct Fungible
{
//    const char32 * obj_type; // should be looked up by id in a map
    std::variant<uint32_t,const char32 *> unique_gid;
    Array<std::variant<std::monostate,Vec2,Float3>> instance_coords; // where in the world things like this reside
};
struct Object
{
    const char32 * name; // instead of storing aliases use other properties ?
    Optional<const char32 *> descr; //basic description to be expanded by other properties
};
struct Container
{
    std::stack<entt::any> contents;
};
struct Inventory
{
    Array<entt::any> contents;
};
struct TooDeeCoords
{
    Vec2 c;
};
struct ThreeDeeCoords
{
    Float3 c;
};
} // namespace GammeOntology


void Main()
{
    int32 fps;
    entt::registry registry;
    auto main_monitor = System::GetCurrentMonitor();
    auto const mms = main_monitor.fullscreenResolution;
    Window::SetStyle(WindowStyle::Frameless);
    Window::Resize(mms);
    Window::Maximize();

    //    auto map_stage1 = JSON::Load(U"resources/stage1.tmj");
    //    Console << map_stage1;
    tson::Tileson tileson_nlohmann {std::make_unique<tson::NlohmannJson>()};
    auto stage1 = tileson_nlohmann.parse("resources/stage1.tmj");

    if(stage1->getStatus() == tson::ParseStatus::OK)
        {
            //std::cout << stage1->getTiledVersion();
        }
    else System::Exit();
    auto stage1_tile_size = stage1->getTilesets()[0].getTileSize();
    auto stage1_tileset_size = stage1->getTilesets()[0].getImageSize();

    auto stage1tilespath = String{stage1->getTilesets()[0].getFullImagePath().generic_u32string()};
    Console << stage1tilespath;


    StageMap stage1map{U"resources/{}"_fmt(stage1tilespath),stage1_tileset_size,stage1_tile_size};
    namespace gq = GammeOntology;
    auto static_layer = stage1->getLayer("Static Tile Ents");
            
                
            for(auto& tileobj: static_layer->getObjects())
            {
                std::variant<std::monostate,Vec2,Float3> extra_coords;
                Vec2 new_coord{tileobj.getPosition().x,tileobj.getPosition().y};
                extra_coords = new_coord;                
                auto entity = registry.create();
                registry.emplace<gq::Fungible>(entity,gq::Fungible{.unique_gid = tileobj.getGid(),.instance_coords = Array{extra_coords}});        
                for(auto [ent,fung] :registry.view<gq::Fungible>().each())
                {
                    Console << new_coord;
                    if(std::get<uint32_t>(fung.unique_gid) == tileobj.getGid() && ent != entity)
                    {
                        fung.instance_coords.push_back(extra_coords);
                        registry.destroy(entity);
                    }
                    
                
                }
            }
    
    const auto scaling_factor = 4;
    while(System::Update())
        {
            const ScopedRenderStates2D sampler{ SamplerState::ClampNearest };
            
                
            for(auto [ent, data]: registry.view<gq::Fungible>().each())
            {
                for(auto coords: data.instance_coords)
                {
                    auto the_coords = std::get<Vec2>(coords);
//                    Console << the_coords;
                    stage1map.get(std::get<uint32_t>(data.unique_gid)).scaled(scaling_factor).drawAt(the_coords.x*scaling_factor,the_coords.y*scaling_factor);
                }
                
            }

            auto interactive_layer = stage1->getLayer("Interactive Ents");
            for(auto& tileobj: interactive_layer->getObjects())
            {
                stage1map.get(tileobj.getGid()).scaled(scaling_factor).drawAt(tileobj.getPosition().x*scaling_factor,tileobj.getPosition().y*scaling_factor);
            }
            auto actor_layer = stage1->getLayer("Actor Ents");
            for(auto& tileobj: actor_layer->getObjects())
            {
                ScopedColorAdd2D recolor{Palette::Green};
                stage1map.get(tileobj.getGid()).scaled(scaling_factor).drawAt(tileobj.getPosition().x*scaling_factor,tileobj.getPosition().y*scaling_factor);
            }

            
            
            if(KeyF5.up())
                goto Therac;
            if(KeyF6.up())
                goto Therac3D;
            fps = Profiler::FPS();
            Window::SetTitle(fps);
        }
Therac3D: t3d::Therac3DScene();
 Therac : TheracMinigame();

}
