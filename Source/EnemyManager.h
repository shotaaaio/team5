#pragma once

#include <vector>
#include "Enemy.h"

// “GŠÇ—
class EnemyManager
{
private:
    EnemyManager() {}
    ~EnemyManager() {}

    // “G“¯m‚ÌÕ“Ëˆ—
    void collisionEnemiesAndEnemies();

public:
    // singleton
    static EnemyManager* instance()
    {
        static EnemyManager inst;
        return &inst;
    }

    // XVˆ—
    void update(float elapsedTime);

    // •`‰æˆ—
    void render(ID3D11DeviceContext* dc);

    // “G‚Ì“o˜^
    void regist(Enemy* enemy);

    // “G‚Ì”jŠü
    void clear();

    // ƒfƒoƒbƒOƒvƒŠƒ~ƒeƒBƒu•`‰æ
    void drawDebugPrimitive();

    // “G‚Ì‘”‚ğæ“¾
    int getEnemyCount() const { return static_cast<int>(enemies.size()); }

    // “G‚ğæ“¾
    Enemy* getEnemy(int index) { return enemies.at(index); }

    Enemy* searchEnemy(DirectX::XMFLOAT3* position)
    {
        // ˆê”Ô‹ß‚­‚Ì“G‚ğƒ^[ƒQƒbƒg‚Éİ’è
        float dist = FLT_MAX;
        Enemy* closestEnemy = nullptr;
        int enemyCount = getEnemyCount();
        for (int i = 0; i < enemyCount; ++i)
        {
            // “G‚Æ‚Ì‹——£‚ğ”»’è
            Enemy* enemy = getEnemy(i);
            DirectX::XMVECTOR posVec = DirectX::XMLoadFloat3(position);
            DirectX::XMVECTOR eneVec = DirectX::XMLoadFloat3(enemy->getPosition());
            DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(eneVec, posVec);
            DirectX::XMVECTOR lengthSqVec = DirectX::XMVector3LengthSq(vec);
            float lengthSq;
            DirectX::XMStoreFloat(&lengthSq, lengthSqVec);
            if (lengthSq < dist)
            {
                dist = lengthSq;
                closestEnemy = enemy;
            }
        }
        return closestEnemy;
    }

    // “Gíœ
    void remove(Enemy* enemy);

    Enemy* getEnemyByRay(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd);

private:
    std::vector<Enemy*>	enemies;
    std::vector<Enemy*> removes;
};