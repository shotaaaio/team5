#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl.h>
#include "Stage.h"
#include "Player.h"
#include "CameraController.h"
#include "Scene.h"
#include "Graphics/Sprite.h"
#include "EnemyManager.h"

// ゲームシーン
class GameScene :public Scene
{
public:
	GameScene() {}
	~GameScene() override {}

	// 初期化
	void initialize() override;

	// 終了処理
	void finalize() override;

	// 更新処理
	void update(float elapsedTime) override;

	// 描画処理
	void render() override;

private:
	// タッチによる敵の出撃
	void enemyPlacementByTouch(ID3D11DeviceContext* dc, float elapsedTime);

	// 敵ライフゲージ描画
	void RenderEnemyGauge(
		ID3D11DeviceContext* dc,
		const DirectX::XMFLOAT4X4* view,
		const DirectX::XMFLOAT4X4* projection
	);

private:
	// シーン定数
	struct SceneConstants
	{
		DirectX::XMFLOAT4X4		viewProjection;
		DirectX::XMFLOAT4		lightDirection;
	};
	// シーン定数バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	//Stage* stage;
	//Player* player = nullptr;
	CameraController* cameraCtrl = nullptr;
	Sprite* gauge = nullptr;

	float lastClickTime = 0.0f;
	bool click = false;

	bool clickGauge[20];
	std::unordered_map<Enemy*, DirectX::XMFLOAT2> gaugeOffsets;
};
