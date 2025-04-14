#pragma once

#include "Graphics/SkinnedMesh.h"
#include "Character.h"
#include "BulletManager.h"
#include "Effect/Effect.h"
#include "Graphics/Sprite.h"

// プレイヤー
class Player : public Character
{
public:
    Player();
    ~Player() override;

    // 更新処理
    void update(float elapsedTime);

    // 描画処理
    void render(ID3D11DeviceContext* dc);

    // デバッグ用の描画
    void drawDebugGui();

    // デバッグプリミティブの描画
    void drawDrawPrimitive();

    // プレイヤーと敵との衝突処理
    void collisionPlayerAndEnemies();

    // 弾と敵の衝突処理
    void collisionBulletsAndEnemies();

    void collisionPlayerAndObj();

private:
    // 入力値から移動ベクトルを取得
    DirectX::XMFLOAT3 getMoveVec() const;

    // 移動入力処理
    void inputMove(float elapsedTime);

    void Hashigo();

    void inGoal();

public:
    // ジャンプ入力処理
    void inputJump();

    // 入力による弾発射処理
    void inputLaunchBullet();

    //アニメーション再生
    void PlayAnimation(int index, bool loop, float blendseconds = 0.2f);

    //アニメーション再生中か
    bool IsPlayAnimation() const;

    void Attack();

protected:
    // 着地したときに呼び出される
    void onLanding() override;

private:
    SkinnedMesh* model = nullptr;
    // 移動スピード
    float moveSpeed = 5.0f;

    // 旋回スピード
    float turnSpeed = DirectX::XMConvertToRadians(720);

    // ジャンプ力
    float jumpSpeed = 20.0f;
    int jumpCount = 0;	// ジャンプ回数
    int jumpLimit = 1;	// ジャンプ制限（最大ジャンプ数、ひとまず２段ジャンプ可）
    // 弾管理
    BulletManager bulletMgr;
    // ヒットエフェクト
    Effect* hitEffect = nullptr;
    Effect* hitEffect1 = nullptr;
    // アニメーションの番号
    int animation_index = -1;
    animation::keyframe keyframe;
    //アニメーション再生時間
    float animation_tick{ 0 };
    //true...アニメーションをループ
    bool animationLoopFlag = false;
    //true...アニメーションを終了
    bool animationEndFlag = false;
    //アニメーションブレンドの経過の合計時間
    float animationBlendTime = 0.0f;
    //アニメーションブレンドの時間
    float animationBlendSeconds = 0.0f;

    int attack_count = 0;

    bool isOnLadder;

    Sprite* shizukuImage = nullptr;

    Sprite* wakuImage = nullptr;

    Sprite* titleImage = nullptr;

    bool IsGoal = false;

    float posY1 = 0.0f;
    float posY2 = 720.0f;

    float posYMax = 0.0f;

    Sprite* whiteImage = nullptr;

    float white_a = 0.0f;

    Sprite* clearImage = nullptr;

    bool clear_b = false;

    float clearTime = 0.0f;

    bool move_b = true;

    void Search();

    Sprite* searchImage = nullptr;

    bool search_b;
};