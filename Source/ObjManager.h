#pragma once

#include <vector>
#include "Obj.h"

// “GŠÇ—
class ObjManager
{
private:
    ObjManager() {}
    ~ObjManager() {}

    // “G“¯m‚ÌÕ“Ëˆ—
    void collisionEnemiesAndEnemies();

public:
    // singleton
    static ObjManager* instance()
    {
        static ObjManager inst;
        return &inst;
    }

    // XVˆ—
    void update(float elapsedTime);

    // •`‰æˆ—
    void render(ID3D11DeviceContext* dc);

    // “G‚Ì“o˜^
    void regist(Obj* enemy);

    // “G‚Ì”jŠü
    void clear();

    // ƒfƒoƒbƒOƒvƒŠƒ~ƒeƒBƒu•`‰æ
    //void drawDebugPrimitive();

    // “G‚Ì‘”‚ğæ“¾
    int getObjCount() const { return static_cast<int>(enemies.size()); }

    // “G‚ğæ“¾
    Obj* getObj(int index) { return enemies.at(index); }

    Obj* searchEnemy(DirectX::XMFLOAT3* position)
    {
        // ˆê”Ô‹ß‚­‚Ì“G‚ğƒ^[ƒQƒbƒg‚Éİ’è
        float dist = FLT_MAX;
        Obj* closestEnemy = nullptr;
        int enemyCount = getObjCount();
        for (int i = 0; i < enemyCount; ++i)
        {
            // “G‚Æ‚Ì‹——£‚ğ”»’è
            Obj* enemy = getObj(i);
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
    void remove(Obj* enemy);

    Obj* getEnemyByRay(const DirectX::XMFLOAT3& rayStart, const DirectX::XMFLOAT3& rayEnd);

private:
    std::vector<Obj*>	enemies;
    std::vector<Obj*> removes;
};