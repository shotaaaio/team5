#pragma once

#include <vector>
#include "Light.h"

// “GŠÇ—
class LightManager
{
private:
    LightManager() {}
    ~LightManager();

    

public:
    // singleton
    static LightManager* instance()
    {
        static LightManager inst;
        return &inst;
    }

   

    

    // “G‚Ì“o˜^
    void regist(Light* enemy);

    // “G‚Ì”jŠü
    void clear();

    // ƒfƒoƒbƒOƒvƒŠƒ~ƒeƒBƒu•`‰æ
    void DrawDebugPrimitive();

    // “G‚Ì‘”‚ğæ“¾
    int getObjCount() const { return static_cast<int>(enemies.size()); }

    // “G‚ğæ“¾
    Light* getObj(int index) { return enemies.at(index); }

    

    // “Gíœ
    void remove(Light* enemy);

    // ƒ‰ƒCƒgî•ñ‚ğRenderContext‚ÉÏ‚Ş
    void PushRenderContext(LightRenderContext& rc);

private:
    std::vector<Light*>	enemies;
    std::vector<Light*> removes;
    DirectX::XMFLOAT4	ambientColor = { 0.2f, 0.2f, 0.2f, 1.0f };
};