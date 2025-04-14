#include "StageMain.h"
#include "DeviceManager.h"

StageMain::StageMain()
{
    model = new SkinnedMesh(DeviceManager::instance()->getDevice(), ".\\Resources\\Model\\ExampleStage\\ExampleStage.cereal");
}

StageMain::~StageMain()
{
    if (model != nullptr)
    {
        delete model;
        model = nullptr;
    }
}

void StageMain::update(float elapsedTime)
{
    // ¡‚Í“Á‚Éˆ—‚Í‚È‚µ
}

void StageMain::render(ID3D11DeviceContext* dc)
{
    model->render(dc, { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 }, { 0.8f,0.8f,0.8f,1.0f }, nullptr);
}

bool StageMain::raycast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
    return Collision::intersectRayAndModel(start, end, model, hit);
}