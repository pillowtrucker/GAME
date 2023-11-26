# include <Siv3D.hpp>
#include "TheracMinigame.h"
#include "Therac3DScene.h"
#include "OpenSiv3D/Siv3D/src/ThirdParty/nlohmann/json.hpp"
#include "tileson/tileson.hpp"


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

void Main()
{
    int32 fps;
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
    auto toilet = stage1->getLayer("Object Layer 1")->firstObj("the toilet");


        
    while(System::Update())
        {
            const ScopedRenderStates2D sampler{ SamplerState::ClampNearest };
            stage1map.get(toilet->getGid()).scaled(4).draw();

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
