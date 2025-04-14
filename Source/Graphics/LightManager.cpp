#include "LightManager.h"
#include "Collision.h"

LightManager::~LightManager()
{
    clear();
}

// XVˆ—




// “G‚ð“o˜^
void LightManager::regist(Light* enemy)
{
    enemies.emplace_back(enemy);
}

// “G‚Ì‘Síœ
void LightManager::clear()
{
    for (Light* enemy : enemies)
    {
        delete enemy;
    }
    enemies.clear();
}

void LightManager::DrawDebugPrimitive()
{
    for (Light* enemy : enemies)
    {
        enemy->DrawDebugPrimitive();
    }
}



void LightManager::remove(Light* enemy)
{
    auto itr = std::find(removes.begin(), removes.end(), enemy);

    // d•¡‚µ‚Ä‚¢‚½‚ç’Ç‰Á‚µ‚È‚¢
    if (itr != removes.end())
        return;

    // ”jŠüƒŠƒXƒg‚É’Ç‰Á
    removes.push_back(enemy);
}

void LightManager::PushRenderContext(LightRenderContext& rc)
{
    // ŠÂ‹«Œõ‚Ìî•ñ‚ð’Ç‰Á
    rc.ambientLightColor = ambientColor;

    // “o˜^‚³‚ê‚Ä‚¢‚éŒõŒ¹‚Ìî•ñ‚ðÝ’è
    for (Light* light : enemies)
    {
        light->PushRenderContext(rc);
    }
}

