#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Bullet.h"

// ’¼i’e
class StraightBullet : public Bullet
{
public:
    StraightBullet(BulletManager* manager);
    ~StraightBullet() override;

    // XVˆ—
    void update(float elapsedTime) override;

    // •`‰æˆ—
    void render(ID3D11DeviceContext* dc) override;

private:
    SkinnedMesh* model = nullptr;
    float speed = 10.0f;
    float lifeTimer = 3.0f; // õ–½i‚R•bj
};