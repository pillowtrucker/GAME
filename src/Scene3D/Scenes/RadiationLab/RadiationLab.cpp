#include "Scene3D/Scenes/RadiationLab/RadiationLab.hpp"
#include "OpenSiv3D/Siv3D/src/Siv3D/Common/Siv3DEngine.hpp"
#include <OpenSiv3D/Siv3D/src/Siv3D/System/ISystem.hpp>
#include "OpenSiv3D/Siv3D/include/Siv3D.hpp"
#include "OpenSiv3D/Siv3D/include/Siv3D/EngineLog.hpp"
#include "OpenSiv3D/Siv3D/include/Siv3D/Error.hpp"
#include <entt/entt.hpp>
namespace GAME::Scene3D::RadiationLab {

// Engine has to be started manually if Siv3D doesn't control main()
void Main() {
  Siv3DEngine engine;
  SIV3D_ENGINE(System)->init();

  Console << Siv3DEngine::isActive();
  RadiationLab();
}

void RadiationLab() {

  const ColorF backgroundColor = ColorF{0.4, 0.6, 0.8}.removeSRGBCurve();
  const Mesh groundPlane{MeshData::OneSidedPlane(2000, {400, 400})};
  const Model theracModel{U"resources/therac-25-base.obj"};
  Model pdp11Model{U"resources/pdp11.obj"};
  const Model vt100Model{U"resources/vt100.obj"};

  Model::RegisterDiffuseTextures(theracModel, TextureDesc::MippedSRGB);
  Model::RegisterDiffuseTextures(vt100Model, TextureDesc::MippedSRGB);
  Model::RegisterDiffuseTextures(pdp11Model, TextureDesc::MippedSRGB);
  const MSRenderTexture renderTexture{
      Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes};
  DebugCamera3D camera{Graphics3D::GetRenderTargetSize(), 40_deg,
                       Vec3{0, 3, -16}};
  const Texture groundTexture{U"resources/PolyPackBoats.png",
                              TextureDesc::MippedSRGB};
  int fps;
  while (System::Update()) {
    ClearPrint();
    camera.update(4.0);
    Graphics3D::SetCameraTransform(camera);

    {
      const ScopedRenderTarget3D target{renderTexture.clear(backgroundColor)};

      {
        groundPlane.draw(groundTexture);

        vt100Model.draw(Vec3{0, 0, 0});
        pdp11Model.draw(Vec3{5, 0, -5});

        theracModel.draw(Vec3{2, 0, -2}, Quaternion::RotateY(180_deg));
      }
    }

    {
      Graphics3D::Flush();
      renderTexture.resolve();

      Shader::LinearToScreen(renderTexture);
    }
    fps = Profiler::FPS();
    Print(fps);
  }
}
} // namespace t3d
