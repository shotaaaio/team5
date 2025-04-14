#include "StraightBullet.h"
#include "DeviceManager.h"

// コンストラクタ
StraightBullet::StraightBullet(BulletManager* manager)
    : Bullet(manager)
{
    model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\Sword\\Sword.cereal");

    const float scale_fcator = 3.0f;	// モデルが大きいのでスケール調整
    scale = { scale_fcator, scale_fcator, scale_fcator };
}

// デストラクタ
StraightBullet::~StraightBullet()
{
    if (model != nullptr)
    {
        delete model;
        model = nullptr;
    }
}

// 更新処理
void StraightBullet::update(float elapsedTime)
{
    // 経過時間を引いていく
    lifeTimer -= elapsedTime;
    if (lifeTimer <= 0.0f)
    {
        // 時間が 0 以下になったら自分を破棄
        destroy();
    }

    // 移動
    float speed = this->speed * elapsedTime;

    // 位置 += 方向 * 速さ
    position.x += direction.x * speed;
    position.y += direction.y * speed;
    position.z += direction.z * speed;

    // ワールド行列の更新
    updateTransform();
}

// 描画処理
void StraightBullet::render(ID3D11DeviceContext* dc)
{
    model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);
}