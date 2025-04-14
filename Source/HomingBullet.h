#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Bullet.h"

// 追尾弾
class HomingBullet : public Bullet
{
public:
    HomingBullet(BulletManager* manager);
    ~HomingBullet() override;

    // 更新処理
    void update(float elapsedTime) override;

    // 描画処理
    void render(ID3D11DeviceContext* dc) override;

    // 追尾のターゲットの設定
    void lockonTarget(DirectX::XMFLOAT3 target)
    {
        this->target = target;

        // ターゲットが変更した後に姿勢が変更されるのでこのタイミングで姿勢行列の更新が必要
        updateTransform();
    }

private:
    SkinnedMesh* model;
    DirectX::XMFLOAT3 target = { 0,0,0 };	// 追尾ターゲット
    float speed = 12.0f; // 移動速度
    float turnSpeed = DirectX::XMConvertToRadians(180); // 最大回転速度
    float lifeTimer = 2.0f; // 寿命(３秒)
};