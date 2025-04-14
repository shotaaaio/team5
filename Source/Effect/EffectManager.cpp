#include "DeviceManager.h"
#include "EffectManager.h"

// 初期化処理
void EffectManager::initialize()
{
    DeviceManager* deviceMgr = DeviceManager::instance();

    // Effekseer レンダラを生成
    effekseerRenderer = EffekseerRendererDX11::Renderer::Create(deviceMgr->getDevice(),
        deviceMgr->getDeviceContext(), 2048);

    // Effekseer 管理生成
    effekseerMgr = Effekseer::Manager::Create(2048);

    // Effekseer 描画周りの各種設定（特殊なカスタマイズをしない場合は以下の設定）
    effekseerMgr->SetSpriteRenderer(effekseerRenderer->CreateSpriteRenderer());
    effekseerMgr->SetRibbonRenderer(effekseerRenderer->CreateRibbonRenderer());
    effekseerMgr->SetRingRenderer(effekseerRenderer->CreateRingRenderer());
    effekseerMgr->SetTrackRenderer(effekseerRenderer->CreateTrackRenderer());
    effekseerMgr->SetModelRenderer(effekseerRenderer->CreateModelRenderer());
    // Effekseer 内での Loader の設定（特殊なカスタマイズをしない場合は以下の設定）
    effekseerMgr->SetTextureLoader(effekseerRenderer->CreateTextureLoader());
    effekseerMgr->SetModelLoader(effekseerRenderer->CreateModelLoader());
    effekseerMgr->SetMaterialLoader(effekseerRenderer->CreateMaterialLoader());

    // Effekseer を左手座標系で計算する
    effekseerMgr->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);
}

// 更新処理
void EffectManager::update(float elapsedTime)
{
    // エフェクト更新処理（フレーム単位）
    effekseerMgr->Update(elapsedTime * 60.0f);
}

// 描画処理
void EffectManager::render(const DirectX::XMFLOAT4X4* view, const DirectX::XMFLOAT4X4* projection)
{
    // ビュー＆プロジェクション行列を Effekseer レンダラに設定
    effekseerRenderer->SetCameraMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(view));
    effekseerRenderer->SetProjectionMatrix(*reinterpret_cast<const Effekseer::Matrix44*>(projection));

    // Efekseer の描画を開始
    effekseerRenderer->BeginRendering();

    // Effekseer の描画を実行
    // この effekseerMgr から生成されるエフェクトはここで一括描画される
    effekseerMgr->Draw();

    // Effekseer の描画を終了
    effekseerRenderer->EndRendering();
}