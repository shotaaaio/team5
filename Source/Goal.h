#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Obj.h"

// ツタ
class Goal : public Obj
{
public:
    Goal();
    ~Goal() override;

    // 更新処理
    void update(float elapsedTime) override;

    // 描画処理
    void render(ID3D11DeviceContext* dc) override;

private:
    SkinnedMesh* model = nullptr;
};