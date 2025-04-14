#pragma once

#include <vector>
#include "Player.h"

class PlayerManager
{
private:
    PlayerManager() {}
    ~PlayerManager() {}

    // “G“¯m‚ÌÕ“Ëˆ—
    void collisionEnemiesAndEnemies();

public:
    // singleton
    static PlayerManager* instance()
    {
        static PlayerManager inst;
        return &inst;
    }

    // XVˆ—
    void update(float elapsedTime);

    // •`‰æˆ—
    void render(ID3D11DeviceContext* dc);

    // “G‚Ì“o˜^
    void regist(Player* enemy);

    // “G‚Ì”jŠü
    void clear();

    // ƒfƒoƒbƒO—p‚Ì•`‰æ
    void drawDebugGui();

    // ƒfƒoƒbƒOƒvƒŠƒ~ƒeƒBƒu•`‰æ
    void drawDebugPrimitive();

    // “G‚Ì‘”‚ğæ“¾
    int getEnemyCount() const { return static_cast<int>(enemies.size()); }

    // “G‚ğæ“¾
    Player* getEnemy(int index) { return enemies.at(index); }

    Player* searchEnemy(DirectX::XMFLOAT3* position)
    {
        // ˆê”Ô‹ß‚­‚Ì“G‚ğƒ^[ƒQƒbƒg‚Éİ’è
        float dist = FLT_MAX;
        Player* closestEnemy = nullptr;
        int enemyCount = getEnemyCount();
        for (int i = 0; i < enemyCount; ++i)
        {
            // “G‚Æ‚Ì‹——£‚ğ”»’è
            Player* enemy = getEnemy(i);
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
    void remove(Player* enemy);

private:
    std::vector<Player*>	enemies;
    std::vector<Player*> removes;
};