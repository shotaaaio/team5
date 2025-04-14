#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Collision.h"
#include "Stage.h"

class StageWall :public Stage
{
public:
    StageWall();
    ~StageWall() override;

    // 更新処理
    void update(float elapsedTime) override;

    // 描画処理
    void render(ID3D11DeviceContext* dc) override;

    // レイキャスト
    bool raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

    // 行列更新処理
    void updateTransform();

private:
    SkinnedMesh* model = nullptr;
};