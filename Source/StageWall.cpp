#include "StageWall.h"
#include "DeviceManager.h"

StageWall::StageWall()
{
    model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\Cube\\cube.000.fbx");
}

StageWall::~StageWall()
{
    if (model != nullptr)
    {
        delete model;
        model = nullptr;
    }
}

void StageWall::update(float elapsedTime)
{
    // 今は特に処理はなし
    updateTransform();
}

void StageWall::render(ID3D11DeviceContext* dc)
{
    model->render(dc, { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 }, { 0.8f,0.8f,0.8f,1.0f }, nullptr);
}

bool StageWall::raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    for (auto& mesh : model->meshes)
    {
        // 各メッシュのデフォルト変換をStageWallのtransformで上書き
        mesh.default_global_transform = transform;
    }
    return Collision::intersectRayAndModel(start, end, model, hit);
}

// 行列更新処理
void StageWall::updateTransform()
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