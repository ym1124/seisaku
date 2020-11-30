
#include "framework.h"
#include "tessellation_object.h"
#include "light.h"

tessellation_object::tessellation_object(std::shared_ptr<tessellation_mesh>& smesh)
{
	mesh = smesh;
	setPosition(DirectX::XMFLOAT3(0, 0, 0.0f));
	setAngle(DirectX::XMFLOAT3(0, 0, 0));
	setScale(DirectX::XMFLOAT3(1, 1, 1));
}

tessellation_object::~tessellation_object()
{
	/*delete StaticObject::mesh;*/
	if (base_shading.get() != nullptr)
		base_shading.get()->End();
}

void tessellation_object::update()
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

void tessellation_object::render(ID3D11DeviceContext *immediate_context, camera* m_camera, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color, bool wire_frame, int tess_factor, int inside_tess_factor, float depth)
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

	mesh->render(immediate_context, world_view_projection, world_inv, light_direction, material_color, wire_frame, tess_factor, inside_tess_factor,depth);
}

void tessellation_object::render(ID3D11DeviceContext *immediate_context, camera* m_camera, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color, bool wire_frame, int tess_factor, int inside_tess_factor, float depth, ID3D11ShaderResourceView *srv)
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

	mesh->render(immediate_context, world_view_projection, world_inv, light_direction, material_color, wire_frame, tess_factor, inside_tess_factor, depth, srv);
}

void tessellation_object::move()
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