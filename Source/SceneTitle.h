#pragma once

#include "Graphics/Sprite.h"
#include "Scene.h"

// タイトルシーン
class SceneTitle : public Scene
{
public:
    SceneTitle() {}
    ~SceneTitle() override {}

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
};