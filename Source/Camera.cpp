#include "Camera.h"

// カメラの位置と姿勢の設定
void Camera::setLookAt(const DirectX::XMFLOAT3& eye, const DirectX::XMFLOAT3& focus, const DirectX::XMFLOAT3& up)
{
    // 視点、注視点、上方向からビュー行列を作成
    DirectX::XMVECTOR Eye = DirectX::XMLoadFloat3(&eye);
    DirectX::XMVECTOR Focus = DirectX::XMLoadFloat3(&focus);
    DirectX::XMVECTOR Up = DirectX::XMLoadFloat3(&up);
    DirectX::XMMATRIX View = DirectX::XMMatrixLookAtLH(Eye, Focus, Up);
    DirectX::XMStoreFloat4x4(&view, View);

    // ビュー行列を逆行列計算し、ワールド行列に戻す
    DirectX::XMMATRIX World = DirectX::XMMatrixInverse(nullptr, View);
    DirectX::XMFLOAT4X4 world;
    DirectX::XMStoreFloat4x4(&world, World);

    // カメラの前方向を取り出す
    this->front.x = world._31;
    this->front.y = world._32;
    this->front.z = world._33;

    // カメラの右方向を取り出す
    this->right.x = world._11;
    this->right.y = world._12;
    this->right.z = world._13;


    // 視点、注視点を保存
    this->eye = eye;
    this->focus = focus;
}

// カメラのレンズの設定
void Camera::setPerspectiveFov(float fovY, float aspect, float nearZ, float farZ)
{
    // 視野角、縦横比率、クリップ距離からプロジェクション行列を作成
    DirectX::XMMATRIX Projection = DirectX::XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
    DirectX::XMStoreFloat4x4(&projection, Projection);
}