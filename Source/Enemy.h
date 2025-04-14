#pragma once

#include <d3d11.h>
#include "Character.h"

// 敵
class Enemy : public Character
{
public:
    Enemy() {}
    ~Enemy() override {}

    // 更新処理
    virtual void update(float elapsedTime) = 0;

    // 描画処理
    virtual void render(ID3D11DeviceContext* dc) = 0;

    // デバッグプリミティブの描画
    virtual void drawDebugPrimitive();

    // 破棄
    void destroy();

public:
    float scaling = 0.002f;
};