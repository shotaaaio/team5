#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"
#include <wrl/client.h> // ComPtr の定義が入ってる

using Microsoft::WRL::ComPtr;

// タイトルシーン
class SceneClear : public Scene
{
public:
    SceneClear() {}
    ~SceneClear() override {}

    // 初期化処理
    void initialize() override;

    // 終了処理
    void finalize() override;

    // 更新処理
    void update(float elapsedTime) override;

    // 描画処理
    void render() override;

private:
    // タイトル画像
    Sprite* titleImage = nullptr;

    float posY = 0;
};