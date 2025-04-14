#include "Obj.h"
#include "MathCulc.h"

// 行列更新処理
void Obj::updateTransform()
{
    // スケール行列を作成
    DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
    // 回転行列を作成
    DirectX::XMMATRIX X = DirectX::XMMatrixRotationX(angle.x);
    DirectX::XMMATRIX Y = DirectX::XMMatrixRotationY(angle.y);
    DirectX::XMMATRIX Z = DirectX::XMMatrixRotationZ(angle.z);
    DirectX::XMMATRIX R = Y * X * Z;
    // 位置行列を作成
    DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
    // ３つの行列を組み合わせ、ワールド行列を作成
    DirectX::XMMATRIX W = S * R * T;
    // 計算したワールド行列を transform に取り出す
    DirectX::XMStoreFloat4x4(&transform, W);
}

void Obj::addImpulse(const DirectX::XMFLOAT3& impulse)
{
    // 速度に力を加える
    velocity.x += impulse.x;
    velocity.y += impulse.y;
    velocity.z += impulse.z;
}

void Obj::updateVelocity(float elapsedTime)
{
    // 垂直速度更新処理
    updateVerticalVelocity(elapsedTime);

    // 垂直移動更新処理
    updateVerticalMove(elapsedTime);

    updateHorizontalVelocity(elapsedTime);

    updateHorizontalMove(elapsedTime);
}

void Obj::updateVerticalVelocity(float elapsedTime)
{
    // 重力処理（フレーム単位で計算）
    velocity.y += gravity * elapsedTime * 60.0f;
}

void Obj::updateVerticalMove(float elapsedTime)
{
    // 垂直方向の移動量
    float moveY = velocity.y * elapsedTime;

    // 姿勢制御用法線ベクトル（デフォルトは上方向）
    DirectX::XMFLOAT3 normal = { 0,1,0 };

    if (moveY < 0.0f)
    {
        // 落下中

        // レイの開始位置を設定（足もとより少し上）
        DirectX::XMFLOAT3 start = { position.x, position.y + 1.0f, position.z };
        // レイの終点位置を設定（移動後の位置）
        DirectX::XMFLOAT3 end = { position.x, position.y + moveY, position.z };

        // レイキャストによる地面判定
        HitResult hit;
        if (StageManager::instance()->RayCast(start, end, hit))
        {
            // 地面の向きを姿勢制御用法線ベクトルに設定
            normal = hit.normal;

            // 地面に設置している（ヒット結果の y 位置を反映）
            position.y = hit.position.y;
            velocity.y = 0.0f;

            // 傾斜率の計算
            float normalLengthXZ = sqrtf(hit.normal.x * hit.normal.x + hit.normal.z * hit.normal.z);
            slopeRate = 1.0f - (hit.normal.y / (normalLengthXZ + hit.normal.y));

            groundedFlag = true;
        }
        else
        {
            // 空中に浮いている
            position.y += moveY;
            groundedFlag = false;
        }
    }
    else if (moveY > 0.0f)
    {
        // 上昇中
        position.y += moveY;
        groundedFlag = false;
    }

    // 姿勢制御用法線ベクトルから x と z の角度を計算
    // y 軸が姿勢制御用法線ベクトル方向に向くように角度を計算します
    float angleX = atan2f(normal.z, normal.y);
    float angleZ = -atan2f(normal.x, normal.y);

    // 線形補間で滑らかに回転
    angle.x = MathCulc::Lerp(angle.x, angleX, 0.2f);
    angle.z = MathCulc::Lerp(angle.z, angleZ, 0.2f);
}

void Obj::updateHorizontalVelocity(float elapsedTime)
{
    // 速度に力が加わっていたら（0 じゃなかったら）減速処理を行う
    float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (length > 0.0f)
    {
        // 摩擦力（フレーム単位で計算）
        float friction = this->friction * elapsedTime * 60.0f;

        // 空中にいるときは摩擦力を減少
        if (groundedFlag == false)
            friction *= airControl;

        if (length > friction)
        {
            // 摩擦による横方向の減速処理
            float vx = velocity.x / length;//正規化している
            float vz = velocity.z / length;

            velocity.x -= vx * friction;
            velocity.z -= vz * friction;
        }
        else
        {
            // 横方向の速度が摩擦力以下になったので速度を無効化
            velocity.x = 0.0f;
            velocity.z = 0.0f;
        }
    }

    // 最大速度以下なら加速処理を行う
    if (length <= maxMoveSpeed)
    {
        // 方向ベクトルがゼロでないなら加速処理を行う
           // ゼロの場合は入力されていない
        float directionLength = sqrtf(direction.x * direction.x + direction.z * direction.z);
        if (directionLength > 0.0f)
        {
            // 加速度（フレーム単位で計算）
            float acceleration = this->acceleration * elapsedTime * 60.0f;

            // 空中にいるときは加速度を減少
            if (groundedFlag == false)
                acceleration *= airControl;

            // 加速処理
                // 方向ベクトルに加速度をスケーリングした値を速度ベクトルに加算
            velocity.x += direction.x * acceleration;
            velocity.z += direction.z * acceleration;

            // 傾斜率が高い場合は落ちていくように計算
            if (groundedFlag == true && slopeRate > 0.0f)
                velocity.y -= length * slopeRate * elapsedTime * 60.0f;

            // 最大速度制限
            float length = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
            if (length > maxMoveSpeed)
            {
                // 方向ベクトルに最大移動速度をスケーリングした値を速度ベクトルに代入
                velocity.x = direction.x * maxMoveSpeed;
                velocity.z = direction.z * maxMoveSpeed;
            }
        }
    }
}

void Obj::updateHorizontalMove(float elapsedTime)
{
    // 水平速度量計算
    float velocityLengthXZ = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
    if (velocityLengthXZ > 0.0f)
    {
        // 計算用の移動後の速度
        float moveX = velocity.x * elapsedTime;
        float moveZ = velocity.z * elapsedTime;

        // レイの始点位置と終点位置
        DirectX::XMFLOAT3 start = { position.x, position.y + 1.0f, position.z };
        DirectX::XMFLOAT3 end = { position.x + moveX, position.y + 1.0f, position.z + moveZ };

        // レイキャスト
        HitResult hit;
        if (StageManager::instance()->getStage(0)->raycast(start, end, hit))
        {
            DirectX::XMVECTOR startVec = DirectX::XMLoadFloat3(&hit.position);
            DirectX::XMVECTOR endVec = DirectX::XMLoadFloat3(&end);
            DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(endVec, startVec);

            // 壁の法線ベクトル
            DirectX::XMVECTOR normalVec = DirectX::XMLoadFloat3(&hit.normal);

            // 入射ベクトルを逆ベクトルに変換
            vec = DirectX::XMVectorNegate(vec);

            // 入射ベクトルを法線で射影（移動後の位置から壁までの距離）
            DirectX::XMVECTOR lengthVec = DirectX::XMVector3Dot(vec, normalVec);
            float length;
            DirectX::XMStoreFloat(&length, lengthVec);

            // 補正位置へのベクトルを計算
            DirectX::XMVECTOR correctVec = DirectX::XMVectorScale(normalVec, length * 1.1f);

            // 最終的な補正位置ベクトルを計算
            DirectX::XMVECTOR correctPosVec = DirectX::XMVectorAdd(endVec, correctVec);
            DirectX::XMFLOAT3 correctPos;
            DirectX::XMStoreFloat3(&correctPos, correctPosVec);

            DirectX::XMVECTOR correctionVec = DirectX::XMVectorSubtract(correctPosVec, startVec);
            HitResult hit2;
            if (StageManager::instance()->getStage(0)->raycast(hit.position, correctPos, hit2))
            {
                position.x = hit2.position.x;
                position.z = hit2.position.z;
            }
            else
            {
                position.x = correctPos.x;
                position.z = correctPos.z;
            }
        }
        else
        {
            // 壁にあたっていない場合は通常の移動
            position.x += moveX;
            position.z += moveZ;
        }
    }
}