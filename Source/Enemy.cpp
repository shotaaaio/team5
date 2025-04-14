#include "Enemy.h"
#include "Graphics/GraphicsManager.h"
#include "EnemyManager.h"

void Enemy::drawDebugPrimitive()
{
    DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

    // 衝突判定用のデバッグ用円柱を描画
    //debugRenderer->drawCylinder(position, radius, height, { 0, 0, 0, 1 });
}

void Enemy::destroy()
{
    // シングルトンインスタンスを取得してremove()を呼び出す
    EnemyManager::instance()->remove(this);
}