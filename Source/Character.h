#pragma once

#include <DirectXMath.h>
#include "Effect/Effect.h"
#include "StageManager.h"

// キャラクター
class Character
{
public:
    Character() {}
    virtual ~Character() {}

    // 行列更新処理
    void updateTransform();

    // 位置取得
    const DirectX::XMFLOAT3* getPosition() const { return &position; }

    void setPosition(const DirectX::XMFLOAT3& position) { this->position = position; }

    const DirectX::XMFLOAT3* getAngle() const { return &angle; }

    void setAngle(const DirectX::XMFLOAT3& angle) { this->angle = angle; }

    const DirectX::XMFLOAT3* getScale() const { return &scale; }

    void setScale(const DirectX::XMFLOAT3& scale) { this->scale = scale; }

    // 半径の取得
    float getRadius() const { return radius; }

    // 着地したかどうかのフラグの取得
    bool isGrounded() const { return groundedFlag; }

    // 高さ取得
    float getHeight() const { return height; }

    // ライフを取得
    int getHealth() const { return health; }
    
    // 最大ライフを取得
    int getMaxHealth() const { return maxHealth; }

    void setHeight(float height) { this->height = height; }

    /// <summary>
    /// ダメージ処理
    /// </summary>
    /// <param name="damage">ダメージ量</param>
    /// <returns>true...HPに変化があった</returns>
    bool applyDamage(int damage, float invincible);

    // 衝撃を与える
    void addImpulse(const DirectX::XMFLOAT3& impulse);

protected:
    // 移動処理
    void move(float vx, float vz, float speed);

    // 旋回処理
    void turn(float elapsedTime, float vx, float vz, float speed);

    // ジャンプ処理
    void jump(float speed);

    // 速度処理更新
    void updateVelocity(float elpasedTime);

    // 着地したときに呼び出される
    virtual void onLanding() {}

    // ダメージを受けたときに呼ばれる
    virtual void onDamaged() {}

    // 死亡したときに呼ばれる
    virtual void onDead() {}

    // 無敵時間の更新
    void updateInvincibleTimer(float elapsedTime);

private:
    // 垂直速度更新処理
    void updateVerticalVelocity(float elapsedTime);

    // 垂直移動更新処理
    void updateVerticalMove(float elapsedTime);

    // 水平速度更新処理
    void updateHorizontalVelocity(float elapsedTime);

    // 水平移動更新処理
    void updateHorizontalMove(float elapsedTime);

protected:
    DirectX::XMFLOAT3 position = { 0,0,0 };	// 位置
    DirectX::XMFLOAT3 angle = { 0,0,0 };	// 回転
    DirectX::XMFLOAT3 scale = { 1,1,1 };	// 拡大縮小

    // 姿勢行列
    DirectX::XMFLOAT4X4	transform = {
        1,0,0,0,
        0,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    float radius = 0.5f; // 半径
    float gravity = -1.0f;	// 重力（フレーム単位の値）
    DirectX::XMFLOAT3 velocity = { 0,0,0 };	// 速度
    bool groundedFlag = false;	// true...着地した
    float height = 2.0f;	// 高さ
    int health = 5; // ライフ（仮のライフとして５）
    int maxHealth = 5;	// 最大ライフ
    float invincibleTimer = 0.5f;   // 無敵時間（0.5秒）
    float friction = 0.5f; // 摩擦
    float acceleration = 1.0f;  // 加速度
    float maxMoveSpeed = 5.0f;  // 最大移動速度
    DirectX::XMFLOAT3 direction = { 0,0,0 };    // 移動方向
    float airControl = 0.3f;    // 空中制御用係数
    float slopeRate = 1.0f; // 傾斜率（デフォルト = 1.0f）
    
private:
    Effect* hitEffect = nullptr;
};