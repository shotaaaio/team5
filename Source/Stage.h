#pragma once

#include "Collision.h"

class Stage
{
public:
    Stage() {}
    virtual ~Stage() {}

    // 更新処理
    virtual void update(float elapsedTime)=0;

    // 描画処理
    virtual void render(ID3D11DeviceContext* dc)=0;

    // レイキャスト
    virtual bool raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit) = 0;

    // 位置取得
    const DirectX::XMFLOAT3* getPosition() const { return &position; }

    void setPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    const DirectX::XMFLOAT3* getAngle() const { return &angle; }

    void setAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

    const DirectX::XMFLOAT3* getScale() const { return &scale; }

    void setScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

protected:
    DirectX::XMFLOAT3 position = { 0,0,0 };	// 位置
    DirectX::XMFLOAT3 angle = { 0,0,0 };	// 回転
    DirectX::XMFLOAT3 scale = { 1,1,1 };	// 拡大縮小
    // 姿勢行列
    DirectX::XMFLOAT4X4	transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    float radius = 0.5f; // 半径
    float height = 2.0f;	// 高さ
};