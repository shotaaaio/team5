#include "BulletManager.h"

// コンストラクタ
BulletManager::BulletManager()
{

}

// デストラクタ
BulletManager::~BulletManager()
{
    clear();
}

// 更新処理
void BulletManager::update(float elapsedTime)
{
    for (Bullet* bullet : bullets)
    {
        bullet->update(elapsedTime);
    }

    // 破棄処理
    for (Bullet* bullet : removes)
    {
        auto itr = std::find(bullets.begin(), bullets.end(), bullet);
        if (itr != bullets.end())
        {
            // 破棄すべき bullet が bullets 内に存在する場合、削除
            delete bullet;
            bullets.erase(itr);
        }
    }
    // 破棄リストをクリア
    removes.clear();
}

// 描画処理
void BulletManager::render(ID3D11DeviceContext* dc)
{
    for (Bullet* bullet : bullets)
    {
        bullet->render(dc);
    }
}

// デバッグプリミティブ描画
void BulletManager::drawDebugPrimitive()
{
    for (Bullet* bullet : bullets)
    {
        bullet->drawDebugPrimitive();
    }
}

// 弾登録
void BulletManager::regist(Bullet* bullet)
{
    bullets.emplace_back(bullet);
}

// 弾全削除
void BulletManager::clear()
{
    for (Bullet* bullet : bullets)
    {
        delete bullet;
    }
    bullets.clear();
}

// 弾削除
void BulletManager::remove(Bullet* bullet)
{
    auto itr = std::find(removes.begin(), removes.end(), bullet);

    // 重複していたら追加しない
    if (itr != removes.end())
        return;

    // 破棄リストに追加
    removes.push_back(bullet);
}