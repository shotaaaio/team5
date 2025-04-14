#include "PlayerManager.h"
#include "Collision.h"

// XVˆ—
void PlayerManager::update(float elapsedTime)
{
    for (Player* enemy : enemies)
    {
        enemy->update(elapsedTime);
    }

    // ”jŠüˆ—
    for (Player* bullet : removes)
    {
        auto itr = std::find(enemies.begin(), enemies.end(), bullet);
        if (itr != enemies.end())
        {
            // ”jŠü‚·‚×‚« bullet ‚ª bullets “à‚É‘¶İ‚·‚éê‡Aíœ
            delete bullet;
            enemies.erase(itr);
        }
    }
    // ”jŠüƒŠƒXƒg‚ğƒNƒŠƒA
    removes.clear();
    collisionEnemiesAndEnemies();
}

// •`‰æˆ—
void PlayerManager::render(ID3D11DeviceContext* dc)
{
    for (Player* enemy : enemies)
    {
        enemy->render(dc);
    }
}

// “G‚ğ“o˜^
void PlayerManager::regist(Player* enemy)
{
    enemies.emplace_back(enemy);
}

// “G‚Ì‘Síœ
void PlayerManager::clear()
{
    for (Player* enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();
}

void PlayerManager::drawDebugPrimitive()
{
    for (Player* enemy : enemies)
    {
        enemy->drawDrawPrimitive();
    }
}

void PlayerManager::drawDebugGui()
{
    for (Player* enemy : enemies)
    {
        enemy->drawDebugGui();
    }
}

void PlayerManager::collisionEnemiesAndEnemies()
{
    int enemyCount = getEnemyCount();
    for (int i = 0; i < enemyCount; i++)
    {
        Player* EnemyA = enemies.at(i);
        for (int j = i + 1; j < enemyCount; j++)
        {
            Player* EnemyB = enemies.at(j);

            DirectX::XMFLOAT3 outVec;

            if (Collision::intersectCylinderAndCylinder(
                *EnemyA->getPosition(),
                EnemyA->getRadius(),
                EnemyA->getHeight(),
                *EnemyB->getPosition(),
                EnemyB->getRadius(),
                EnemyB->getHeight(),
                outVec))
            {
                // Õ“Ë‚µ‚Ä‚¢‚éê‡AEnemyB‚ÌˆÊ’u‚ğC³
                float radius = EnemyA->getRadius() + EnemyB->getRadius();
                DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), radius);
                vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(EnemyA->getPosition()), vec);
                DirectX::XMFLOAT3 pos;
                DirectX::XMStoreFloat3(&pos, vec);
                pos.y = EnemyB->getPosition()->y;
                EnemyB->setPosition(pos);
            }
        }
    }
}

void PlayerManager::remove(Player* enemy)
{
    auto itr = std::find(removes.begin(), removes.end(), enemy);

    // d•¡‚µ‚Ä‚¢‚½‚ç’Ç‰Á‚µ‚È‚¢
    if (itr != removes.end())
        return;

    // ”jŠüƒŠƒXƒg‚É’Ç‰Á
    removes.push_back(enemy);
}