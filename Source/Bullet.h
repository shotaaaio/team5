#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

// 弾管理の前方宣言
class BulletManager;

// 弾
class Bullet
{
public:
    Bullet(BulletManager* manager);
    virtual ~Bullet() {}

    // 更新処理
    virtual void update(float elapsedTime) = 0;

    // 描画処理
    virtual void render(ID3D11DeviceContext* dc) = 0;

    // デバッグプリミティブ描画
    virtual void drawDebugPrimitive();

    // 位置取得
    const DirectX::XMFLOAT3* getPosition() const { return &position; }

    // 位置の設定
    void setPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    // 方向取得
    const DirectX::XMFLOAT3* getDirection() const { return &direction; }

    // 方向の設定
    void setDirection(const DirectX::XMFLOAT3& direction) { this->direction = direction; }

    // 拡大縮小取得
    const DirectX::XMFLOAT3* getScale() const { return &scale; }

    // 半径取得
    float getRadius() const { return radius; }

    // 破棄
    void destroy();

protected:
    // 行列更新処理
    void updateTransform();

protected:
    DirectX::XMFLOAT3	position = { 0,0,0 };	// 位置
    DirectX::XMFLOAT3	direction = { 0,0,1 };	// 方向
    DirectX::XMFLOAT3	scale = { 1,1,1 };		// 拡大縮小
    // 姿勢行列
    DirectX::XMFLOAT4X4	transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    // 弾管理
    BulletManager* manager;

    float radius = 0.5f;	// 半径
};