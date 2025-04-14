#include "StageManager.h"
#include "Collision.h"

// 更新処理
void StageManager::update(float elapsedTime)
{
    for (Stage* stage : stages) stage->update(elapsedTime);
}

// 描画処理
void StageManager::render(ID3D11DeviceContext* dc)
{
    for (Stage* stage : stages) stage->render(dc);
}

// 敵の全削除
void StageManager::clear()
{
    for (Stage* stage : stages) delete stage;
}

bool StageManager::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    bool hitDetected = false;
    for (Stage* stage : stages) {
        if (stage->raycast(start, end, hit)) {
            hitDetected = true;

            // 他のオブジェクトやモデルとの衝突判定を強化
            for (Stage* otherStage : stages) {
                if (otherStage != stage) {
                    HitResult tempHit;
                    // 他のステージ内のモデルに対するレイキャストを行う
                    if (otherStage->raycast(start, end, tempHit)) {
                        // 衝突が検出された場合、適切に補正位置を更新
                        if (tempHit.distance < hit.distance) {
                            hit = tempHit; // より近い衝突を選択
                        }
                    }
                }
            }
        }
    }
    return hitDetected;
}