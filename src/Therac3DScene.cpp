#include "Therac3DScene.h"
#include "OpenSiv3D/Siv3D/src/Siv3D/Common/Siv3DEngine.hpp"
#include <Siv3D.hpp>
# include <OpenSiv3D/Siv3D/src/Siv3D/System/ISystem.hpp>
# include <Siv3D/Error.hpp>
#include <Siv3D/EngineLog.hpp>
#include <entt/entt.hpp>
namespace t3d
{




// Engine has to be started manually if Siv3D doesn't control main()
void Main()
{
    Siv3DEngine engine;
    SIV3D_ENGINE(System)->init();

    Console << Siv3DEngine::isActive();
    Therac3DScene();
}

void Therac3DScene()
{
    
	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
    const Mesh groundPlane{ MeshData::OneSidedPlane(2000, { 400, 400 }) };
    const Model theracModel{ U"resources/therac-25-base.obj" };
    Model pdp11Model{ U"resources/pdp11.obj" };
    const Model vt100Model{ U"resources/vt100.obj" };
    
    Model::RegisterDiffuseTextures(theracModel, TextureDesc::MippedSRGB);
    Model::RegisterDiffuseTextures(vt100Model, TextureDesc::MippedSRGB);
    Model::RegisterDiffuseTextures(pdp11Model, TextureDesc::MippedSRGB);
    const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };
	DebugCamera3D camera{ Graphics3D::GetRenderTargetSize(), 40_deg, Vec3{ 0, 3, -16 } };
    const Texture groundTexture{ U"resources/PolyPackBoats.png", TextureDesc::MippedSRGB };
    
	while (System::Update())
	{
        camera.update(4.0);
		Graphics3D::SetCameraTransform(camera);

		// [3D シーンの描画]
		{
			const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };


			// [モデルの描画]
			{
				// 地面の描画
				groundPlane.draw(groundTexture);
                

				// 球の描画
                //				Sphere{ { 0, 1, 0 }, 1 }.draw(ColorF{ 0.75 }.removeSRGBCurve());
                vt100Model.draw(Vec3{0,0,0});
                pdp11Model.draw(Vec3{5,0,-5});
                
				theracModel.draw(Vec3{ 2, 0, -2 }, Quaternion::RotateY(180_deg));
			}
		}

		// [RenderTexture を 2D シーンに描画]
		{
			Graphics3D::Flush();
			renderTexture.resolve();
            
			Shader::LinearToScreen(renderTexture);
		}
        }
}
} // namespace t3d
