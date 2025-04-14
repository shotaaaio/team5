#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Collision.h"
#include "Stage.h"

class StageMain :public Stage
{
public:
    StageMain();
    ~StageMain() override;

    // 更新処理
    void update(float elapsedTime) override;

    // 描画処理
    void render(ID3D11DeviceContext* dc) override;

    // レイキャスト
    bool raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) override;

private:
    SkinnedMesh* model = nullptr;
};