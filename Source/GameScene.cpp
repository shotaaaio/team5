
#include "GameScene.h"

#include "DeviceManager.h"
#include "Graphics/GraphicsManager.h"
#include "Camera.h"
#include "EnemyManager.h"
#include "EnemySlime.h"
#include "Effect/EffectManager.h"
#include "StageManager.h"
#include "Input/InputManager.h"
#include "PlayerManager.h"
#include "Graphics/ImGuiRenderer.h" 
#include "Ivy.h"
#include "StageMain.h"
#include "ObjManager.h"
#include "StageWall.h"
#include "Goal.h"
//#include "Graphics/LightManager.h"

// 初期化
void GameScene::initialize()
{
	// シーン定数バッファの作成
	createBuffer<GameScene::SceneConstants>(DeviceManager::instance()->getDevice(), buffer.GetAddressOf());

	// ステージの作成
	StageManager* staMgr = StageManager::instance();

	StageMain* stage = new StageMain();
	staMgr->regist(stage);

	std::vector<DirectX::XMFLOAT3> positions = {
		{-3,0,-5},
		{7,0,1.5},
		{-3,0,8},
		{-12.5,0,1}
		//{-12.5,5,5}
	};
	std::vector<DirectX::XMFLOAT3> angles = {
		{0,0,0},
		{0,DirectX::XMConvertToRadians(90),0},
		{0,0,0},
		{0,DirectX::XMConvertToRadians(90),0}
		//{0,DirectX::XMConvertToRadians(90),0}
	};
	std::vector<DirectX::XMFLOAT3> scales = {
		{10,5,0.5},
		{7,5,0.5},
		{10,5,0.5},
		{10,5,0.5}
		//{2.5,5,0.5}
	};
	for (size_t i = 0; i < positions.size(); ++i) {
		StageWall* stageWall = new StageWall();
		stageWall->setPosition(positions[i]);  // i番目の位置
		stageWall->setAngle(angles[i]);       // i番目の角度
		stageWall->setScale(scales[i]);       // i番目の大きさ
		staMgr->regist(stageWall);
	}

	// プレイヤーの作成
	PlayerManager* plaMgr = PlayerManager::instance();

	Player* player = new Player();
	plaMgr->regist(player);

	// カメラ初期設定
	DeviceManager* deviceMgr = DeviceManager::instance();
	Camera* camera = Camera::instance();
	camera->setLookAt(
		DirectX::XMFLOAT3(0, 10, -10),	// カメラの視点（位置）
		DirectX::XMFLOAT3(0, 0, 0),		// カメラの注視点（ターゲット）
		DirectX::XMFLOAT3(0, 1, 0)		// カメラの上方向
	);
	camera->setPerspectiveFov(
		DirectX::XMConvertToRadians(45),	// 視野角
		deviceMgr->getScreenWidth() / deviceMgr->getScreenHeight(),	// 画面縦横比率
		0.1f,		// カメラが映し出すの最近距離
		1000.0f		// カメラが映し出すの最遠距離
	);

	// カメラ操作の初期化
	cameraCtrl = new CameraController();

	// 敵管理クラスに取り付け
	EnemyManager* eneMgr = EnemyManager::instance();

	// 敵の作成と初期化
	for (int i = 0; i < 2; i++)
	{
		EnemySlime* slime = new EnemySlime();
		slime->setPosition({ i * 2.0f,0,5 });
		eneMgr->regist(slime);
	}

	Ivy* ivy = new Ivy();
	ivy->setPosition({ -12, 0, 0 });
	ObjManager::instance()->regist(ivy);

	Goal* goal = new Goal();
	goal->setPosition({ -18, 0, 0 });
	ObjManager::instance()->regist(goal);

	// ゲージ用スプライト（nullptr を設定するとダミーのテクスチャが生成されるように実装しています）
	gauge = new Sprite(deviceMgr->getDevice(), nullptr);

	for (int i = 0; i < 20; i++)
	{
		clickGauge[i] = false;
	}
}

// 終了処理
void GameScene::finalize()
{
	StageManager::instance()->clear();

	// ゲージ用スプライトを破棄
	if (gauge != nullptr)
	{
		delete gauge;
		gauge = nullptr;

	}

	// 敵の破棄
	EnemyManager::instance()->clear();
	PlayerManager::instance()->clear();
	ObjManager::instance()->clear();

	// カメラ操作の破棄
	if (cameraCtrl != nullptr)
	{
		delete cameraCtrl;
		cameraCtrl = nullptr;
	}
}

// 更新処理
void GameScene::update(float elapsedTime)
{
	// ステージ更新処理
	StageManager::instance()->update(elapsedTime);

	// タッチによる敵の配置
	enemyPlacementByTouch(DeviceManager::instance()->getDeviceContext(), elapsedTime);

	// 敵更新処理
	EnemyManager::instance()->update(elapsedTime);
	PlayerManager::instance()->update(elapsedTime);
	ObjManager::instance()->update(elapsedTime);

	// エフェクト更新処理
	EffectManager::instance()->update(elapsedTime);
	
	// カメラ操作の更新
	DirectX::XMFLOAT3 target = *(PlayerManager::instance()->getEnemy(0)->getPosition());
	target.y += 0.5f;	// プレイヤーの腰のあたりに注視点を設定
	cameraCtrl->setTarget(target);
	cameraCtrl->update(elapsedTime);
}


// 描画処理
void GameScene::render()
{
	DeviceManager* mgr = DeviceManager::instance();
	GraphicsManager* graphics = GraphicsManager::instance();

	ID3D11DeviceContext* dc = mgr->getDeviceContext();
	ID3D11RenderTargetView* rtv = mgr->getRenderTargetView();
	ID3D11DepthStencilView* dsv = mgr->getDepthStencilView();

	// 画面クリア＆レンダーターゲット設定
	FLOAT color[] = { 0.0f, 0.0f, 0.5f, 1.0f };	// RGBA(0.0～1.0)
	dc->ClearRenderTargetView(rtv, color);
	dc->ClearDepthStencilView(dsv, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	dc->OMSetRenderTargets(1, &rtv, dsv);

	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();

	// 3D モデルの描画に必要な情報
	SceneConstants sc;

	{
		// ビュー行列
		DirectX::XMMATRIX View = DirectX::XMLoadFloat4x4(camera->getView());
		// プロジェクション行列
		DirectX::XMMATRIX Projection = DirectX::XMLoadFloat4x4(camera->getProjection());

		DirectX::XMStoreFloat4x4(&sc.viewProjection, View * Projection);

		// ライト方向（下方向）
		sc.lightDirection = { 0.0f, -1.0f, 0.0f, 0.0f };
	}

	// 3D 描画設定
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc){
		// サンプラーステートの設定（アニソトロピック）
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::ANISOTROPIC)].GetAddressOf());
		// ブレンドステートの設定（アルファ）
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステートの設定（深度テストオン、深度書き込みオン）
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::ON_ON)].Get(), 0);
		// ラスタライザステートの設定（ソリッド、裏面表示オフ）
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
	});

	// 3D 描画
	{
		// 3D 描画に利用する定数バッファの更新と設定
		bindBuffer(dc, 1, buffer.GetAddressOf(), &sc);
		
		// ステージの描画
		StageManager::instance()->render(dc);

		// 敵描画
		EnemyManager::instance()->render(dc);
		PlayerManager::instance()->render(dc);
		ObjManager::instance()->render(dc);
	}

	// 3D エフェクト描画
	{
		EffectManager::instance()->render(view, proj);
	}

	// 3Dデバッグ描画
	{
		// 敵衝突判定用デバッグ描画
		EnemyManager::instance()->drawDebugPrimitive();
		PlayerManager::instance()->drawDebugPrimitive();

		PlayerManager::instance()->drawDebugGui();
		ImGui::Begin("gauge");

		
		
		



		ImGui::End();

		// ラインレンダラ描画実行
		graphics->getLineRenderer()->render(dc, *view, *proj);

		// デバッグレンダラ描画実行
		graphics->getDebugRenderer()->render(dc, *view, *proj);
	}

	// 2D 描画設定
	graphics->SettingRenderContext([](ID3D11DeviceContext* dc, RenderContext* rc) {
		// サンプラーステートの設定（リニア）
		dc->PSSetSamplers(0, 1, rc->samplerStates[static_cast<uint32_t>(SAMPLER_STATE::LINEAR)].GetAddressOf());
		// ブレンドステートの設定（アルファ）
		dc->OMSetBlendState(rc->blendStates[static_cast<uint32_t>(BLEND_STATE::ALPHABLENDING)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステートの設定（深度テストオフ、深度書き込みオフ）
		dc->OMSetDepthStencilState(rc->depthStencilStates[static_cast<uint32_t>(DEPTH_STENCIL_STATE::OFF_OFF)].Get(), 0);
		// ラスタライザステートの設定（ソリッド、裏面表示オフ）
		dc->RSSetState(rc->rasterizerStates[static_cast<uint32_t>(RASTERIZER_STATE::SOLID_CULLNONE)].Get());
	});

	// 2D 描画
	{
		RenderEnemyGauge(dc, view, proj);
	}

	// 2DデバッグGUI描画
	{
		
	}
}

// 敵ライフゲージの描画
void GameScene::RenderEnemyGauge(
	ID3D11DeviceContext* dc,
	const DirectX::XMFLOAT4X4* view,
	const DirectX::XMFLOAT4X4* projection)
{
	// ビューポート取得
	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	// 変換行列
	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(view);
	DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(projection);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	// マウス入力
	Mouse* mouse = InputManager::instance()->getMouse();
	static Enemy* selectedEnemy = nullptr;
	static bool isDragging = false;

	// 敵マネージャー取得
	EnemyManager* enemyMgr = EnemyManager::instance();
	int enemyCount = enemyMgr->getEnemyCount();

	for (int i = 0; i < enemyCount; ++i)
	{
		Enemy* enemy = enemyMgr->getEnemy(i);

		// エネミーの頭上のワールド座標
		DirectX::XMFLOAT3 worldPosition = *enemy->getPosition();
		worldPosition.y += enemy->getHeight();

		DirectX::XMVECTOR worldPositionVec = DirectX::XMLoadFloat3(&worldPosition);

		// ワールド座標からスクリーン座標へ変換
		DirectX::XMVECTOR screenPositionVec = DirectX::XMVector3Project(
			worldPositionVec,
			viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height,
			viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		// スクリーン座標取得
		DirectX::XMFLOAT2 screenPosition;
		DirectX::XMStoreFloat2(&screenPosition, screenPositionVec);

		// ゲージの長さ
		const float gaugeWidth = 30.0f;
		const float gaugeHeight = 5.0f;

		float healthRate = enemy->getHealth() / static_cast<float>(enemy->getMaxHealth());

		if (!clickGauge[i])
		{
			gaugeOffsets[enemy] = { screenPosition.x - gaugeWidth * 0.5f, screenPosition.y - gaugeHeight };
		}

		float& gaugeOffsetX = gaugeOffsets[enemy].x;
		float& gaugeOffsetY = gaugeOffsets[enemy].y;

		// マウス選択処理
		if (mouse->getButtonDown() & Mouse::BTN_LEFT)
		{
			float mouseX = static_cast<float>(mouse->getPositionX());
			float mouseY = static_cast<float>(mouse->getPositionY());

			// マウスがゲージの範囲内にあるかチェック
			if (mouseX >= gaugeOffsetX && mouseX <= gaugeOffsetX + gaugeWidth &&
				mouseY >= gaugeOffsetY && mouseY <= gaugeOffsetY + gaugeHeight)
			{
				selectedEnemy = enemy;
				isDragging = true;
			}
		}

		// ドラッグ中に位置更新
		if (isDragging && selectedEnemy == enemy && (mouse->getButton() & Mouse::BTN_LEFT))
		{
			gaugeOffsetX = static_cast<float>(mouse->getPositionX()) - gaugeWidth * 0.5f;
			gaugeOffsetY = static_cast<float>(mouse->getPositionY()) - gaugeHeight;
			clickGauge[i] = true;
		}

		// マウスボタンを離したらドロップ
		if (isDragging && selectedEnemy == enemy && !(mouse->getButton() & Mouse::BTN_LEFT))
		{
			isDragging = false;
			selectedEnemy = nullptr;
			InputManager::instance()->attack = false;
		}

		// ゲージ描画
		gauge->render(
			dc,
			gaugeOffsetX, gaugeOffsetY,
			gaugeWidth * healthRate, gaugeHeight,
			1.0f, 0.0f, 0.0f, 1.0f,
			0.0f,
			0, 0,
			static_cast<float>(gauge->getTextureWidth()),
			static_cast<float>(gauge->getTextureHeight())
		);
	}
}

// タッチによる敵の配置
void GameScene::enemyPlacementByTouch(ID3D11DeviceContext* dc, float elapsedTime)
{
	static Enemy* selectedEnemy = nullptr;
	static bool isDragging = false;

	Camera* camera = Camera::instance();
	const DirectX::XMFLOAT4X4* view = camera->getView();
	const DirectX::XMFLOAT4X4* proj = camera->getProjection();

	D3D11_VIEWPORT viewport;
	UINT numViewports = 1;
	dc->RSGetViewports(&numViewports, &viewport);

	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(view);
	DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(proj);
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	Mouse* mouse = InputManager::instance()->getMouse();
	/*const float doubleClickThreshold = 0.3f;
	if (click) lastClickTime += elapsedTime;
	if (lastClickTime > doubleClickThreshold)
	{
		click = !click;
		lastClickTime = 0.0f;
	}*/
	
	// --- 敵選択処理 ---
	if (mouse->getButtonDown() & Mouse::BTN_LEFT)
	{
		/*click = !click;
		if (click) return;
		if (lastClickTime > doubleClickThreshold)
		{
			lastClickTime = 0.0f;
			return;
		}
		lastClickTime = 0.0f;*/
		DirectX::XMFLOAT3 screenPosition{
			static_cast<float>(mouse->getPositionX()),
			static_cast<float>(mouse->getPositionY()),
			0.0f
		};
		
		DirectX::XMVECTOR screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec, viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, worldPositionVec);

		screenPosition.z = 1.0f;
		screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec, viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, worldPositionVec);

		if (!isDragging)
		{
			selectedEnemy = EnemyManager::instance()->getEnemyByRay(rayStart, rayEnd);
			if (selectedEnemy)
			{
				isDragging = true;
			}
		}
	}

	// --- ドラッグ中の敵の位置を更新 ---
	if (isDragging && selectedEnemy && mouse->getButton() & Mouse::BTN_LEFT)
	{
		DirectX::XMFLOAT3 screenPosition{
			static_cast<float>(mouse->getPositionX()),
			static_cast<float>(mouse->getPositionY()),
			0.0f
		};

		DirectX::XMVECTOR screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec, viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		DirectX::XMFLOAT3 rayStart;
		DirectX::XMStoreFloat3(&rayStart, worldPositionVec);

		screenPosition.z = 1.0f;
		screenPositionVec = DirectX::XMLoadFloat3(&screenPosition);
		worldPositionVec = DirectX::XMVector3Unproject(
			screenPositionVec, viewport.TopLeftX, viewport.TopLeftY,
			viewport.Width, viewport.Height, viewport.MinDepth, viewport.MaxDepth,
			projMat, viewMat, worldMat
		);

		DirectX::XMFLOAT3 rayEnd;
		DirectX::XMStoreFloat3(&rayEnd, worldPositionVec);

		/*HitResult hit;
		if (stage->raycast(rayStart, rayEnd, hit))
		{
			selectedEnemy->setPosition(hit.position);
		}*/
	}

	// --- ドロップ処理 ---
	if (isDragging && selectedEnemy && !(mouse->getButton() & Mouse::BTN_LEFT))
	{
		isDragging = false;
		selectedEnemy = nullptr;
	}
}