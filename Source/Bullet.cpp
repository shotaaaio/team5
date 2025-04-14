#include "Bullet.h"
#include "BulletManager.h"
#include "Graphics/GraphicsManager.h"

// コンストラクタ
Bullet::Bullet(BulletManager* manager)
    : manager(manager)
{
    manager->regist(this);
}

// デバッグプリミティブ描画
void Bullet::drawDebugPrimitive()
{
    DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

    // 衝突判定用のデバッグ用球を描画
    debugRenderer->drawSphere(position, radius, { 0, 0, 0, 1 });
}

// 行列更新処理
void Bullet::updateTransform()
{
    DirectX::XMVECTOR frontVec, upVec, rightVec;

    // 現在の方向を正規化し前方向ベクトルを計算
    frontVec = DirectX::XMLoadFloat3(&direction);
    frontVec = DirectX::XMVector3Normalize(frontVec);

    // 仮の上方向ベクトルを計算
    upVec = DirectX::XMVectorSet(0.001f, 1, 0, 0);
    upVec = DirectX::XMVector3Normalize(upVec);

    // 外積を利用して右方向ベクトルを計算
    rightVec = DirectX::XMVector3Cross(upVec, frontVec);
    rightVec = DirectX::XMVector3Normalize(rightVec);

    // 外積を利用して正しい上方向ベクトルを計算
    upVec = DirectX::XMVector3Cross(frontVec, rightVec);

    // 計算結果を XMFLOAT3 に変換
    DirectX::XMFLOAT3 right, up, front;
    DirectX::XMStoreFloat3(&right, rightVec);
    DirectX::XMStoreFloat3(&up, upVec);
    DirectX::XMStoreFloat3(&front, frontVec);

    // 計算した軸ベクトルから姿勢行列を作成
    transform._11 = right.x * scale.x;
    transform._12 = right.y * scale.x;
    transform._13 = right.z * scale.x;
    transform._14 = 0.0f;

    transform._21 = up.x * scale.y;
    transform._22 = up.y * scale.y;
    transform._23 = up.z * scale.y;
    transform._24 = 0.0;

    transform._31 = front.x * scale.z;
    transform._32 = front.y * scale.z;
    transform._33 = front.z * scale.z;
    transform._34 = 0.0f;

    transform._41 = position.x;
    transform._42 = position.y;
    transform._43 = position.z;
    transform._44 = 1.0f;

    // 発射方向
    this->direction = front;
}

// 破棄
void Bullet::destroy()
{
    manager->remove(this);
}