#pragma once

#include <DirectXMath.h>
#include <Effekseer.h>

// エフェクト
class Effect
{
public:
    Effect(const char* filename);
    ~Effect() {}

    // 再生
    Effekseer::Handle play(const DirectX::XMFLOAT3* position, float scale = 1.0f);

    // 停止
    void stop(Effekseer::Handle handle);

    // 座標設定
    void setPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3* position);

    // 拡大縮小設定
    void setScale(Effekseer::Handle handle, const DirectX::XMFLOAT3* scale);

private:
    Effekseer::EffectRef effekseerEffect;
};