#include "SceneManager.h"

// 更新処理
void SceneManager::update(float elapsedTime)
{
    // 「次のシーン」が設定されているか？
    if (nextScene != nullptr)
    {
        // 「現在のシーン」を破棄
        clear();

        // 「次のシーン」を「現在のシーン」として設定
        currentScene = nextScene;

        // 「次のシーン」を nullptr を代入
        nextScene = nullptr;

        // シーンを初期化
        if (currentScene->isReady() == false)
        {
            currentScene->initialize();
        }
    }

    if (currentScene != nullptr)
    {
        currentScene->update(elapsedTime);
    }
}

// 描画処理
void SceneManager::render()
{
    if (currentScene != nullptr)
    {
        currentScene->render();
    }
}

// 現在のシーンをクリア
void SceneManager::clear()
{
    if (currentScene != nullptr)
    {
        currentScene->finalize();
        delete currentScene;
        currentScene = nullptr;
    }
}

// シーンの切り替え（外部で生成したシーンを次のシーンとして設定）
void SceneManager::changeScene(Scene* scene)
{
    // 次のシーンとして設定
    nextScene = scene;
}