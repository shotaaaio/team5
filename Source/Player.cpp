#include "Player.h"
#include "DeviceManager.h"
#include "Input/InputManager.h"
#include "Graphics/ImGuiRenderer.h" 
#include "Camera.h"
#include "Graphics/GraphicsManager.h"
#include "EnemyManager.h"
#include "Collision.h"
#include "StraightBullet.h"
#include "HomingBullet.h"
#include "ObjManager.h"
#include "Ivy.h"
#include "Goal.h"
#include "SceneClear.h"
#include "SceneManager.h"

// コンストラクタ
Player::Player()
{
    model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\Nico\\nico.fbx");

    const float scale_fcator = 0.01f;	// モデルが大きいのでスケール調整
    scale = { scale_fcator, scale_fcator, scale_fcator };

    // ヒットエフェクト読み込み
    hitEffect = new Effect(".\\Resources\\Effect\\01_AndrewFM01\\hit_eff.efk");
    hitEffect1 = new Effect(".\\Resources\\Effect\\01_AndrewFM01\\blue_laser.efk");

    PlayAnimation(0, true);

    shizukuImage = new Sprite(DeviceManager::instance()->getDevice(), L".\\Resources\\Image\\shizuku.png");

    wakuImage = new Sprite(DeviceManager::instance()->getDevice(), L".\\Resources\\Image\\waku.png");

    titleImage = new Sprite(DeviceManager::instance()->getDevice(), L".\\Resources\\Image\\柵.png");

    whiteImage = new Sprite(DeviceManager::instance()->getDevice(), L".\\Resources\\Image\\white.jpg");

    clearImage = new Sprite(DeviceManager::instance()->getDevice(), L".\\Resources\\Font\\clear.png");

    searchImage = new Sprite(DeviceManager::instance()->getDevice(), L".\\Resources\\Image\\search.png");
}

// デストラクタ
Player::~Player()
{
    if (hitEffect != nullptr)
    {
        delete hitEffect;
        hitEffect = nullptr;

    }

    if (hitEffect1 != nullptr)
    {
        delete hitEffect1;
        hitEffect1 = nullptr;

    }

    if (model != nullptr)
    {
        delete model;
        model = nullptr;
    }

    if (shizukuImage != nullptr)
    {
        delete shizukuImage;
        shizukuImage = nullptr;
    }

    if (wakuImage != nullptr)
    {
        delete wakuImage;
        wakuImage = nullptr;
    }

    if (titleImage != nullptr)
    {
        delete titleImage;
        titleImage = nullptr;
    }

    if (whiteImage != nullptr)
    {
        delete whiteImage;
        whiteImage = nullptr;
    }

    if (clearImage != nullptr)
    {
        delete clearImage;
        clearImage = nullptr;
    }

    if (searchImage != nullptr)
    {
        delete clearImage;
        clearImage = nullptr;
    }
}

// 更新処理
void Player::update(float elapsedTime)
{
    if (!move_b) {
        updateTransform();
        return;
    }

    //Bボタン押下でワンショットアニメーション再生
    GamePad* gamePad = InputManager::instance()->getGamePad();
    if (gamePad->getButtonDown() & GamePad::BTN_X)
    {
        PlayAnimation(7, false);
        Attack();
    }

    if (!IsPlayAnimation())
    {
        PlayAnimation(0, true);
    }

    inputMove(elapsedTime);

    // ジャンプ入力処理
    inputJump();

    // 入力による弾発射処理
    //inputLaunchBullet();

    // 速度処理更新
    updateVelocity(elapsedTime);

    // 弾更新処理
    bulletMgr.update(elapsedTime);

    collisionPlayerAndEnemies();

    collisionPlayerAndObj();

    if (animation_index >= 0 /*&& animation_index < model->animation_clips.size()*/)
    {
        //ブレンド率の計算
        float blendrate = 1.0f;
        if (animationBlendTime < animationBlendSeconds)
        {
            animationBlendTime += elapsedTime;
            if (animationBlendTime >= animationBlendSeconds)
            {
                animationBlendTime = animationBlendSeconds;
            }
            blendrate = animationBlendTime / animationBlendSeconds;
            blendrate *= blendrate;
        }

        //ブレンド補完処理
        if (blendrate < 1.0f)
        {
            //結果出力用のキーフレーム
            animation::keyframe outkeyframe;
            const animation::keyframe* keyframes[2]=
            {
                &keyframe,
                &model->animation_clips.at(animation_index).sequence.at(0)
            };
            model->blend_animations(keyframes, blendrate, outkeyframe);
            model->update_animation(outkeyframe);

            keyframe = outkeyframe;
        }
        //通常処理
        else
        {
            //モデルアニメーション更新処理
            int clip_index{ animation_index };
            // フレーム番号
            int frame_index{ 0 };
            // アニメーション経過時間
            animation& animation{ model->animation_clips.at(animation_index) };
            //アニメーションが終了していなかったらフレームの計算を行う
            if (!animationEndFlag)
                frame_index = static_cast<int>(animation_tick * animation.sampling_rate);

            if (frame_index > animation.sequence.size() - 1)
            {
                frame_index = 0;
                animation_tick = 0;
                if (!animationLoopFlag)
                {
                    animationEndFlag = true;
                    animation_index = -1;
                }
            }
            else
            {
                if (!animationEndFlag)
                    animation_tick += elapsedTime;
            }
            keyframe = animation.sequence.at(frame_index);
        }
    }

    // 弾と敵の衝突処理
    collisionBulletsAndEnemies();

    Hashigo();

    inGoal();

    // ワールド行列の更新
    updateTransform();

    model->update_animation(keyframe);

    //model->UpdateTransform(transform);
}

// 描画処理
void Player::render(ID3D11DeviceContext* dc)
{
    model->render(dc, transform, { 1.0f,1.0f,1.0f,1.0f }, &keyframe);

    // 弾描画処理
    bulletMgr.render(dc);

    if (attack_count >= 10)
    {
        shizukuImage->render(dc,
            600, 550, 100, 100,
            1, 1, 1, 1,
            0,
            0, 0, 768, 768
        );

        wakuImage->render(dc,
            565, 540, 170, 130,
            1, 1, 1, 1,
            0,
            0, 0, 800, 600
        );
    }

    if (IsGoal)
    {
        if (white_a <= 0.5)white_a += 0.002;

        whiteImage->render(dc,
            0, 0, 1280, 720,
            1, 1, 1, white_a,
            0,
            0, 0, 850, 478
        );

        for (int i = 0; i < 10; i++)
        {
            posY1 += 0.5;
            posYMax = posY1 - (100 * (i+1));
            if (posYMax >= 0) posYMax = 0;
            titleImage->render(dc,
                140 * i-50, posYMax, 200, 500,
                1, 1, 1, 1,
                180,
                -170, -290, 1200, 1200
            );
        }

        for (int i = 0; i < 10; i++)
        {
            posY2 -= 0.5;
            posYMax = posY2 + (100 * (i + 1));
            if (posYMax <= 470)
            {
                posYMax = 470;
                if (i == 9) clear_b = true;
            }
            titleImage->render(dc,
                140 * (9-i)-50, posYMax, 200, 500,
                1, 1, 1, 1,
                0,
                170, 300, 1200, 1200
            );
        }
        
        if (clear_b){
            if(clearTime<=1) clearTime += 0.01f;
            clearImage->render(dc,
                440, 310, 400, 76,
                1, 1, 1, clearTime,
                0,
                0, 0, 171, 38
            );
        }
    }

    /*if (search_b) {
        searchImage->render(dc,
            440, 310, 200, 100,
            1, 1, 1, 1,
            0,
            0, 0, 2562, 960
        );
    }*/
}

// デバッグ用の描画
void Player::drawDebugGui()
{
    scene::node* leftHandBone = model->FindNode("NIC:wand2_BK");
    
    ImGui::Begin("Player");

    // 位置
    ImGui::InputFloat3("Position", &position.x);
    ImGui::InputFloat3("bone_Position", &leftHandBone->worldTransform._41);

    ImGui::SliderFloat("camera_position.x", &position.x, -10.0f, +10.0f);

    // 回転
    DirectX::XMFLOAT3 a{ DirectX::XMConvertToDegrees(angle.x), DirectX::XMConvertToDegrees(angle.y) , DirectX::XMConvertToDegrees(angle.z) };
    ImGui::InputFloat3("Angle", &a.x);
    angle = { DirectX::XMConvertToRadians(a.x),DirectX::XMConvertToRadians(a.y), DirectX::XMConvertToRadians(a.z) };

    // 拡大縮小
    ImGui::InputFloat3("Scale", &scale.x);
    ImGui::InputInt("attack", &attack_count);

    

    ImGui::End();
}

void Player::drawDrawPrimitive()
{
    DebugRenderer* debugRenderer = GraphicsManager::instance()->getDebugRenderer();

    // 衝突判定用のデバッグ用円柱を描画
    debugRenderer->drawCylinder(position, radius, height, { 0, 0, 0, 1 });

    // 弾デバッグプリミティブ描画
    bulletMgr.drawDebugPrimitive();

    scene::node* leftHandBone = model->FindNode("NIC:wand2_BK");
    debugRenderer->drawSphere(DirectX::XMFLOAT3(
        leftHandBone->worldTransform._41,
        leftHandBone->worldTransform._42,
        leftHandBone->worldTransform._43),
        0.4f,
        DirectX::XMFLOAT4(1, 0, 0, 1)
    );
}

DirectX::XMFLOAT3 Player::getMoveVec() const
{
    // 入力情報を取得
    GamePad* gamePad = InputManager::instance()->getGamePad();
    float ax = gamePad->getAxisLX();
    float ay = gamePad->getAxisLY();

    // カメラ方向を取得
    Camera* camera = Camera::instance();
    const DirectX::XMFLOAT3* cameraFront = camera->getFront();
    const DirectX::XMFLOAT3* cameraRight = camera->getRight();
    const DirectX::XMFLOAT3* cameraUp = camera->getUp();

    // カメラ前方向ベクトルを XZ 単位ベクトルに変換
    float cameraFrontX = cameraFront->x;
    float cameraFrontZ = cameraFront->z;

    // カメラ前方向ベクトルを単位ベクトル化
    float cameraFrontLength = sqrtf(cameraFrontX * cameraFrontX + cameraFrontZ * cameraFrontZ);
    if (cameraFrontLength > 0.0f)
    {
        cameraFrontX = cameraFrontX / cameraFrontLength;
        cameraFrontZ = cameraFrontZ / cameraFrontLength;
    }

    // カメラ右方向ベクトルを XZ 単位ベクトルに変換
    float cameraRightX = cameraRight->x;
    float cameraRightZ = cameraRight->z;
    float cameraRightLength = sqrtf(cameraRightX * cameraRightX + cameraRightZ * cameraRightZ);
    if (cameraRightLength > 0.0f)
    {
        cameraRightX = cameraRightX / cameraRightLength;
        cameraRightZ = cameraRightZ / cameraRightLength;
    }

    // 垂直入力値をカメラ前方向に、水平方向をカメラ右方向に反映し進行ベクトルを計算する
    DirectX::XMFLOAT3 vec;
    vec.x = cameraFrontX * ay + cameraRightX * ax;
    vec.z = cameraFrontZ * ay + cameraRightZ * ax;

    // Y 軸方向には移動しない。
    vec.y = 0.0f;

    return vec;
}

void Player::inputMove(float elapsedTime)
{
    //// 入力情報を取得
    //GamePad* gamePad = InputManager::instance()->getGamePad();
    //float ax = gamePad->getAxisLX();
    //float ay = gamePad->getAxisLY();

    //// 移動処理
    //float moveSpeed = 5.0f * elapsedTime;
    //position.x += ax * moveSpeed;
    //position.z += ay * moveSpeed;

    ////回転処理
    //float rotateSpeed = DirectX::XMConvertToRadians(360.0f) * elapsedTime;//一秒間に最大360度に制限

    //if (gamePad->getButton() & GamePad::BTN_A)
    //{
    //    // Y軸回転操作
    //    angle.y += rotateSpeed;
    //}

    if (!isOnLadder)
    {
        // 進行ベクトル取得
        DirectX::XMFLOAT3 moveVec = getMoveVec();

        // 移動処理
        move(moveVec.x, moveVec.z, moveSpeed);

        // 旋回処理
        turn(elapsedTime, moveVec.x, moveVec.z, turnSpeed);
    }
    else
    {
        move(0, 0, 0);
    }
}

void Player::collisionPlayerAndEnemies()
{
    EnemyManager* eneMgr = EnemyManager::instance();

    // 全ての敵と総当たりで衝突処理
    int enemyCount = eneMgr->getEnemyCount();
    for (int i = 0; i < enemyCount; ++i)
    {
        Enemy* enemy = eneMgr->getEnemy(i);

        // 衝突方向
        DirectX::XMFLOAT3 outVec;

        // 衝突処理
        if (Collision::intersectCylinderAndCylinder(
            position,
            radius,
            height,
            *(enemy->getPosition()),
            enemy->getRadius(),
            enemy->getHeight(),
            outVec))
        {
            //OutputDebugStringA("衝突\n");

            DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR E = DirectX::XMLoadFloat3((enemy->getPosition()));
            DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
            DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
            DirectX::XMFLOAT3 normal;
            DirectX::XMStoreFloat3(&normal, N);
            if (normal.y > 0.8f)
            {
                jump(jumpSpeed * 0.5f);
            }
            else
            {
                ////半径の合計
                //float range = radius + enemy->getRadius();

                //// 衝突方向のベクトルを半径の合計の長さにスケーリング
                //DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), range);

                //// そのベクトルを衝突する側の位置に足す
                //vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), vec);

                //// その位置ベクトルを衝突される側に設定する
                //DirectX::XMFLOAT3 pos;
                //DirectX::XMStoreFloat3(&pos, vec);
                //pos.y = enemy->getPosition()->y;
                //enemy->setPosition(pos);

                /*velocity.x -= 2*outVec.x;
                velocity.z -= 2*outVec.z;*/
            }
        }
    }
}

void Player::collisionBulletsAndEnemies()
{
    EnemyManager* enemyMgr = EnemyManager::instance();

    // 全ての弾と全ての敵を総当たりで衝突処理
    int bulletCount = bulletMgr.getBulletCount();
    int enemyCount = enemyMgr->getEnemyCount();
    for (int i = 0; i < bulletCount; ++i)
    {
        Bullet* bullet = bulletMgr.getBullet(i);

        for (int j = 0; j < enemyCount; ++j)
        {
            Enemy* enemy = enemyMgr->getEnemy(j);

            // 衝突処理
            DirectX::XMFLOAT3 outVec;
            if (Collision::intersectSphereAndCylinder(
                *bullet->getPosition(),
                bullet->getRadius(),
                *enemy->getPosition(),
                enemy->getRadius(),
                enemy->getHeight(),
                outVec))
            {
                int damage = 3; // 仮のダメージとして１
                if (enemy->applyDamage(damage, 0.1f))
                {
                    // 吹き飛ばし
                    float power = 10.0f;
                    DirectX::XMFLOAT3 impulse;
                    impulse.x = outVec.x * power;
                    impulse.y = power * 0.5f;
                    impulse.z = outVec.z * power;
                    enemy->addImpulse(impulse);

                    // ヒットエフェクトの再生
                    DirectX::XMFLOAT3 enePos = *enemy->getPosition();
                    enePos.y += enemy->getHeight() * 0.5f;
                    Effekseer::Handle handle = hitEffect->play(&enePos, 0.5f);

                    // 弾の破棄
                    bullet->destroy();
                }
            }
        }
    }
}

void Player::collisionPlayerAndObj()
{
    ObjManager* objMgr = ObjManager::instance();

    // 全ての敵と総当たりで衝突処理
    int objCount = objMgr->getObjCount();
    for (int i = 0; i < objCount; ++i)
    {
        Obj* enemy = objMgr->getObj(i);

        // 衝突方向
        DirectX::XMFLOAT3 outVec;

        // 衝突処理
        if (Collision::intersectCylinderAndCylinder(
            position,
            radius,
            height,
            *(enemy->getPosition()),
            enemy->getRadius(),
            enemy->getHeight(),
            outVec))
        {
            //OutputDebugStringA("衝突\n");

            DirectX::XMVECTOR P = DirectX::XMLoadFloat3(&position);
            DirectX::XMVECTOR E = DirectX::XMLoadFloat3((enemy->getPosition()));
            DirectX::XMVECTOR V = DirectX::XMVectorSubtract(P, E);
            DirectX::XMVECTOR N = DirectX::XMVector3Normalize(V);
            DirectX::XMFLOAT3 normal;
            DirectX::XMStoreFloat3(&normal, N);
            if (normal.y > 0.8f)
            {
                jump(jumpSpeed * 0.5f);
            }
            else
            {
                ////半径の合計
                //float range = radius + enemy->getRadius();

                //// 衝突方向のベクトルを半径の合計の長さにスケーリング
                //DirectX::XMVECTOR vec = DirectX::XMVectorScale(DirectX::XMLoadFloat3(&outVec), range);

                //// そのベクトルを衝突する側の位置に足す
                //vec = DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position), vec);

                //// その位置ベクトルを衝突される側に設定する
                //DirectX::XMFLOAT3 pos;
                //DirectX::XMStoreFloat3(&pos, vec);
                //pos.y = enemy->getPosition()->y;
                //enemy->setPosition(pos);

                /*velocity.x -= 2*outVec.x;
                velocity.z -= 2*outVec.z;*/
            }
        }
    }
}

void Player::inputJump()
{
    GamePad* gamePad = InputManager::instance()->getGamePad();
    if (gamePad->getButtonDown() & GamePad::BTN_Z)
    {
        // ジャンプ回数制限
        if (jumpCount < jumpLimit)
        {
            // ジャンプ
            jumpCount++;
            jump(jumpSpeed);
        }
    }
}

void Player::inputLaunchBullet()
{
    GamePad* gamePad = InputManager::instance()->getGamePad();

    // ストレート弾発射
    if (gamePad->getButtonDown() & GamePad::BTN_F)
    {
        // 前方向
        DirectX::XMFLOAT3 dir;
        dir.x = sinf(angle.y);
        dir.y = 0.0f;
        dir.z = cosf(angle.y);

        // 発射位置（プレイヤーの腰あたり
        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height * 0.5f;
        pos.z = position.z;

        // 発射
        StraightBullet* bullet = new StraightBullet(&bulletMgr);
        bullet->setDirection(dir);
        bullet->setPosition(pos);
    }

    // ホーミング弾発射
    if (gamePad->getButtonDown() & GamePad::BTN_V)
    {
        // 前方向
        DirectX::XMFLOAT3 dir;
        dir.x = sinf(angle.y);
        dir.y = 0.0f;
        dir.z = cosf(angle.y);

        // 発射位置（プレイヤーの腰あたり
        DirectX::XMFLOAT3 pos;
        pos.x = position.x;
        pos.y = position.y + height * 0.5f;
        pos.z = position.z;

        // ターゲット
        DirectX::XMFLOAT3 target;
        target.x = pos.x + dir.x * 1000.0f;
        target.y = pos.y + dir.y * 1000.0f;
        target.z = pos.z + dir.z * 1000.0f;

        // 一番近くの敵を取得
        Enemy* enemy = EnemyManager::instance()->searchEnemy(&position);
        if (enemy != nullptr)
        {
            target = *enemy->getPosition();
            target.y += enemy->getHeight() * 0.5f;
        }

        // 発射
        HomingBullet* bullet = new HomingBullet(&bulletMgr);
        bullet->setDirection(dir);
        bullet->setPosition(pos);
        bullet->lockonTarget(target);
    }
}

void Player::onLanding()
{
    if (jumpCount > 0)
    {
        // ヒットエフェクトの再生
        Effekseer::Handle handle = hitEffect->play(&position, 0.5f);
    }

    jumpCount = 0;
}

//アニメーション再生
void Player::PlayAnimation(int index, bool loop, float blendSeconds)
{
    animation_index = index;
    animation_tick = 0.0f;
    animationLoopFlag = loop;
    animationEndFlag = false;
    animationBlendTime = 0.0f;
    animationBlendSeconds = blendSeconds;
}

//アニメーション再生中か
bool Player::IsPlayAnimation() const
{
    if (animation_index < 0) return false;
    if (animation_index >= model->animation_clips.size()) return false;
    return true;
}

void Player::Attack()
{
        // 前方向
        DirectX::XMFLOAT3 dir;
        dir.x = sinf(angle.y);
        dir.y = 0.0f;
        dir.z = cosf(angle.y);

        DirectX::XMFLOAT3 playerPos = position;
        playerPos.x += dir.x * (radius + 0.5f);
        playerPos.z += dir.z * (radius + 0.5f);

        EnemyManager* eneMgr = EnemyManager::instance();

        // 全ての敵と総当たりで衝突処理
        int enemyCount = eneMgr->getEnemyCount();
        for (int i = 0; i < enemyCount; ++i)
        {
            Enemy* enemy = eneMgr->getEnemy(i);

            // 衝突方向
            DirectX::XMFLOAT3 outVec;

            // 衝突処理
            if (Collision::intersectCylinderAndCylinder(
                playerPos,
                radius,
                height,
                *(enemy->getPosition()),
                enemy->getRadius(),
                enemy->getHeight(),
                outVec))
            {
                if (InputManager::instance()->attack)
                {
                    int damage = 2; // 仮のダメージとして１
                    if (enemy->applyDamage(damage, 0.1f))
                    {
                    }
                }
                // 吹き飛ばし
                float power = 10.0f;
                DirectX::XMFLOAT3 impulse;
                impulse.x = dir.x * power;
                impulse.y = power * 0.5f;
                impulse.z = dir.z * power;
                enemy->addImpulse(impulse);

                attack_count++;
            }
        }
}

void Player::Hashigo()
{
    ObjManager* eneMgr = ObjManager::instance();

    int enemyCount = eneMgr->getObjCount();
    isOnLadder = false;
    for (int i = 0; i < enemyCount; i++)
    {
        Obj* enemy = eneMgr->getObj(i);

        Ivy* ivy = dynamic_cast<Ivy*>(enemy);
        if (ivy)
        {
            DirectX::XMFLOAT3 outVec;

            if (Collision::intersectCylinderAndCylinder(position, radius, height, *ivy->getPosition(), ivy->getRadius(), ivy->getHeight(), outVec))
            {

                DirectX::XMFLOAT3 toIvyVec = {
                ivy->getPosition()->x - position.x,
                0.0f,
                ivy->getPosition()->z - position.z
                };

                // スティックの入力
                GamePad* gamePad = InputManager::instance()->getGamePad();
                float ax = gamePad->getAxisLX();
                float ay = gamePad->getAxisLY();

                // 正規化
                DirectX::XMVECTOR inputVec = DirectX::XMVector3Normalize(DirectX::XMVectorSet(ax, 0, ay, 0));
                DirectX::XMVECTOR toIvy = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&toIvyVec));

                // 入力ベクトルがツタの方向と近ければ上昇
                float dot = DirectX::XMVectorGetX(DirectX::XMVector3Dot(inputVec, toIvy));
                if ((ax != 0 || ay != 0) && dot > 0.7f)
                {
                    isOnLadder = true;
                    velocity.x = 0;
                    velocity.y = 2;
                    velocity.z = 0;
                }
                else
                {
                    velocity.y = 0;
                }

                if (attack_count >= 10)
                {
                    if (gamePad->getButtonDown() & GamePad::BTN_F)
                    {
                        DirectX::XMFLOAT3 eneScale = *enemy->getScale();
                        eneScale.y = 0.01f;
                        eneScale.z = 0.01f;
                        enemy->setScale(eneScale);
                        enemy->setHeight(7);
                    }
                }

                //Search();

                search_b = true;
            }
            else
                search_b = false;
        }
    }
}

void Player::inGoal()
{
    ObjManager* eneMgr = ObjManager::instance();

    int enemyCount = eneMgr->getObjCount();
    
    for (int i = 0; i < enemyCount; i++)
    {
        Obj* enemy = eneMgr->getObj(i);

        Goal* ivy = dynamic_cast<Goal*>(enemy);
        if (ivy)
        {
            DirectX::XMFLOAT3 outVec;

            if (Collision::intersectCylinderAndCylinder(position, radius, height, *ivy->getPosition(), ivy->getRadius(), ivy->getHeight(), outVec))
            {
                IsGoal = true;
                move_b = false;
                //SceneManager::instance()->changeScene(new SceneClear);
            }
        }
    }
}

void Player::Search()
{
    // 前方向
    DirectX::XMFLOAT3 dir;
    dir.x = sinf(angle.y);
    dir.y = 0.0f;
    dir.z = cosf(angle.y);

    GamePad* gamePad = InputManager::instance()->getGamePad();
    
    if (gamePad->getButtonDown() & GamePad::BTN_F) position.x = 1;
}