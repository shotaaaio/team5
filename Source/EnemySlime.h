#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Enemy.h"
#include "BulletManager.h"

// ƒXƒ‰ƒCƒ€
class EnemySlime : public Enemy
{
public:
    EnemySlime();
    ~EnemySlime() override;

    // XVˆ—
    void update(float elapsedTime) override;

    // •`‰æˆ—
    void render(ID3D11DeviceContext* dc) override;

    // €–S‚µ‚½‚Æ‚«‚ÉŒÄ‚Î‚ê‚é
    void onDead() override;

    void collisionBulletsAndEnemies();

    void inputLaunchBullet();

private:
    SkinnedMesh* model = nullptr;
    // ’eŠÇ—
    BulletManager bulletMgr;
};