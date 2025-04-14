#include "Effect.h"
#include "EffectManager.h"
#include "DeviceManager.h"

// コンストラクタ
Effect::Effect(const char* filename)
{
    // 排他処理
    std::lock_guard<std::mutex> lock(DeviceManager::instance()->getMutex());

    // ファイル名は UTF-16 しか対応していないので文字コードを変換
    char16_t utf16Filename[256];
    Effekseer::ConvertUtf8ToUtf16(utf16Filename, 256, filename);

    Effekseer::ManagerRef effekseerMgr = EffectManager::instance()->getEffekseerMgr();
    
    // Effekseer のエフェクトを読み込み
    effekseerEffect = Effekseer::Effect::Create(effekseerMgr, (EFK_CHAR*)utf16Filename);
}

// 再生
Effekseer::Handle Effect::play(const DirectX::XMFLOAT3* position, float scale)
{
    Effekseer::ManagerRef effekseerMgr = EffectManager::instance()->getEffekseerMgr();

    // 再生と位置設定
    Effekseer::Handle handle = effekseerMgr->Play(effekseerEffect, position->x, position->y, position->z);

    // 拡大率の変更
    effekseerMgr->SetScale(handle, scale, scale, scale);

    return handle;
}

// 停止
void Effect::stop(Effekseer::Handle handle)
{
    Effekseer::ManagerRef effekseerMgr = EffectManager::instance()->getEffekseerMgr();

    effekseerMgr->StopEffect(handle);
}

// 位置設定
void Effect::setPosition(Effekseer::Handle handle, const DirectX::XMFLOAT3* position)
{
    Effekseer::ManagerRef effekseerMgr = EffectManager::instance()->getEffekseerMgr();

    effekseerMgr->SetLocation(handle, position->x, position->y, position->z);
}

// 拡大縮小設定
void Effect::setScale(Effekseer::Handle handle, const DirectX::XMFLOAT3* scale)
{
    Effekseer::ManagerRef effekseerMgr = EffectManager::instance()->getEffekseerMgr();

    effekseerMgr->SetScale(handle, scale->x, scale->y, scale->z);
}