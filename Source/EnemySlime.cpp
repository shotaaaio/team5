#include "EnemySlime.h"
#include "DeviceManager.h"
#include "Input/InputManager.h"
#include "StraightBullet.h"
#include "HomingBullet.h"
#include "PlayerManager.h"
#include "Player.h"

EnemySlime::EnemySlime()
{
    model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\Cube\\plantune.fbx");

    // モデルが大きいのでスケール調整
    const float scale_fcator = 0.001f;
    scale = { scale_fcator, scale_fcator, scale_fcator };

    radius = 0.5f;  // 幅
    height = 1.0f;  // 高さ
}

EnemySlime::~EnemySlime()
{
    if (model != nullptr)
    {
        delete model;
        model = nullptr;
    }
}

// 更新処理
void EnemySlime::update(float elapsedTime)
{
    // 速度処理更新
    updateVelocity(elapsedTime);

    // 無敵時間の更新
    updateInvincibleTimer(elapsedTime);

    collisionBulletsAndEnemies();

    //inputLaunchBullet();

    // 弾更新処理
    bulletMgr.update(elapsedTime);

    // ワールド行列の更新
    updateTransform();
}

// 描画処理
void EnemySlime::render(ID3D11DeviceContext* dc)
{
    model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, nullptr);

    // 弾デバッグプリミティブ描画
    bulletMgr.drawDebugPrimitive();
}

void EnemySlime::onDead()
{
    // 敵の破棄
    destroy();
}

void EnemySlime::collisionBulletsAndEnemies()
{
    PlayerManager* enemyMgr = PlayerManager::instance();

    // 全ての弾と全ての敵を総当たりで衝突処理
    int bulletCount = bulletMgr.getBulletCount();
    int enemyCount = enemyMgr->getEnemyCount();
    for (int i = 0; i < bulletCount; ++i)
    {
        Bullet* bullet = bulletMgr.getBullet(i);

        for (int j = 0; j < enemyCount; ++j)
        {
            Player* enemy = enemyMgr->getEnemy(j);

            // 衝突処理
            DirectX::XMFLOAT3 outVec;
            if (Collision::intersectSphereAndCylinder(
                *bullet->getPosition(),
                bullet->getRadius(),
                *enemy->getPosition(),
                enemy->getRadius(),
                enemy->getHeight(),
                outVec))
            {
                int damage = 3; // 仮のダメージとして１
                if (enemy->applyDamage(damage, 0.1f))
                {
                    // 吹き飛ばし
                    float power = 10.0f;
                    DirectX::XMFLOAT3 impulse;
                    impulse.x = outVec.x * power;
                    impulse.y = power * 0.5f;
                    impulse.z = outVec.z * power;
                    enemy->addImpulse(impulse);

                    // ヒットエフェクトの再生
                    DirectX::XMFLOAT3 enePos = *enemy->getPosition();
                    enePos.y += enemy->getHeight() * 0.5f;
                    //Effekseer::Handle handle = hitEffect->play(&enePos, 0.5f);

                    // 弾の破棄
                    bullet->destroy();
                }
            }
        }
    }
}

void EnemySlime::inputLaunchBullet()
{
    GamePad* gamePad = InputManager::instance()->getGamePad();

    // ストレート弾発射
    if (gamePad->getButtonDown() & GamePad::BTN_F)
    {
        // 前方向
        DirectX::XMFLOAT3 dir;
        dir.x = sinf(angle.y);
        dir.y = 0.0f;
        dir.z = cosf(angle.y);

        // 発射位置（プレイヤーの腰あたり
        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height * 0.5f;
        pos.z = position.z;

        // 発射
        StraightBullet* bullet = new StraightBullet(&bulletMgr);
        bullet->setDirection(dir);
        bullet->setPosition(pos);
    }

    // ホーミング弾発射
    if (gamePad->getButtonDown() & GamePad::BTN_V)
    {
        // 前方向
        DirectX::XMFLOAT3 dir;
        dir.x = sinf(angle.y);
        dir.y = 0.0f;
        dir.z = cosf(angle.y);

        // 発射位置（プレイヤーの腰あたり
        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height * 0.5f;
        pos.z = position.z;

        // ターゲット
        DirectX::XMFLOAT3 target;
        target.x = pos.x + dir.x * 1000.0f;
        target.y = pos.y + dir.y * 1000.0f;
        target.z = pos.z + dir.z * 1000.0f;

        // 一番近くの敵を取得
        Player* enemy = PlayerManager::instance()->searchEnemy(&position);
        if (enemy != nullptr)
        {
            DirectX::XMStoreFloat3(&dir, DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(enemy->getPosition()), DirectX::XMLoadFloat3(&position)));
        }

        // 発射
        StraightBullet* bullet = new StraightBullet(&bulletMgr);
        bullet->setDirection(dir);
        bullet->setPosition(pos);
    }
}