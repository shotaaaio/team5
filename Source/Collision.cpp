#include "Collision.h"

/// <summary>
/// 球と球の交差判定
/// </summary>
/// <param name="positionA">球A</param>
/// <param name="radiusA">半径A</param>
/// <param name="positionB">球B</param>
/// <param name="radiusB">半径B</param>
/// <param name="outVec">衝突方向のベクトル（出力用）</param>
/// <returns>true...衝突した</returns>
bool Collision::intersectSphererAndSphere(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    DirectX::XMFLOAT3& outVec)
{
    DirectX::XMVECTOR posA = DirectX::XMLoadFloat3(&positionA);
    DirectX::XMVECTOR posB = DirectX::XMLoadFloat3(&positionB);
    // ２点間の位置ベクトル引く
    DirectX::XMVECTOR vec = DirectX::XMVectorSubtract(posB, posA);
    // 単一の長さを計算
    DirectX::XMVECTOR length = DirectX::XMVector3Length(vec);
    // float 型に変換
    float len;
    DirectX::XMStoreFloat(&len, length);

    // 距離判定
    float range = radiusA + radiusB;
    if (len > range)
    {
        return false;
    }

    // 衝突していたら衝突方向のベクトルを設定
    vec = DirectX::XMVector3Normalize(vec);
    DirectX::XMStoreFloat3(&outVec, vec);

    return true;
}

bool Collision::intersectCylinderAndCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    float heightA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outVec)
{
    // A の足もとが B の頭より上なら衝突していない
    if (positionA.y > positionB.y + heightB)
        return false;

    // A の頭が B の足もとより下なら衝突していない
    if (positionA.y + heightA < positionB.y)
        return false;

    // XZ 平面での範囲チェック
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx * vx + vz * vz);
    if (distXZ > range)
        return false;

    // 衝突方向のベクトル
    outVec.x = vx / distXZ;
    outVec.y = 0.0f;
    outVec.z = vz / distXZ;

    return true;
}

bool Collision::intersectSphereAndCylinder(
    const DirectX::XMFLOAT3& positionA,
    float radiusA,
    const DirectX::XMFLOAT3& positionB,
    float radiusB,
    float heightB,
    DirectX::XMFLOAT3& outVec)
{
    // A（球）の足もとが B（円柱）の頭より上なら衝突していない
    if (positionA.y - radiusA > positionB.y + heightB)
        return false;

    // A（球）の頭が B（円柱）の足もとより下なら衝突していない
    if (positionA.y + radiusA < positionB.y)
        return false;

    // XZ 平面での範囲チェック
    float vx = positionB.x - positionA.x;
    float vz = positionB.z - positionA.z;
    float range = radiusA + radiusB;
    float distXZ = sqrtf(vx * vx + vz * vz);
    if (distXZ > range)
        return false;

    // 衝突方向のベクトル
    outVec.x = vx / distXZ;
    outVec.y = 0.0f;
    outVec.z = vz / distXZ;

    return true;
}

bool Collision::intersectRayAndModel(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const SkinnedMesh* model,
    HitResult& result)
{
    // ワールド空間上でのレイの始点
    DirectX::XMVECTOR worldRayStartVec = DirectX::XMLoadFloat3(&start);
    // ワールド空間上でのレイの終点
    DirectX::XMVECTOR worldRayEndVec = DirectX::XMLoadFloat3(&end);
    // ワールド空間上でのレイの始点から終点までのベクトル
    DirectX::XMVECTOR worldRayVec = DirectX::XMVectorSubtract(worldRayEndVec, worldRayStartVec);
    // ワールド空間上でのレイの長さ
    DirectX::XMVECTOR worldRayLength = DirectX::XMVector3Length(worldRayVec);
    DirectX::XMStoreFloat(&result.distance, worldRayLength);

    // true...衝突した
    bool hit = false;
    for (const SkinnedMesh::mesh& mesh : model->meshes)
    {
        // レイをワールド空間からローカル空間へ変換
        DirectX::XMMATRIX worldTransformMat = DirectX::XMLoadFloat4x4(&mesh.default_global_transform);
        DirectX::XMMATRIX inverseWorldTransformMat = DirectX::XMMatrixInverse(nullptr, worldTransformMat);

        // ローカル空間上でのレイの始点
        DirectX::XMVECTOR localRayStartVec = DirectX::XMVector3TransformCoord(worldRayStartVec, inverseWorldTransformMat);
        // ローカル空間上でのレイの終点
        DirectX::XMVECTOR localRayEndVec = DirectX::XMVector3TransformCoord(worldRayEndVec, inverseWorldTransformMat);
        // ローカル空間上でのレイの始点から終点までのベクトル
        DirectX::XMVECTOR localRayVec = DirectX::XMVectorSubtract(localRayEndVec, localRayStartVec);
        // ローカル空間上でのレイの方向ベクトル
        DirectX::XMVECTOR localRayDirectVec = DirectX::XMVector3Normalize(localRayVec);
        // ローカル空間上でのレイの長さ
        DirectX::XMVECTOR localRayLengthVec = DirectX::XMVector3Length(localRayVec);
        float localRayLength;
        DirectX::XMStoreFloat(&localRayLength, localRayLengthVec);

        //　頂点データを取得
        const std::vector<SkinnedMesh::vertex>& vertices = mesh.vertices;
        const std::vector<UINT>& indices = mesh.indices;

        // 候補となる情報
        int materialIndex = -1;
        DirectX::XMVECTOR hitPosition;
        DirectX::XMVECTOR hitNormal;

        for (const SkinnedMesh::mesh::subset& subset : mesh.subsets)
        {
            for (UINT i = 0; i < subset.index_count; i += 3)
            {
                UINT index = subset.start_index_location + i;

                // 三角形の頂点の抽出
                const SkinnedMesh::vertex& a = vertices.at(indices.at(index));
                const SkinnedMesh::vertex& b = vertices.at(indices.at(index + 1));
                const SkinnedMesh::vertex& c = vertices.at(indices.at(index + 2));

                DirectX::XMVECTOR aVec = DirectX::XMLoadFloat3(&a.position);
                DirectX::XMVECTOR bVec = DirectX::XMLoadFloat3(&b.position);
                DirectX::XMVECTOR cVec = DirectX::XMLoadFloat3(&c.position);

                DirectX::XMVECTOR abVec = DirectX::XMVectorSubtract(bVec, aVec);
                DirectX::XMVECTOR bcVec = DirectX::XMVectorSubtract(cVec, bVec);
                DirectX::XMVECTOR caVec = DirectX::XMVectorSubtract(aVec, cVec);

                // 三角形の法線を計算
                DirectX::XMVECTOR normalVec = DirectX::XMVector3Cross(abVec, bcVec);

                // レイの方向と面の方向をチェック（平面の表裏判定）
                DirectX::XMVECTOR dotVec = DirectX::XMVector3Dot(localRayDirectVec, normalVec);
                float dot;
                DirectX::XMStoreFloat(&dot, dotVec);
                if (dot >= 0)
                    continue;

                // レイの始点から交点までの長さを計算
                DirectX::XMVECTOR tmp = DirectX::XMVectorSubtract(aVec, localRayStartVec);
                float length = DirectX::XMVectorGetX(DirectX::XMVector3Dot(tmp, normalVec)) / dot;

                // 長さが 0 以下なら以降の処理は飛ばす
                if (length < 0.0f)
                    continue;

                // 交点までの距離が今までに計算した最近距離より大きい時はスキップ
                if (length > localRayLength)
                    continue;

                // レイと平面の交点を計算
                DirectX::XMVECTOR hitPosVec = DirectX::XMVectorAdd(localRayStartVec, DirectX::XMVectorScale(localRayDirectVec, length));

                // 交点が三角形の内側にあるかどうか判定

                   // 交点から点 A までのベクトル
                DirectX::XMVECTOR paVec = DirectX::XMVectorSubtract(aVec, hitPosVec);
                // 交点から点 A までのベクトルと点A点Bのベクトルの外積
                DirectX::XMVECTOR crossVec1 = DirectX::XMVector3Cross(paVec, abVec);
                // 外積と法線の向きをチェック
                DirectX::XMVECTOR dotVec1 = DirectX::XMVector3Dot(crossVec1, normalVec);
                float dot1;
                DirectX::XMStoreFloat(&dot1, dotVec1);
                if (dot1 < 0.0f)
                    continue;

                // 交点から点 B までのベクトル
                DirectX::XMVECTOR pbVec = DirectX::XMVectorSubtract(bVec, hitPosVec);
                // 交点から点 B までのベクトルと点B点Cのベクトルの外積
                DirectX::XMVECTOR crossVec2 = DirectX::XMVector3Cross(pbVec, bcVec);
                // 外積と法線の向きをチェック
                DirectX::XMVECTOR dotVec2 = DirectX::XMVector3Dot(crossVec2, normalVec);
                float dot2;
                DirectX::XMStoreFloat(&dot2, dotVec2);
                if (dot2 < 0.0f)
                    continue;

                // 交点から点 C までのベクトル
                DirectX::XMVECTOR pcVec = DirectX::XMVectorSubtract(cVec, hitPosVec);
                // 交点から点 C までのベクトルと点C点Aのベクトルの外積
                DirectX::XMVECTOR crossVec3 = DirectX::XMVector3Cross(pcVec, caVec);
                // 外積と法線の向きをチェック
                DirectX::XMVECTOR dotVec3 = DirectX::XMVector3Dot(crossVec3, normalVec);
                float dot3;
                DirectX::XMStoreFloat(&dot3, dotVec3);
                if (dot3 < 0.0f)
                    continue;

                // 最近距離を更新
                localRayLength = length;

                // マテリアル番号を更新
                materialIndex = subset.material_unique_id;

                // 交点と法線を更新
                hitPosition = hitPosVec;
                hitNormal = normalVec;
            }
        }

        if (materialIndex >= 0)
        {
            // 交点座標をローカル空間からワールド空間へ変換
            DirectX::XMVECTOR worldPositionVec = DirectX::XMVector3TransformCoord(hitPosition, worldTransformMat);
            // ワールド空間上でのレイの始点から交点までのベクトル
            DirectX::XMVECTOR worldVec = DirectX::XMVectorSubtract(worldPositionVec, worldRayStartVec);
            // ワールド空間上でのレイの視点から交点までの長さ
            DirectX::XMVECTOR worldLengthVec = DirectX::XMVector3Length(worldVec);
            float distance;
            DirectX::XMStoreFloat(&distance, worldLengthVec);

            // ヒット結果情報保存
            if (result.distance > distance)
            {
                // ヒット時の面の法線をローカル空間からワールド空間へ変換
                DirectX::XMVECTOR worldNormal = DirectX::XMVector3TransformNormal(hitNormal, worldTransformMat);

                result.distance = distance;
                result.materialIndex = materialIndex;
                DirectX::XMStoreFloat3(&result.position, worldPositionVec);
                DirectX::XMStoreFloat3(&result.normal, DirectX::XMVector3Normalize(worldNormal));
                hit = true;
            }
        }
    }

    return hit;
}

bool Collision::intersectPrizmAndPrizm(
    const DirectX::XMFLOAT3& positionA, // 直方体Aの底面中心
    float radiusA,                     // 直方体Aの半分の横幅（X,Z）
    float heightA,                     // 直方体Aの高さ
    const DirectX::XMFLOAT3& positionB, // 直方体Bの底面中心
    float radiusB,                     // 直方体Bの半分の横幅（X,Z）
    float heightB,                     // 直方体Bの高さ
    DirectX::XMFLOAT3& outVec)          // 衝突解決用補正ベクトル（XZ軸のみ）
{
    // 垂直方向（Y軸）の重なりチェック
    // A の底面が B の上面より上なら衝突なし
    if (positionA.y > positionB.y + heightB)
        return false;
    // A の上面が B の底面より下なら衝突なし
    if (positionA.y + heightA < positionB.y)
        return false;

    // XZ平面での各軸の重なりチェック
    float dx = fabs(positionA.x - positionB.x);
    float dz = fabs(positionA.z - positionB.z);
    float rangeX = radiusA + radiusB;
    float rangeZ = radiusA + radiusB; // 四角柱の場合、横幅は同じとしてチェック

    if (dx > rangeX || dz > rangeZ)
        return false;

    // 各軸ごとの浸透深さ（penetration depth）を計算
    float penX = rangeX - dx;
    float penZ = rangeZ - dz;

    // どちらの軸で解決するか、より小さい方を選択
    if (penX < penZ)
    {
        outVec.x = (positionA.x < positionB.x) ? -penX : penX;
        outVec.y = 0.0f;
        outVec.z = 0.0f;
    }
    else
    {
        outVec.x = 0.0f;
        outVec.y = 0.0f;
        outVec.z = (positionA.z < positionB.z) ? -penZ : penZ;
    }

    return true;
}

bool Collision::intersectBoxAndBox(
    const DirectX::XMFLOAT3& start,
    const DirectX::XMFLOAT3& end,
    const DirectX::XMFLOAT3& box1Min,
    const DirectX::XMFLOAT3& box1Max,
    const DirectX::XMFLOAT3& box2Min,
    const DirectX::XMFLOAT3& box2Max,
    HitResult& result)
{
    // ワールド空間上でのレイの始点と終点
    DirectX::XMVECTOR worldRayStartVec = DirectX::XMLoadFloat3(&start);
    DirectX::XMVECTOR worldRayEndVec = DirectX::XMLoadFloat3(&end);
    DirectX::XMVECTOR worldRayVec = DirectX::XMVectorSubtract(worldRayEndVec, worldRayStartVec);
    DirectX::XMVECTOR worldRayLength = DirectX::XMVector3Length(worldRayVec);
    DirectX::XMStoreFloat(&result.distance, worldRayLength);

    bool hit = false;

    // 2つの箱の6面を定義
    std::vector<DirectX::XMFLOAT3> box1Faces = {
        {box1Min.x, box1Min.y, box1Min.z}, {box1Max.x, box1Min.y, box1Min.z},
        {box1Max.x, box1Max.y, box1Min.z}, {box1Min.x, box1Max.y, box1Min.z},
        {box1Min.x, box1Min.y, box1Max.z}, {box1Max.x, box1Min.y, box1Max.z},
    };

    std::vector<DirectX::XMFLOAT3> box2Faces = {
        {box2Min.x, box2Min.y, box2Min.z}, {box2Max.x, box2Min.y, box2Min.z},
        {box2Max.x, box2Max.y, box2Min.z}, {box2Min.x, box2Max.y, box2Min.z},
        {box2Min.x, box2Min.y, box2Max.z}, {box2Max.x, box2Min.y, box2Max.z},
    };

    // それぞれの面に対して交差判定
    for (size_t i = 0; i < box1Faces.size(); ++i)
    {
        // 交差判定処理（四角柱の面とレイの交差判定）
        // ここでは単純な法線方向で面とレイの交差判定を行いますが、実際には
        // 面の法線とレイの方向ベクトルのドット積を計算し、交差するかどうかを判定します。
        // レイと面の交差判定を行う処理をここに追加してください。

        // 交差していた場合
        if (true)  // 実際の交差判定の条件をここに記述
        {
            hit = true;
            // ヒット結果を保存
            // 交点の座標、法線などを計算し、resultに格納します
        }
    }

    return hit;
}