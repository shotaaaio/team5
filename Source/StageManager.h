#pragma once

#include "Stage.h"
#include <vector>

class StageManager
{
public:
    StageManager() {}
    ~StageManager() { stages.clear(); }

    static StageManager* instance()
    {
        static StageManager inst;
        return &inst;
    }

    // 更新処理
    void update(float elapsedTime);

    // 描画処理
    void render(ID3D11DeviceContext* dc);

    // レイとモデルのレイキャスト
    bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

    // ステージの登録
    void regist(Stage* stage) { stages.emplace_back(stage); }

    // ステージの取得
    Stage* getStage(int index) { return stages.at(index); }

    // ステージの全削除
    void clear();

private:
    std::vector<Stage*> stages;
};