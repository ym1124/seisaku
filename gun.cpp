#include "gun.h"
#include "bullet.h"
#include "mouse_input.h"

#include "num_manager.h"

void gun::reLoad()
{
	//if (!isReLoad) return;
	if (pNumManager->plBullet.isZero()) return;

	if (m_bulletNum > 0) return;

	int setBulletNum = m_bulletNumMax - m_bulletNum;	//リロードする弾数

	if (pNumManager->plBullet.getNum() >= setBulletNum) {//弾数を上限まで補充できるとき
		pNumManager->plBullet.subNum(setBulletNum);

		m_bulletNum = m_bulletNumMax;
	}
	else {
		for (int i = pNumManager->plBullet.getNum(); i > 0; i--) {
			pNumManager->plBullet.subNum();

			m_bulletNum++;
		}
	}

	isReLoad = false;
}

//ハンドガン
void hand_gun::init(bullet_manager* bulletManager, const DirectX::XMFLOAT3 pos, const DirectX::XMFLOAT4 orient)
{
	m_bulletManager = bulletManager;

	//メッシュのロード
	m_mesh.initialize();
	m_mesh.load("FBX/Weapon/Asalt.bin");

	m_mesh.setAllMaterialTex(L"FBX/Texture/Weapon/Asalt/Asalt_B.png");

	m_mesh.scale = DirectX::XMFLOAT3(-0.1f, 0.1f, 0.1f);

	m_pos = pos;
	m_orient = orient;

	m_bulletNumMax = 10;

	m_bulletNum = 10;

	m_shotInterval = 10;

	m_shotTimer = 0;

	m_attackPower = 5.0f;

	m_isActivate = false;
}

void hand_gun::update()
{
	reLoad();

	//モデルの姿勢更新
	DirectX::XMVECTOR orient = DirectX::XMLoadFloat4(&m_mesh.orientation);

	DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(orient);
	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, mat);

	m_mesh.right = DirectX::XMFLOAT3(m._11, m._12, m._13);
	m_mesh.up = DirectX::XMFLOAT3(m._21, m._22, m._23);
	m_mesh.forward = DirectX::XMFLOAT3(m._31, m._32, m._33);

	//弾発射
	if (m_shotTimer != 0)m_shotTimer -= 1;

	if (pad.RTrigger(GamePad::TriggerType::PUSH, GamePad::_1P)) {

		if (m_shotTimer == 0 && m_bulletNum > 0) {
			DirectX::XMFLOAT4 bulletOrientation = m_mesh.orientation;

			DirectX::XMFLOAT3 bulletPos = m_pos;
			bulletPos = Vec3Add(bulletPos, Vec3Multiply(m_mesh.forward, 5.0f));

			DirectX::XMFLOAT3 bulletAccel = Vec3Multiply(m_mesh.forward, 0.1f);
			DirectX::XMFLOAT3 bulletSpeed = m_mesh.forward;

			float bulletAttackPower = m_attackPower;

			m_bulletManager->bulletSet(bulletOrientation, bulletPos, bulletAccel, bulletSpeed, bulletAttackPower);

			m_bulletNum -= 1;

			m_shotTimer = m_shotInterval;
		}
	}
}

void hand_gun::render(std::unique_ptr<shader_ex>& shader, MainCamera *m_camera, DirectX::XMFLOAT4& light_dir)
{
	//描画用変数の更新
	m_mesh.orientation = m_orient;
	m_mesh.pos = m_pos;

	//描画
	m_mesh.render(shader, m_camera, light_dir, true);
}

void hand_gun::release()
{
}


//ショットガン
void shot_gun::init(bullet_manager* bulletManager, const DirectX::XMFLOAT3 pos, const DirectX::XMFLOAT4 orient)
{
	m_bulletManager = bulletManager;

	//メッシュのロード
	m_mesh.initialize();
	m_mesh.load("FBX/Weapon/ShotGun.bin");

	m_mesh.setAllMaterialTex(L"FBX/Texture/Weapon/ShotGun/ShotGun_B.png");

	m_mesh.scale = DirectX::XMFLOAT3(-0.1f, 0.1f, 0.1f);

	m_pos = pos;
	m_orient = orient;

	m_bulletNumMax = 5;

	m_bulletNum = 5;

	m_shotInterval = 30;

	m_shotTimer = 0;

	m_attackPower = 20.0f;

	m_isActivate = false;
}

void shot_gun::update()
{
	reLoad();

	//モデルの姿勢更新
	DirectX::XMVECTOR orient = DirectX::XMLoadFloat4(&m_mesh.orientation);

	DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(orient);
	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, mat);

	m_mesh.right = DirectX::XMFLOAT3(m._11, m._12, m._13);
	m_mesh.up = DirectX::XMFLOAT3(m._21, m._22, m._23);
	m_mesh.forward = DirectX::XMFLOAT3(m._31, m._32, m._33);

	//弾発射
	if (m_shotTimer != 0)m_shotTimer -= 1;

	if (pad.RTrigger(GamePad::TriggerType::PUSH, GamePad::_1P)) {

		if (m_shotTimer == 0 && m_bulletNum > 0) {
			DirectX::XMFLOAT4 bulletOrientation = m_mesh.orientation;

			DirectX::XMFLOAT3 bulletPos = m_pos;
			bulletPos = Vec3Add(bulletPos, Vec3Multiply(m_mesh.forward, 5.0f));

			DirectX::XMFLOAT3 bulletAccel = Vec3Multiply(m_mesh.forward, 0.2f);
			DirectX::XMFLOAT3 bulletSpeed = m_mesh.forward;

			float bulletAttackPower = m_attackPower;

			m_bulletManager->bulletSet(bulletOrientation, bulletPos, bulletAccel, bulletSpeed, bulletAttackPower);

			m_bulletNum -= 1;

			m_shotTimer = m_shotInterval;
		}
	}
}

void shot_gun::render(std::unique_ptr<shader_ex>& shader, MainCamera *m_camera, DirectX::XMFLOAT4& light_dir)
{
	//描画用変数の更新
	m_mesh.orientation = m_orient;
	m_mesh.pos = m_pos;

	//描画
	m_mesh.render(shader, m_camera, light_dir, true);
}

void shot_gun::release()
{
}


//マシンガン
void machine_gun::init(bullet_manager* bulletManager, const DirectX::XMFLOAT3 pos, const DirectX::XMFLOAT4 orient)
{
	m_bulletManager = bulletManager;

	//メッシュのロード
	m_mesh.initialize();

	//メッシュのロード
	m_mesh.initialize();
	m_mesh.load("FBX/Weapon/Launcher.bin");

	m_mesh.setAllMaterialTex(L"FBX/Texture/Weapon/Launcher/Launcher_B.png");

	m_mesh.scale = DirectX::XMFLOAT3(-0.1f, 0.1f, 0.1f);

	m_pos = pos;
	m_orient = orient;

	m_bulletNumMax = 3;
	m_bulletNum = 3;

	m_shotInterval = 30;

	m_shotTimer = 0;

	m_attackPower = 2.0f;

	m_isActivate = false;
}

void machine_gun::update()
{
	reLoad();

	//モデルの姿勢更新
	DirectX::XMVECTOR orient = DirectX::XMLoadFloat4(&m_mesh.orientation);

	DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(orient);
	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, mat);

	m_mesh.right = DirectX::XMFLOAT3(m._11, m._12, m._13);
	m_mesh.up = DirectX::XMFLOAT3(m._21, m._22, m._23);
	m_mesh.forward = DirectX::XMFLOAT3(m._31, m._32, m._33);

	//弾発射
	if (m_shotTimer != 0)m_shotTimer -= 1;

	if (pad.RTrigger(GamePad::TriggerType::PUSH, GamePad::_1P) || kb.Push('B')) {

		if (m_shotTimer == 0 && m_bulletNum > 0) {
			DirectX::XMFLOAT4 bulletOrientation = m_mesh.orientation;

			DirectX::XMFLOAT3 bulletPos = m_pos;
			bulletPos = Vec3Add(bulletPos, Vec3Multiply(m_mesh.forward, 5.0f));

			DirectX::XMFLOAT3 bulletAccel = Vec3Multiply({ 0.0f,-0.5f,0.0f }, 0.1f);
			DirectX::XMFLOAT3 bulletSpeed = Vec3Multiply(m_mesh.forward, 2.0f);

			float bulletAttackPower = m_attackPower;

			m_bulletManager->bulletSet(bulletOrientation, bulletPos, bulletAccel, bulletSpeed, bulletAttackPower, bulletType::Bomb);
			m_bulletNum -= 1;

			m_shotTimer = m_shotInterval;
		}
	}
}

//#include "..\..\Lib\Header\3dDebugRender.h"

void machine_gun::render(std::unique_ptr<shader_ex>& shader, MainCamera *m_camera, DirectX::XMFLOAT4& light_dir)
{
	//描画用変数の更新
	m_mesh.orientation = m_orient;
	m_mesh.pos = m_pos;

	//描画
	m_mesh.render(shader,m_camera, light_dir, true);

	DirectX::XMVECTOR orient = DirectX::XMLoadFloat4(&m_mesh.orientation);

	DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(orient);
	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, mat);

	m_mesh.right = DirectX::XMFLOAT3(m._11, m._12, m._13);
	m_mesh.up = DirectX::XMFLOAT3(m._21, m._22, m._23);
	m_mesh.forward = DirectX::XMFLOAT3(m._31, m._32, m._33);

	//DirectX::XMFLOAT3 startPos = m_mesh.pos;
	//DirectX::XMFLOAT3 endPos = Vec3Add(startPos, Vec3Multiply(m_mesh.forward, 10.0f));

	//debug3d->LineRender(startPos, endPos, view, projection, { 0.0f,0.0f,1.0f,1.0f });

	//endPos = Vec3Add(startPos, Vec3Multiply(m_mesh.up, 10.0f));

	//debug3d->LineRender(startPos, endPos, view, projection, { 0.0f,1.0f,0.0f,1.0f });

	//endPos = Vec3Add(startPos, Vec3Multiply(m_mesh.right, 10.0f));

	//debug3d->LineRender(startPos, endPos, view, projection, { 1.0f,0.0f,0.0f,1.0f });
}

void machine_gun::release()
{
}