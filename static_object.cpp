
#include "framework.h"
#include "static_object.h"
#include "light.h"

static_object::static_object(std::shared_ptr<static_mesh>& smesh, bool DSLflg)
{
	mesh = smesh;//

	if (DSLflg)
	{
		this->DSLflg = DSLflg;
		DSLMode = 0;
		base_shading = std::make_unique<BASE_SHADING>();
		base_shading.get()->Init(FRAMEWORK.getDevice(), L"dynamic_shader_link_basic.fx", "VSMain", "PSMain");
	}
	setPosition(DirectX::XMFLOAT3(0, 0, 0.0f));
	setAngle(DirectX::XMFLOAT3(0, 0, 0));
	setScale(DirectX::XMFLOAT3(1, 1, 1));
}

static_object::~static_object()
{
	/*delete StaticObject::mesh;*/
	if (base_shading.get() != nullptr)
		base_shading.get()->End();
}

void static_object::update()
{
	//ワールド変換行列の初期化
	DirectX::XMMATRIX s, r, t;
	DirectX::XMFLOAT4X4 rm;

	//拡大行列
	s = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//回転行列
	r = DirectX::XMMatrixRotationRollPitchYaw(angle.x*(DirectX::XM_PI / 180.0f), angle.y*(DirectX::XM_PI / 180.0f), angle.z*(DirectX::XM_PI / 180.0f));
	DirectX::XMStoreFloat4x4(&rm, r);
	setRight(DirectX::XMFLOAT3(rm._11, rm._12, rm._13));
	setUp(DirectX::XMFLOAT3(rm._21, rm._22, rm._23));
	setForward(DirectX::XMFLOAT3(rm._31, rm._32, rm._33));

	//移動行列
	t = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//行列の合成と変換
	DirectX::XMStoreFloat4x4(&world, s*r*t);
}

void static_object::update(DirectX::XMFLOAT3 axis, float speed, float rotation)
{
	//ワールド変換行列の初期化
	DirectX::XMMATRIX s, r, t, r2;

	//拡大行列
	s = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//回転行列
	//r = DirectX::XMMatrixRotationRollPitchYaw(angle.x*(DirectX::XM_PI / 180.0f), angle.y*(DirectX::XM_PI / 180.0f), angle.z*(DirectX::XM_PI / 180.0f));

	//常に頭がカメラに向く回転行列
	//変換
	DirectX::XMFLOAT3 up = axis;//視線の逆ベクトル
	DirectX::XMFLOAT3 right = DirectX::XMFLOAT3(-1, 0, 0);//仮の右ベクトル
	DirectX::XMFLOAT3 forward;//

	DirectX::XMVECTOR up_v = DirectX::XMLoadFloat3(&up);
	DirectX::XMVECTOR right_v = DirectX::XMLoadFloat3(&right);

	//前方
	DirectX::XMVECTOR cross1 = DirectX::XMVector3Cross(right_v, up_v);//視線の逆ベクトルと仮の右ベクトルの外積でキャラの正面ベクトルを作る
	DirectX::XMVECTOR forward_v = cross1;
	//右
	DirectX::XMVECTOR cross2 = DirectX::XMVector3Cross(up_v, forward_v);//仮の右ベクトルを視線の逆ベクトルと前方ベクトルの外積から求めなおす
	right_v = cross2;
	//正規化
	forward_v = DirectX::XMVector3Normalize(forward_v);
	up_v = DirectX::XMVector3Normalize(up_v);
	right_v = DirectX::XMVector3Normalize(right_v);
	//回転行列に値を設定していく
	DirectX::XMFLOAT4X4 rot;
	//とりあえずfloatに変換
	DirectX::XMStoreFloat3(&forward, forward_v);
	DirectX::XMStoreFloat3(&up, up_v);
	DirectX::XMStoreFloat3(&right, right_v);

	//何も操作されてないなら、xyz各軸の向きを行列にセットすれば回転行列になる

	//xyzの順
	//x軸を表すベクトルの設定
	rot._11 = right.x;
	rot._12 = right.y;
	rot._13 = right.z;
	rot._14 = 0.0f;//回転行列なので平行移動は全部0

				   //y軸を表すベクトルの設定
	rot._21 = up.x;
	rot._22 = up.y;
	rot._23 = up.z;
	rot._24 = 0.0f;//回転行列なので平行移動は全部0

				   //z軸を表すベクトルの設定
	rot._31 = forward.x;
	rot._32 = forward.y;
	rot._33 = forward.z;
	rot._34 = 0.0f;//回転行列なので平行移動は全部0

	rot._41 = 0.0f;
	rot._42 = 0.0f;
	rot._43 = 0.0f;
	rot._44 = 1.0f;

	//***************************************//

	/*rot._11 = -right.x;
	rot._12 = -right.y;
	rot._13 = -right.z;
	rot._14 = 0.0f;
	rot._21 = -forward.x;
	rot._22 = -forward.y;
	rot._23 = -forward.z;
	rot._24 = 0.0f;
	rot._31 = -up.x;
	rot._32 = -up.y;
	rot._33 = -up.z;
	rot._34 = 0.0f;
	rot._41 = 0.0f;
	rot._42 = 0.0f;
	rot._43 = 0.0f;
	rot._44 = 1.0f;*/

	r = DirectX::XMLoadFloat4x4(&rot);

	//カメラへのベクトルを軸として
	DirectX::XMVECTOR axis_v;
	axis_v = DirectX::XMLoadFloat3(&axis);
	static float angle = 0;
	angle += rotation;
	DirectX::XMVECTOR quaternion;
	quaternion = DirectX::XMQuaternionRotationAxis(axis_v, angle);//quaternionに変換
	r2 = DirectX::XMMatrixRotationQuaternion(quaternion);//quaternion→matrixに変換

	//移動行列
	//少しずつプレイヤーの方に近づいてくる
	setPosition(DirectX::XMFLOAT3(getPosition().x + up.x / 10.0f*speed, getPosition().y + up.y / 10.0f*speed, getPosition().z + up.z / 10.0f*speed));
	t = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//行列の合成と変換
	DirectX::XMStoreFloat4x4(&world, s*r*r2*t);
}

void static_object::render(ID3D11DeviceContext *immediate_context, camera* m_camera, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color, bool wire_frame, bool cull)
{
	//ワールドビュープロジェクション作成
	DirectX::XMMATRIX wrl = DirectX::XMLoadFloat4x4(&world);
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMMATRIX wvp = wrl*DirectX::XMLoadFloat4x4(&m_camera->getView())*DirectX::XMLoadFloat4x4(&m_camera->getProjection());
	DirectX::XMStoreFloat4x4(&world_view_projection, wvp);

	//ワールド逆行列
	DirectX::XMMATRIX world_invM = DirectX::XMMatrixInverse(nullptr, wrl);
	DirectX::XMFLOAT4X4 world_inv;
	DirectX::XMStoreFloat4x4(&world_inv, world_invM);

	if (DSLflg)
	{
		base_shading.get()->SetCBVertexShader(FRAMEWORK.getImmediateContext(), &wrl, &wvp);
		switch (DSLMode)
		{
		case 0:
			// 定数シェーダーの設定
			base_shading.get()->SetCBLambert(FRAMEWORK.getImmediateContext(), &Light::LightDir);
			// シェーダーの設定
			base_shading.get()->Begin(FRAMEWORK.getImmediateContext(), "Lambert");//HLSLの定数バッファのインスタンス名
			break;
		case 1:
			// 定数シェーダーの設定
			base_shading.get()->SetCBHalfLambert(FRAMEWORK.getImmediateContext(), &Light::LightDir);
			// シェーダーの設定
			base_shading.get()->Begin(FRAMEWORK.getImmediateContext(), "HalfLambert");//HLSLの定数バッファのインスタンス名
			break;
		case 2:
			// 定数シェーダーの設定
			base_shading.get()->SetCBPhongShading(FRAMEWORK.getImmediateContext(), &Light::LightDir, &DirectX::XMFLOAT4(m_camera->getEye().x, m_camera->getEye().y, m_camera->getEye().z, 1.0f));
			// シェーダーの設定
			base_shading.get()->Begin(FRAMEWORK.getImmediateContext(), "PhongShading");//HLSLの定数バッファのインスタンス名
			break;
		case 3:
			// 定数シェーダーの設定
			base_shading.get()->SetCBNoShading(FRAMEWORK.getImmediateContext()/*, &Light::LightDir*/);
			// シェーダーの設定
			base_shading.get()->BeginNM(FRAMEWORK.getImmediateContext(), "cNoShading");//↑とは違って"HLSLのクラスの型の名前"
			break;
		}
	}

	//if (!tangent)
	mesh->render(immediate_context, world_view_projection, world_inv, light_direction, material_color, DSLflg, wire_frame, cull);
	//else
	//	mesh->render(immediate_context, world_view_projection, world_inv, light_direction, material_color, DSLflg, wire_frame, true);
}

//シェーダー指定してから描画
void static_object::render(ID3D11DeviceContext *immediate_context, shader *shader, camera *m_camera, const DirectX::XMFLOAT4& light_direction /*, const DirectX::XMFLOAT4& material_color*/, bool wire_frame, bool cull)
{
	//ワールドビュープロジェクション作成
	DirectX::XMMATRIX wrl = DirectX::XMLoadFloat4x4(&world);
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMMATRIX wvp = wrl*DirectX::XMLoadFloat4x4(&m_camera->getView())*DirectX::XMLoadFloat4x4(&m_camera->getProjection());
	DirectX::XMStoreFloat4x4(&world_view_projection, wvp);

	//定数バッファ設定
	ConstantBufferWorld cb;
	cb.world = XMLoadFloat4x4(&world);
	cb.matWVP = wvp;
	FRAMEWORK.getImmediateContext()->UpdateSubresource(shader->ConstantBuffer_w.Get(), 0, NULL, &cb, 0, 0);
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());
	//FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());//こいつが上書きしたりする
	FRAMEWORK.getImmediateContext()->GSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());
	if (shader->hs != nullptr)
		FRAMEWORK.getImmediateContext()->HSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());
	if (shader->ds != nullptr);
	FRAMEWORK.getImmediateContext()->DSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());

	mesh->render(immediate_context, shader, m_camera, world, light_direction, /*material_color,*/ wire_frame);
}

void static_object::renderTessellation(ID3D11DeviceContext *immediate_context, shader *shader, camera *m_camera, const DirectX::XMFLOAT4& light_direction /*, const DirectX::XMFLOAT4& material_color*/, bool wire_frame)
{
	//ワールドビュープロジェクション作成
	DirectX::XMMATRIX wrl = DirectX::XMLoadFloat4x4(&world);
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMMATRIX wvp = wrl*DirectX::XMLoadFloat4x4(&m_camera->getView())*DirectX::XMLoadFloat4x4(&m_camera->getProjection());
	DirectX::XMStoreFloat4x4(&world_view_projection, wvp);

	//定数バッファ設定
	ConstantBufferWorld cb;
	cb.world = XMLoadFloat4x4(&world);
	cb.matWVP = wvp;
	FRAMEWORK.getImmediateContext()->UpdateSubresource(shader->ConstantBuffer_w.Get(), 0, NULL, &cb, 0, 0);
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());
	//FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());//こいつが上書きしたりする
	FRAMEWORK.getImmediateContext()->GSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());
	if (shader->hs != nullptr)
		FRAMEWORK.getImmediateContext()->HSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());
	if (shader->ds != nullptr)
		FRAMEWORK.getImmediateContext()->DSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());

	mesh->renderTessellation(immediate_context, shader, world_view_projection, world, light_direction, /*material_color,*/ wire_frame);
}

//テクスチャ渡して描画
void static_object::render(ID3D11DeviceContext *immediate_context, shader* shader, Texture *pTex, camera* m_camera, const DirectX::XMFLOAT4& light_direction, bool wire_frame)
{
	//ワールドビュープロジェクション作成
	DirectX::XMMATRIX wrl = DirectX::XMLoadFloat4x4(&world);
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMMATRIX wvp = wrl*DirectX::XMLoadFloat4x4(&m_camera->getView())*DirectX::XMLoadFloat4x4(&m_camera->getProjection());
	DirectX::XMStoreFloat4x4(&world_view_projection, wvp);

	//定数バッファ設定
	ConstantBufferWorld cb;
	cb.world = XMLoadFloat4x4(&world);
	cb.matWVP = wvp;
	FRAMEWORK.getImmediateContext()->UpdateSubresource(shader->ConstantBuffer_w.Get(), 0, NULL, &cb, 0, 0);
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());
	//FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());//こいつが上書きしたりする
	FRAMEWORK.getImmediateContext()->GSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());

	mesh->render(immediate_context, shader, pTex, world_view_projection, world, light_direction, wire_frame);
}

//void static_object::render(ID3D11DeviceContext *immediate_context, shader* shader, camera* m_camera, environment_cubemap* env_map, const DirectX::XMFLOAT4& light_direction = DirectX::XMFLOAT4(0, 0, 1, 0),/*const DirectX::XMFLOAT4& material_color = DirectX::XMFLOAT4(1, 1, 1, 1),*/ bool wire_frame = false)
//{
//	//ワールドビュープロジェクション作成
//	DirectX::XMMATRIX wrl = DirectX::XMLoadFloat4x4(&world);
//	DirectX::XMFLOAT4X4 world_view_projection;
//	DirectX::XMMATRIX wvp = wrl*DirectX::XMLoadFloat4x4(&m_camera->getView())*DirectX::XMLoadFloat4x4(&m_camera->getProjection());
//	DirectX::XMStoreFloat4x4(&world_view_projection, wvp);
//
//	//定数バッファ設定
//	ConstantBufferWorld cb;
//	cb.world = XMLoadFloat4x4(&world);
//	cb.matWVP = wvp;
//	FRAMEWORK.getImmediateContext()->UpdateSubresource(shader->ConstantBuffer_w, 0, NULL, &cb, 0, 0);
//	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(0, 1, &shader->ConstantBuffer_w);
//	//FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(0, 1, shader->ConstantBuffer_w.GetAddressOf());//こいつが上書きしたりする
//	FRAMEWORK.getImmediateContext()->GSSetConstantBuffers(0, 1, &shader->ConstantBuffer_w);
//
//	//PBRv2用のテクスチャ
//	FRAMEWORK.getImmediateContext()->PSSetShaderResources(4, 1, env_map->getShaderResourceViewAdress());
//	FRAMEWORK.getImmediateContext()->PSSetShaderResources(5, 1, env_map->getShaderResourceViewAdress());
//	FRAMEWORK.getImmediateContext()->PSSetShaderResources(6, 1, env_map->getShaderResourceViewAdress());
//
//	mesh->render(immediate_context, shader, world_view_projection, world, light_direction, wire_frame);
//}

void static_object::move()
{
	//static DirectX::XMFLOAT3 position(0, 0, 10);
	if (GetAsyncKeyState(VK_SHIFT) < 0 && GetAsyncKeyState('M') < 0)
	{
		if (GetAsyncKeyState(VK_UP) < 0)
		{
			position.x -= getUp().x;
			position.y -= getUp().y;
			position.z -= getUp().z;
		}
		if (GetAsyncKeyState(VK_DOWN) < 0)
		{
			position.x += getUp().x;
			position.y += getUp().y;
			position.z += getUp().z;
		}
		if (GetAsyncKeyState('W') < 0)
		{
			position.x += getForward().x;
			position.y += getForward().y;
			position.z += getForward().z;
		}
		if (GetAsyncKeyState('S') < 0)
		{
			position.x -= getForward().x;
			position.y -= getForward().y;
			position.z -= getForward().z;
		}
		if (GetAsyncKeyState(VK_LEFT) < 0)
		{
			position.x += getRight().x;
			position.y += getRight().y;
			position.z += getRight().z;
		}
		if (GetAsyncKeyState(VK_RIGHT) < 0)
		{
			position.x -= getRight().x;
			position.y -= getRight().y;
			position.z -= getRight().z;
		}
	}

	//DirectX::XMFLOAT3 dimensions(1, 1, 1);

	//static DirectX::XMFLOAT3 angles(0, 0, 0);
	if (GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState('M') < 0)
	{
		if (GetAsyncKeyState(VK_UP) < 0)
			angle.x += DirectX::XM_PI / 180.0f * 10;
		if (GetAsyncKeyState(VK_DOWN) < 0)
			angle.x -= DirectX::XM_PI / 180.0f * 10;
		if (GetAsyncKeyState(VK_LEFT) < 0)
			angle.y += DirectX::XM_PI / 180.0f * 10;
		if (GetAsyncKeyState(VK_RIGHT) < 0)
			angle.y -= DirectX::XM_PI / 180.0f * 10;
		//if (GetAsyncKeyState('UP') & 1)
		angle.z = 0;
	}
}

void static_object::focusMove(camera *cam)
{
	//x,z平面において計算する
	//前後
	DirectX::XMFLOAT2 cam_to_obj = DirectX::XMFLOAT2(getPosition().x - cam->getEye().x, getPosition().z - cam->getEye().z);
	DirectX::XMVECTOR vec = DirectX::XMLoadFloat2(&cam_to_obj);
	vec = DirectX::XMVector2Normalize(vec);
	DirectX::XMStoreFloat2(&cam_to_obj, vec);
	//左右
	DirectX::XMFLOAT3 cam_to_obj3 = DirectX::XMFLOAT3(getPosition().x - cam->getEye().x, getPosition().y - cam->getEye().y, getPosition().z - cam->getEye().z);
	DirectX::XMVECTOR vec3 = DirectX::XMLoadFloat3(&cam_to_obj3);
	vec3 = DirectX::XMVector3Normalize(vec3);
	DirectX::XMVECTOR vec_up = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0, 1, 0));
	vec_up = DirectX::XMVector3Normalize(vec_up);
	DirectX::XMVECTOR vec_cross = DirectX::XMVector3Cross(vec_up, vec3);
	DirectX::XMStoreFloat3(&cam_to_obj3, vec_cross);

	if (GetAsyncKeyState('W') < 0)
		setPosition(DirectX::XMFLOAT3(getPosition().x + cam_to_obj.x / 5.0f, getPosition().y, getPosition().z + cam_to_obj.y / 5.0f));
	if (GetAsyncKeyState('S') < 0)
		setPosition(DirectX::XMFLOAT3(getPosition().x - cam_to_obj.x / 5.0f, getPosition().y, getPosition().z - cam_to_obj.y / 5.0f));
	if (GetAsyncKeyState('A') < 0)
		setPosition(DirectX::XMFLOAT3(getPosition().x - cam_to_obj3.x / 5.0f, getPosition().y, getPosition().z - cam_to_obj3.z / 5.0f));
	if (GetAsyncKeyState('D') < 0)
		setPosition(DirectX::XMFLOAT3(getPosition().x + cam_to_obj3.x / 5.0f, getPosition().y, getPosition().z + cam_to_obj3.z / 5.0f));
	if (GetAsyncKeyState('Q') < 0)
		setPosition(DirectX::XMFLOAT3(getPosition().x, getPosition().y + 0.2f, getPosition().z));
	if (GetAsyncKeyState('E') < 0)
		setPosition(DirectX::XMFLOAT3(getPosition().x, getPosition().y - 0.2f, getPosition().z));

	//DirectX::XMFLOAT3 dimensions(1, 1, 1);

	//static DirectX::XMFLOAT3 angles(0, 0, 0);
	if (GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState('M') < 0)
	{
		if (GetAsyncKeyState(VK_UP) < 0)
			angle.x += DirectX::XM_PI / 180.0f * 10;
		if (GetAsyncKeyState(VK_DOWN) < 0)
			angle.x -= DirectX::XM_PI / 180.0f * 10;
		if (GetAsyncKeyState(VK_LEFT) < 0)
			angle.y += DirectX::XM_PI / 180.0f * 10;
		if (GetAsyncKeyState(VK_RIGHT) < 0)
			angle.y -= DirectX::XM_PI / 180.0f * 10;
		//if (GetAsyncKeyState('UP') & 1)
		angle.z = 0;
	}
}

void static_object::changeDSLBasic(int mode)
{
	DSLMode = mode;
}

void static_object::setPosture(DirectX::XMFLOAT3 outNormal)
{
	//仮の３軸を外積を使って計算する
	DirectX::XMVECTOR vy = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&outNormal));//地面の法線からy軸
	DirectX::XMFLOAT3 front = DirectX::XMFLOAT3(sin(this->getAngle().y), 0, cos(this->getAngle().y));
	DirectX::XMVECTOR vz = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front));//仮のz
	DirectX::XMVECTOR vx = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vy, vz));//x
	vz = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vx, vy));//真のz

																	  // スケール設定 (XMVectorScaleはvecとfloatの掛け算)
	const DirectX::XMFLOAT3& s = this->getScale();
	vx = DirectX::XMVectorScale(vx, s.x);
	vy = DirectX::XMVectorScale(vy, s.y);
	vz = DirectX::XMVectorScale(vz, s.z);

	// 各軸を取得
	//変換して代入
	DirectX::XMFLOAT3 x, y, z;
	DirectX::XMStoreFloat3(&x, vx);
	DirectX::XMStoreFloat3(&y, vy);
	DirectX::XMStoreFloat3(&z, vz);

	// 位置取得
	const DirectX::XMFLOAT3& p = this->getPosition();

	// ワールド変換行列設定
	DirectX::XMFLOAT4X4 world = {
		x.x,x.y,x.z,0,
		y.x,y.y,y.z,0,
		z.x,z.y,z.z,0,
		p.x,p.y,p.z,1
	};
	this->setWorld(world);
}