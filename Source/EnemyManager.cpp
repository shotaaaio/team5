#include "EnemyManager.h"
#include "Collision.h"

// çXêVèàóù
void EnemyManager::update(float elapsedTime)
{
    for (Enemy* enemy : enemies)
    {
        enemy->update(elapsedTime);
    }

    // îjä¸èàóù
    for (Enemy* bullet : removes)
    {
        auto itr = std::find(enemies.begin(), enemies.end(), bullet);
        if (itr != enemies.end())
        {
            // îjä¸Ç∑Ç◊Ç´ bullet Ç™ bullets ì‡Ç…ë∂ç›Ç∑ÇÈèÍçáÅAçÌèú
            delete bullet;
            enemies.erase(itr);
        }
    }
    // îjä¸ÉäÉXÉgÇÉNÉäÉA
    removes.clear();
    collisionEnemiesAndEnemies();
}

// ï`âÊèàóù
void EnemyManager::render(ID3D11DeviceContext* dc)
{
    for (Enemy* enemy : enemies)
    {
        enemy->render(dc);
    }
}

// ìGÇìoò^
void EnemyManager::regist(Enemy* enemy)
{
    enemies.emplace_back(enemy);
}

// ìGÇÃëSçÌèú
void EnemyManager::clear()
{
    for (Enemy* enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();
}

void EnemyManager::drawDebugPrimitive()
{
    for (Enemy* enemy : enemies)
    {
        enemy->drawDebugPrimitive();
    }
}

void EnemyManager::collisionEnemiesAndEnemies()
{
    int enemyCount = getEnemyCount();
    for (int i = 0; i < enemyCount; i++)
    {
        Enemy* EnemyA = enemies.at(i);
        for (int j = i + 1; j < enemyCount; j++)
        {
            Enemy* EnemyB = enemies.at(j);

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
                // è’ìÀÇµÇƒÇ¢ÇÈèÍçáÅAEnemyBÇÃà íuÇèCê≥
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

void EnemyManager::remove(Enemy* enemy)
{
    auto itr = std::find(removes.begin(), removes.end(), enemy);

    // èdï°ÇµÇƒÇ¢ÇΩÇÁí«â¡ÇµÇ»Ç¢
    if (itr != removes.end())
        return;

    // îjä¸ÉäÉXÉgÇ…í«â¡
    removes.push_back(enemy);
}

Enemy* EnemyManager::getEnemyByRay(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd)
{
    DirectX::XMVECTOR startVec = DirectX::XMLoadFloat3(&rayStart);
    DirectX::XMVECTOR endVec = DirectX::XMLoadFloat3(&rayEnd);
    DirectX::XMVECTOR rayDir = DirectX::XMVector3Normalize(DirectX::XMVectorSubtract(endVec, startVec));

    float closestDist = FLT_MAX;
    Enemy* closestEnemy = nullptr;

    for (int i = 0; i < getEnemyCount(); ++i)
    {
        Enemy* enemy = getEnemy(i);
        DirectX::XMFLOAT3 enemyPos = *enemy->getPosition();
        DirectX::XMVECTOR enemyVec = DirectX::XMLoadFloat3(&enemyPos);

        DirectX::XMVECTOR toEnemyVec = DirectX::XMVectorSubtract(enemyVec, startVec);
        float t;
        DirectX::XMStoreFloat(&t, DirectX::XMVector3Dot(toEnemyVec, rayDir));

        if (t < 0)
            continue;

        DirectX::XMVECTOR closestPoint = DirectX::XMVectorAdd(startVec, DirectX::XMVectorScale(rayDir, t));
        DirectX::XMVECTOR diffVec = DirectX::XMVectorSubtract(enemyVec, closestPoint);

        float distSq;
        DirectX::XMStoreFloat(&distSq, DirectX::XMVector3LengthSq(diffVec));

        float radius = enemy->getRadius();
        if (distSq <= radius * radius && t < closestDist)
        {
            closestDist = t;
            closestEnemy = enemy;
        }
    }
    return closestEnemy;
}