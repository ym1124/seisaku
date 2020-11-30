//#include <stdlib.h>
#include "camera.h"
//#include "input.h"
#include "framework.h"

void camera::active(bool PerspectiveFlg)
{
	DirectX::XMVECTOR eye;
	DirectX::XMVECTOR focus;
	DirectX::XMVECTOR up;
	eye = DirectX::XMLoadFloat3(&getEye());
	focus = DirectX::XMLoadFloat3(&getFocus());
	up = DirectX::XMLoadFloat3(&getUp());

	if (PerspectiveFlg)
		DirectX::XMStoreFloat4x4(&projection, DirectX::XMMatrixPerspectiveFovLH(fov, aspect, nearZ, farZ));
	else if (!PerspectiveFlg)
		DirectX::XMStoreFloat4x4(&projection, DirectX::XMMatrixOrthographicLH(width, height, nearZ, farZ));

	DirectX::XMStoreFloat4x4(&view, DirectX::XMMatrixLookAtLH(eye, focus, up));

	//CB更新
	camera_cb ccb;
	ccb.eye = DirectX::XMFLOAT4(getEye().x, getEye().y, getEye().z, 1.0f);//eye_pos
	ccb.dir = DirectX::XMFLOAT4(getFocus().x, getFocus().y, getFocus().z, 0.0f);//focus
	ccb.view = getView();//view
	DirectX::XMMATRIX view_m = DirectX::XMLoadFloat4x4(&getView());//view_m
	ccb.projection = getProjection();//proj
	DirectX::XMMATRIX proj_m = DirectX::XMLoadFloat4x4(&getProjection());//proj_m
	DirectX::XMStoreFloat4x4(&ccb.view_proj,view_m*proj_m);//vp
	view_projection = ccb.view_proj;
	XMStoreFloat4x4(&ccb.inv_view, DirectX::XMMatrixInverse(0, view_m));//inv_view
	XMStoreFloat4x4(&ccb.inv_projection, DirectX::XMMatrixInverse(0, proj_m));//inv_proj
	XMStoreFloat4x4(&ccb.inv_view_proj, DirectX::XMMatrixInverse(0, view_m*proj_m));//inv_view_proj

	FRAMEWORK.getImmediateContext()->UpdateSubresource(constant_buffer.Get(), 0, NULL, &ccb, 0, 0);
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(4, 1, constant_buffer.GetAddressOf());
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(4, 1, constant_buffer.GetAddressOf());
	FRAMEWORK.getImmediateContext()->GSSetConstantBuffers(4, 1, constant_buffer.GetAddressOf());
	FRAMEWORK.getImmediateContext()->CSSetConstantBuffers(4, 1, constant_buffer.GetAddressOf());

	//ライトのCb更新
	ConstantBufferForPhong cbl;
	cbl.ambient_color = Light::Ambient;
	cbl.dir_light.direction = Light::LightDir;
	cbl.dir_light.color = Light::DirLightColor;
	FRAMEWORK.getImmediateContext()->UpdateSubresource(constant_buffer_light.Get(), 0, NULL, &cbl, 0, 0);
	FRAMEWORK.getImmediateContext()->VSSetConstantBuffers(1, 1, constant_buffer_light.GetAddressOf());
	FRAMEWORK.getImmediateContext()->PSSetConstantBuffers(1, 1, constant_buffer_light.GetAddressOf());
	FRAMEWORK.getImmediateContext()->GSSetConstantBuffers(1, 1, constant_buffer_light.GetAddressOf());
	FRAMEWORK.getImmediateContext()->CSSetConstantBuffers(1, 1, constant_buffer_light.GetAddressOf());
}


void camera::setPerspective(float fov, float aspect, float nearZ, float farZ)
{
	this->fov = fov;
	this->aspect = aspect;
	this->nearZ = nearZ;
	this->farZ = farZ;
}

void camera::setOrthographic(float width, float height, float nearZ, float farZ)
{
	this->width = width;
	this->height = height;
	this->nearZ = nearZ;
	this->farZ = farZ;
}

main_camera::main_camera()
{
	target_obj = nullptr;
	target_obj_s = nullptr;
	angle_x = 0;
	angle_y = 0;
	setPerspective(DirectX::XM_PI / 8.0f, framework::SCREEN_WIDTH / framework::SCREEN_HEIGHT, 0.1f, 1000.0f);
	setEye(DirectX::XMFLOAT3(0.0f, 0.0f, -1.0f));
	setUp(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
	setVibration(.0f, 0.0f);

	// 定数バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof(camera_cb);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	HRESULT hr = FRAMEWORK.getDevice()->CreateBuffer(&bd, NULL, constant_buffer.GetAddressOf());
	assert(SUCCEEDED(hr));

	// ライト用定数バッファ生成
	ZeroMemory(&bd, sizeof(D3D11_BUFFER_DESC));
	bd.ByteWidth = sizeof(ConstantBufferForPhong);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;

	hr = FRAMEWORK.getDevice()->CreateBuffer(&bd, NULL, constant_buffer_light.GetAddressOf());
	assert(SUCCEEDED(hr));
}

main_camera::~main_camera()
{
	//if(target_obj)
	//	delete target_obj;
	//if(target_obj_s)
	//	delete target_obj_s;
}

void main_camera::update(float elapsedTime)
{
	switch (mode)
	{
	case Mode::MODE_NORMAL:
		normal();
		break;
	case Mode::MODE_SET:
		set(getFocus());
		break;
	case Mode::MODE_DIR_LIGHT:
		dir_light();
		break;
	case Mode::MODE_FOCUS:
		focus();
		break;
	case Mode::MODE_TPS:
		tps();
		break;
	default:
		break;
	}
	vibrate(elapsedTime);
}



void main_camera::setMode(Mode mode)
{
	this->mode = mode;
}

void main_camera::normal()
{
	setFocus(DirectX::XMFLOAT3(getEye().x, getEye().y, getEye().z + 1.0f));
}

void main_camera::set(DirectX::XMFLOAT3 setPos)
{
	setFocus(setPos);
}

void main_camera::focus()
{
	//setFocus(moveTarget);
}

void main_camera::dir_light()
{
	//if (getTarget() != nullptr)
	//{
	//	////setDirLight必須
	//	//SetEye(DirectX::XMFLOAT3(getTarget()->getPosition().x + directional_light.x*-DIR_LIGHT_RANGE, getTarget()->getPosition().y + directional_light.y*-DIR_LIGHT_RANGE + 4.0f, getTarget()->getPosition().z + directional_light.z*-DIR_LIGHT_RANGE));
	//	//SetFocus(DirectX::XMFLOAT3(getTarget()->getPosition().x, getTarget()->getPosition().y + 5.0f, getTarget()->getPosition().z));
	//	setEye(DirectX::XMFLOAT3(getTarget()->getPosition().x + directional_light.x*-DIR_LIGHT_RANGE, getTarget()->getPosition().y + directional_light.y*-DIR_LIGHT_RANGE + 30.0f/*モデルを上から見れる高さ*/, getTarget()->getPosition().z + directional_light.z*-DIR_LIGHT_RANGE));
	//	setFocus(DirectX::XMFLOAT3(getTarget()->getPosition().x, getTarget()->getPosition().y, getTarget()->getPosition().z));
	//}
	//else if (getTarget_s() != nullptr)
	//{
	//	setEye(DirectX::XMFLOAT3(getTarget_s()->getPosition().x + directional_light.x*-DIR_LIGHT_RANGE, getTarget_s()->getPosition().y + directional_light.y*-DIR_LIGHT_RANGE + 20.0f, getTarget_s()->getPosition().z + directional_light.z*-DIR_LIGHT_RANGE));
	//	setFocus(DirectX::XMFLOAT3(getTarget_s()->getPosition().x, getTarget_s()->getPosition().y + 5.0f, getTarget_s()->getPosition().z));
	//}

	//2020/8/26//objのbase_shadingが邪魔する
	setEye(DirectX::XMFLOAT3(getFocus().x + directional_light.x*-DIR_LIGHT_RANGE, getFocus().y + directional_light.y*-DIR_LIGHT_RANGE + 30.0f/*モデルを上から見れる高さ*/, getFocus().z + directional_light.z*-DIR_LIGHT_RANGE));
}

void main_camera::tps()
{
	//setTarget必須
	setEye(DirectX::XMFLOAT3(getTarget()->getPosition().x + getTarget()->getForward().x * TPS_RANGE, getTarget()->getPosition().y + getTarget()->getForward().y * TPS_RANGE +11.0f, getTarget()->getPosition().z + getTarget()->getForward().z *  TPS_RANGE));
	setFocus(DirectX::XMFLOAT3(getTarget()->getPosition().x, getTarget()->getPosition().y + 3.0f, getTarget()->getPosition().z));
}

void main_camera::setVibration(float range, float time)
{
	this->range = range;
	this->time = time;
}

//void MainCamera::SetTarget(DirectX::XMFLOAT3 target)
//{
//	//moveTarget = target;
//}

void main_camera::vibrate(float elapsedTime)
{
	if (time > 0)
	{
		time -= elapsedTime;

		DirectX::XMFLOAT3 focus = getFocus();

		DirectX::XMFLOAT3 rng = getRandom();
		setFocus(DirectX::XMFLOAT3(focus.x + rng.x, focus.y + rng.y, focus.z + rng.z));
	}
}

DirectX::XMFLOAT3 main_camera::getRandom()
{
	DirectX::XMFLOAT3 rng;
	rng.x = rand() % (int)(range * 2 + 1) - range;
	rng.x *= (time / 10);
	rng.y = rand() % (int)(range * 2 + 1) - range;
	rng.y *= (time / 10);
	rng.z = rand() % (int)(range * 2 + 1) - range;
	rng.z *= (time / 10);

	return rng;
}

void main_camera::move()
{
	//static DirectX::XMFLOAT3 position(0, 0, 10);
	//if (GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState('C') < 0 /*&& GetAsyncKeyState('Z') < 0*/)
	//{
		if (GetAsyncKeyState(VK_LEFT) < 0)
			setEye(DirectX::XMFLOAT3(getEye().x - 1.0f, getEye().y, getEye().z));
		if (GetAsyncKeyState(VK_RIGHT) < 0)
			setEye(DirectX::XMFLOAT3(getEye().x + 1.0f, getEye().y, getEye().z));
		if (GetAsyncKeyState('P') < 0)
			setEye(DirectX::XMFLOAT3(getEye().x, getEye().y+1, getEye().z));
		if (GetAsyncKeyState('L') < 0)
			setEye(DirectX::XMFLOAT3(getEye().x, getEye().y-1, getEye().z));
		if (GetAsyncKeyState(VK_UP) < 0)
			setEye(DirectX::XMFLOAT3(getEye().x, getEye().y, getEye().z + 1));
		if (GetAsyncKeyState(VK_DOWN) < 0)
			setEye(DirectX::XMFLOAT3(getEye().x, getEye().y, getEye().z - 1));
	/*}*/

	//if (GetAsyncKeyState(VK_CONTROL) < 0 && GetAsyncKeyState('C') < 0 && GetAsyncKeyState('X') < 0)
	//{
	//	static int x, y, z;
	//	if (GetAsyncKeyState(VK_LEFT) < 0)
	//		x++;
	//	if (GetAsyncKeyState(VK_RIGHT) < 0)
	//		x--;
	//	if (x > 90)
	//		x = 90;
	//	else if (x < -90)
	//		x = -90;
	//	if (GetAsyncKeyState(VK_UP) < 0)
	//		y++;
	//	if (GetAsyncKeyState(VK_DOWN) < 0)
	//		y--;
	//	if (y > 90)
	//		y = 90;
	//	else if (y < -90)
	//		y = -90;
	//	if (GetAsyncKeyState('W') < 0)
	//		z++;
	//	if (GetAsyncKeyState('S') < 0)
	//		z--;

	//	DirectX::XMFLOAT3 f = DirectX::XMFLOAT3(x*DirectX::XM_PI/180.0f, y*DirectX::XM_PI / 180.0f, z*DirectX::XM_PI / 180.0f);
	//	f.x = sinf(f.x);
	//	f.y = sinf(f.y);
	//	SetFocus(DirectX::XMFLOAT3(GetEye().x+f.x, GetEye().y + f.y, GetEye().z + f.z));
	//}
}

void main_camera::focusMove(float times)
{
	if (GetAsyncKeyState(VK_LEFT) < 0)
		angle_x += 0.01f*times;
	if (GetAsyncKeyState(VK_RIGHT) < 0)
		angle_x -= 0.01f*times;
	if (GetAsyncKeyState(VK_UP) < 0 && angle_y < DirectX::XM_PI/2)
		angle_y += 0.01f*times;
	if (GetAsyncKeyState(VK_DOWN) < 0 && angle_y > -DirectX::XM_PI / 2)
		angle_y -= 0.01f*times;
	if (GetAsyncKeyState(VK_RSHIFT) < 0)
		length+= 0.2f;
	if (GetAsyncKeyState(VK_RCONTROL) < 0)
		length -= 0.2f;

	setEye(DirectX::XMFLOAT3(getFocus().x + sinf(angle_x)*length, getFocus().y + sinf(angle_y)*length, getFocus().z + cosf(angle_x)*length));
}

void main_camera::setFocusMoveEye(const DirectX::XMFLOAT2 eye, const float length)
{
	angle_x = eye.x;
	angle_y = eye.y;
	this->length = length;
}

void main_camera::setReversePosY(DirectX::XMFLOAT3 target_pos, main_camera *mainCamera)
{
	float eye_y = target_pos.y - mainCamera->getEye().y;
	setEye(DirectX::XMFLOAT3(mainCamera->getEye().x, eye_y, mainCamera->getEye().z));
	//SetUp(DirectX::XMFLOAT3(mainCamera->GetUp().x, -mainCamera->GetUp().y, mainCamera->GetUp().z));
}

void main_camera::setReversePos(static_object* target, main_camera *mainCamera)
{
	//モデルの右と前方ベクトルの外積
	DirectX::XMVECTOR r, f;
	r = DirectX::XMLoadFloat3(&target->getRight());
	f = DirectX::XMLoadFloat3(&target->getUp());
	DirectX::XMVECTOR Cross = DirectX::XMVector3Cross(r, f);
	Cross = DirectX::XMVector3Normalize(Cross);//正規化
	DirectX::XMFLOAT3 CrossF;
	DirectX::XMStoreFloat3(&CrossF, Cross);//FLOAT3化

	//面からカメラまでの距離を算出
	float d = abs(CrossF.x*mainCamera->getEye().x + CrossF.y*mainCamera->getEye().y + CrossF.z*mainCamera->getEye().z)/sqrtf(CrossF.x*CrossF.x+ CrossF.y*CrossF.y+ CrossF.z*CrossF.z);

	//カメラから法線と逆向きにd進んだ位置
	DirectX::XMFLOAT3 Pos = DirectX::XMFLOAT3(mainCamera->getEye().x+CrossF.x*d, mainCamera->getEye().y+CrossF.y*d, mainCamera->getEye().z+CrossF.z*d);

	//位置を設定
	setEye(Pos);
	//SetUp(DirectX::XMFLOAT3(mainCamera->GetUp().x, -mainCamera->GetUp().y, mainCamera->GetUp().z));
}