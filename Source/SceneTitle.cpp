#include "SceneTitle.h"
#include "DeviceManager.h"
#include "Graphics/GraphicsManager.h"
#include "GameScene.h"
#include "SceneManager.h"
#include "Input/InputManager.h"
#include "LoadingScene.h"

// 初期化処理
void SceneTitle::initialize()
{
    // タイトル画像読み込み
    titleImage = new Sprite(DeviceManager::instance()->getDevice(), L".\\Resources\\Image\\Chat1.png");
}

// 終了処理
void SceneTitle::finalize()
{
    if (titleImage != nullptr)
    {
        delete titleImage;
        titleImage = nullptr;
    }
}

// 更新処理
void SceneTitle::update(float elapsedTime)
{
    GamePad* gamePad = InputManager::instance()->getGamePad();

    // ボタンを押したらゲームシーンに遷移
    if (gamePad->getButtonDown() & GamePad::BTN_Z)
    {
        SceneManager::instance()->changeScene(new LoadingScene(new GameScene));
    }
}

// 描画処理
void SceneTitle::render()
{
    DeviceManager* mgr = DeviceManager::instance();
    GraphicsManager* graphics = GraphicsManager::instance();

    ID3D11DeviceContext* dc = mgr->getDeviceContext();
    ID3D11RenderTargetView* rtv = mgr->getRenderTargetView();
    ID3D11DepthStencilView* dsv = mgr->getDepthStencilView();

    // 画面クリア＆レンダーターゲット設定
    FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
    dc->ClearRenderTargetView(rtv, color);
    dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    dc->OMSetRenderTargets(1, &rtv, dsv);

    // 2D 描画設定
    graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
        // サンプラーステートの設定（リニア）
        dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
        // ブレンドステートの設定（アルファ）
        dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
        // 深度ステンシルステートの設定（深度テストオフ、深度書き込みオフ）
        dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 0);
        // ラスタライザステートの設定（ソリッド、裏面表示オフ）
        dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
        });

    // 2D 描画
    {
        titleImage->render(dc,
            0, 0, mgr->getScreenWidth(), mgr->getScreenHeight(),
            1, 1, 1, 1,
            0,
            0, 0, 1024, 1024
        );
    }
}