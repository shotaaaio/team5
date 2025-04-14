#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX11.h>

// エフェクト管理
class EffectManager
{
private:
    EffectManager() {}
    ~EffectManager() {}

public:
    static EffectManager* instance()
    {
        static EffectManager inst;
        return &inst;
    }

    // 初期化処理
    void initialize();

    // 終了処理
    void finalize();

    // 更新処理
    void update(float elapsedTime);

    // 描画処理
    void render(const DirectX::XMFLOAT4X4* view, const DirectX::XMFLOAT4X4* projection);

    // Effekseer 管理の取得
    Effekseer::ManagerRef getEffekseerMgr() { return effekseerMgr; }

private:
    // Effekseer 管理
    Effekseer::ManagerRef effekseerMgr;
    // Effekseer 描画周り
    EffekseerRenderer::RendererRef effekseerRenderer;
};