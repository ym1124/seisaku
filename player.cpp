#include "player.h"
#include "mouse_input.h"
#include "gun.h"
#include "bullet.h"

#include "num_manager.h"

DirectX::XMFLOAT3 scale = { 1.0f, 1.0f, 1.0f };
DirectX::XMFLOAT4 orient = { 0.0f, 0.0f, 0.0f ,1.0f };

static int handCount = 0;
static int shotCount = 0;
static int machineCount = 0;

void player::init()
{
	//弾数の初期値設定
	pNumManager->plBullet.initNum(30);

	handCount = 0;
	shotCount = 0;
	machineCount = 0;

	m_weapon.clear();

	m_weaponID = -1;

	m_damage = 0.0f;

	//メッシュのロード
	m_mesh.initialize();
	m_mesh.load("FBX/Player/Player_Idle.bin");
	m_mesh.setAllMaterialTex(L"FBX/Texture/Player/Player_B.png");

	m_mesh.scale = DirectX::XMFLOAT3(-0.1f, 0.1f, 0.1f);

	//アニメーションの初期化
	//m_idolAnim = m_mesh.AnimationInit(true);
	//m_animID = m_prevAnimID = m_idolAnim;

	//アニメーションの追加
	anim.add(m_mesh._model.get(), "FBX/Player/Player_Run.bin", "Run");
	anim.add(m_mesh._model.get(), "FBX/Player/Player_Shot.bin", "IdleShot");
	anim.add(m_mesh._model.get(), "FBX/Enemy/Omake2.bin", "Omake");

	//m_runAnim = m_mesh.AddAnimation("FBX/Player/Player_Run.bin");
	//m_omakeAnim = m_mesh.AddAnimation("FBX/Enemy/Omake2.bin");

	//位置の初期化
	m_pos = { 0.0f,0.0f,0.0f };

	cameraAngle = 0.0f;


	//networkData.Get_GameObject3D().Regist("Player", &m_pos, &m_mesh.scale, &m_mesh.orientation, DirectX::XMFLOAT3(-5.0f, 0.0f, -5.0f), DirectX::XMFLOAT3(5.0f, 13.0f, 5.0f));
}

void player::update()
{
	////1フレーム前のアニメーション保存
	//m_prevAnimID = m_animID;

	//重力
	//m_pos.y -= 2.0f;

	//走ってるときの武器の位置補正
	static float adjust;
	adjust = 0.0f;

	//位置の初期化
	if (pad.PushTrigger(XINPUT_GAMEPAD_BACK, GamePad::PlayerID::_1P)) {
		m_pos = { 0.0f,0.0f,0.0f };
	}

	//ダメージの回復
	if (pad.PushTrigger(XINPUT_GAMEPAD_B, GamePad::PlayerID::_1P)) {

		float recoveryDamage = FloatRandom(5.0f, 10.0f);	//回復量

		if (!pNumManager->karage.isZero() && m_damage > 0.0f) {
			pNumManager->karage.subNum();

			m_damage -= recoveryDamage;

			if (m_damage < 0.0f) m_damage = 0.0f;
		}
	}

	//唐揚げを弾へ変換！！
	if (pad.PushTrigger(XINPUT_GAMEPAD_X, GamePad::PlayerID::_1P)) {

		if (!pNumManager->karage.isZero()) {
			pNumManager->karage.subNum();

			pNumManager->plBullet.addNum(30);
		}
	}

	//モデルの姿勢更新
	DirectX::XMVECTOR orient = DirectX::XMLoadFloat4(&m_mesh.orientation);

	DirectX::XMMATRIX mat = DirectX::XMMatrixRotationQuaternion(orient);
	DirectX::XMFLOAT4X4 m;
	DirectX::XMStoreFloat4x4(&m, mat);

	m_mesh.right = DirectX::XMFLOAT3(m._11, m._12, m._13);
	m_mesh.up = DirectX::XMFLOAT3(m._21, m._22, m._23);
	m_mesh.forward = DirectX::XMFLOAT3(m._31, m._32, m._33);


	DirectX::XMVECTOR right = DirectX::XMLoadFloat3(&Vec3Normalize(m_mesh.right));
	DirectX::XMVECTOR up = DirectX::XMLoadFloat3(&Vec3Normalize(m_mesh.up));
	DirectX::XMVECTOR forward = DirectX::XMLoadFloat3(&Vec3Normalize(m_mesh.forward));

	static float dangle = 0.0f;	//ゲームパッドのLスティックの倒れてる角度(ラジアン)

	if (pad.CheckLStickRange(GamePad::PlayerID::_1P))
	{
		dangle = pad.GetLStickRadianAngle(GamePad::PlayerID::_1P);

		//カメラ方向を正面にする
		dangle -= cameraAngle;

		DirectX::XMFLOAT3 vec;

		vec = { sinf(dangle),0.0f,cosf(dangle) };
		//speed代入
		{
			//モーションブレンド中じゃなかったら
			if (!anim.getBlendMode()) {
				//仮の移動速度
				const float speed = 1.0f;
				m_pos.x += vec.x * speed;
				m_pos.z += vec.z * speed;
			}
		}

		vec = Vec3Normalize(vec);
		float _angle = acosf(Vec3Dot(Vec3Normalize(m_mesh.forward), vec));

		if (fabs(_angle) > 1e-8f)
		{
			DirectX::XMFLOAT3 axis;
			axis = Vec3Cross(Vec3Normalize(m_mesh.forward), vec);
			axis = Vec3Normalize(axis);

			DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axis), _angle);
			//orient = DirectX::XMQuaternionMultiply(orient, q);
			orient = DirectX::XMQuaternionSlerp(orient, DirectX::XMQuaternionMultiply(orient, q), 0.2f);

			DirectX::XMStoreFloat4(&m_mesh.orientation, orient);
		}

		if (!anim.set("Run")) m_pos = { 0.0f,0.0f,0.0f };
	}
	else {
		static int keyCheck = 0;

		if (kb.PushTrigger(VK_UP)) {
			keyCheck += 1;
		}
		if (kb.PushTrigger(VK_RIGHT)) {
			keyCheck += 10;
		}
		if (kb.PushTrigger(VK_DOWN)) {
			keyCheck += 100;
		}
		if (kb.PushTrigger(VK_LEFT)) {
			keyCheck += 1000;
		}

		if (kb.ReleaseTrigger(VK_UP)) {
			keyCheck -= 1;
		}
		if (kb.ReleaseTrigger(VK_RIGHT)) {
			keyCheck -= 10;
		}
		if (kb.ReleaseTrigger(VK_DOWN)) {
			keyCheck -= 100;
		}
		if (kb.ReleaseTrigger(VK_LEFT)) {
			keyCheck -= 1000;
		}

		if (keyCheck == 1 || keyCheck == 1011) {
			dangle = DirectX::XMConvertToRadians(0.0f);
		}
		if (keyCheck == 11) {
			dangle = DirectX::XMConvertToRadians(45.0f);
		}
		if (keyCheck == 10 || keyCheck == 111) {
			dangle = DirectX::XMConvertToRadians(90.0f);
		}
		if (keyCheck == 110) {
			dangle = DirectX::XMConvertToRadians(135.0f);
		}
		if (keyCheck == 100 || keyCheck == 1110) {
			dangle = DirectX::XMConvertToRadians(180.0f);
		}
		if (keyCheck == 1100) {
			dangle = DirectX::XMConvertToRadians(225.0f);
		}
		if (keyCheck == 1000 || keyCheck == 1101) {
			dangle = DirectX::XMConvertToRadians(270.0f);
		}
		if (keyCheck == 1001) {
			dangle = DirectX::XMConvertToRadians(315.0f);
		}

		if (kb.Push(VK_LEFT) || kb.Push(VK_RIGHT) || kb.Push(VK_UP) || kb.Push(VK_DOWN)) {
			//カメラ方向を正面にする
			dangle -= cameraAngle;

			DirectX::XMFLOAT3 vec;

			vec = { sinf(dangle),0.0f,cosf(dangle) };
			//speed代入
			{
				//モーションブレンド中じゃなかったら
				if (!anim.getBlendMode()) {
					//仮の移動速度
					const float speed = 1.0f;
					m_pos.x += vec.x * speed;
					m_pos.z += vec.z * speed;
				}
			}

			vec = Vec3Normalize(vec);

			float _angle = acosf(Vec3Dot(Vec3Normalize(m_mesh.forward), vec));

			if (fabs(_angle) > 1e-8f)
			{
				DirectX::XMFLOAT3 axis;
				axis = Vec3Cross(Vec3Normalize(m_mesh.forward), vec);
				axis = Vec3Normalize(axis);

				DirectX::XMVECTOR q = DirectX::XMQuaternionRotationAxis(DirectX::XMLoadFloat3(&axis), _angle);
				//orient = DirectX::XMQuaternionMultiply(orient, q);
				orient = DirectX::XMQuaternionSlerp(orient, DirectX::XMQuaternionMultiply(orient, q), 0.2f);
				DirectX::XMStoreFloat4(&m_mesh.orientation, orient);
			}

			if (!anim.set("Run")) m_pos = { 0.0f,0.0f,0.0f };
		}
		else {
			//Xボタン押してる間だけおまけアニメーション
			if (/*pad.Push(XINPUT_GAMEPAD_X, GamePad::PlayerID::_1P)*/GetAsyncKeyState('Q')<0) {
				if (!anim.set("Omake")) m_pos = { 0.0f,0.0f,0.0f };
			}
			else if (/*pad.RTrigger(GamePad::TriggerType::PUSH, GamePad::_1P)*/GetAsyncKeyState('W')<0) {
				if (!anim.set("IdleShot")) m_pos = { 0.0f,0.0f,0.0f };
			}
			else {
				if (!anim.set("Idle")) m_pos = { 0.0f,0.0f,0.0f };
			}
		}
	}

	//武器をもってるかどうか
	if ((int)m_weapon.size() > 0) {
		//武器チェンジ
		if (pad.PushTrigger(XINPUT_GAMEPAD_RIGHT_SHOULDER, GamePad::PlayerID::_1P) || kb.PushTrigger('1')) {
			m_weapon[m_weaponID]->unActiveWeapon();

			m_weaponID++;
			if (m_weaponID > (int)m_weapon.size() - 1) {
				m_weaponID = 0;
			}

			m_weapon[m_weaponID]->activeWeapon();
		}

		if (pad.PushTrigger(XINPUT_GAMEPAD_LEFT_SHOULDER, GamePad::PlayerID::_1P) || kb.PushTrigger('2')) {
			m_weapon[m_weaponID]->unActiveWeapon();

			m_weaponID--;
			if (m_weaponID < 0) {
				m_weaponID = (int)m_weapon.size() - 1;
			}

			m_weapon[m_weaponID]->activeWeapon();
		}

		nowWeaponUpdate();

		////リロード
		//if (pad.PushTrigger(XINPUT_GAMEPAD_X, GamePad::PlayerID::_1P)) {

		//	m_weapon[m_weaponID]->ReLoadOn();
		//}
	}

	if (pad.PushTrigger(XINPUT_GAMEPAD_RIGHT_THUMB, GamePad::PlayerID::_1P)) {
		anim.stop();
	}

	if (pad.PushTrigger(XINPUT_GAMEPAD_LEFT_THUMB, GamePad::PlayerID::_1P)) {
		anim.play();
	}

	//アニメーションの更新
	if (anim.isCurrentAnimation("IdolShot")) {
		anim.update(m_mesh._model.get(), false);
	}
	else {
		anim.update(m_mesh._model.get(), true);
	}
}

void player::render(std::unique_ptr<shader_ex>& shader, MainCamera* m_camera , DirectX::XMFLOAT4& light_dir)
{
	float texAnimTime = 0.0f;		//テクスチャのアニメーションする速さ
	float AnimTime = 1.0f / 60.0f;	//モデルのアニメーションの１フレームの速さ

	//if (m_animID == m_runAnim) {
	//	AnimTime = 1.0f / 30.0f;
	//}

	//描画用posの更新
	m_mesh.pos = m_pos;

	//TODO 色変えてますあとで消す
	//static int colorTimer = 0;
	//colorTimer++;
	//if (colorTimer > 60) {
	//	colorTimer -= 60;
	//	m_mesh.color = { FloatRandom(0.0f,1.0f),FloatRandom(0.0f,1.0f), FloatRandom(0.0f,1.0f), 1.0f };
	//}

	//描画
	m_mesh.render(shader, m_camera, light_dir, true, texAnimTime, AnimTime);
}

void player::render(std::unique_ptr<shader>& shader, MainCamera* m_camera, DirectX::XMFLOAT4& light_dir)
{
	float texAnimTime = 0.0f;		//テクスチャのアニメーションする速さ
	float AnimTime = 1.0f / 60.0f;	//モデルのアニメーションの１フレームの速さ

									//if (m_animID == m_runAnim) {
									//	AnimTime = 1.0f / 30.0f;
									//}

									//描画用posの更新
	m_mesh.pos = m_pos;

	//TODO 色変えてますあとで消す
	//static int colorTimer = 0;
	//colorTimer++;
	//if (colorTimer > 60) {
	//	colorTimer -= 60;
	//	m_mesh.color = { FloatRandom(0.0f,1.0f),FloatRandom(0.0f,1.0f), FloatRandom(0.0f,1.0f), 1.0f };
	//}

	//描画
	m_mesh.render(shader, m_camera, light_dir,  true, texAnimTime, AnimTime);
}

void player::release()
{
}

void player::newWeapon(weapon_manager* weaponManager, bullet_manager* bulletManager)
{
	hand_gun* handGun = new hand_gun(bulletManager, m_mesh.getBonePos("SKT_Weapon"), m_mesh.orientation);
	shot_gun* shotGun = new shot_gun(bulletManager, m_mesh.getBonePos("SKT_Weapon"), m_mesh.orientation);
	machine_gun* machineGun = new machine_gun(bulletManager, m_mesh.getBonePos("SKT_Weapon"), m_mesh.orientation);

	weaponManager->weapon.push_back(machineGun);
	m_weapon.push_back(machineGun);

	weaponManager->weapon.push_back(handGun);
	m_weapon.push_back(handGun);

	weaponManager->weapon.push_back(shotGun);
	m_weapon.push_back(shotGun);


	//int rand = IntRandom(0, 2);

	//switch (rand) {
	//case 0:
	//	handCount++;

	//	weaponManager->weapon.push_back(new HandGun(bulletManager, m_mesh.GetBonePos("SKT_Weapon"), m_mesh.orientation));

	//	if (handCount == 1)
	//		m_weapon.push_back(weaponManager->weapon[(int)weaponManager->weapon.size() - 1]);

	//	weaponManager->weapon[(int)weaponManager->weapon.size() - 1]->ActiveWeapon();
	//	break;

	//case 1:
	//	shotCount++;

	//	weaponManager->weapon.push_back(new ShotGun(bulletManager, m_mesh.GetBonePos("SKT_Weapon"), m_mesh.orientation));

	//	if (shotCount == 1)
	//		m_weapon.push_back(weaponManager->weapon[(int)weaponManager->weapon.size() - 1]);

	//	weaponManager->weapon[(int)weaponManager->weapon.size() - 1]->ActiveWeapon();
	//	break;

	//case 2:
	//	machineCount++;

	//	weaponManager->weapon.push_back(new MachineGun(bulletManager, m_mesh.GetBonePos("SKT_Weapon"), m_mesh.orientation));

	//	if (machineCount == 1)
	//		m_weapon.push_back(weaponManager->weapon[(int)weaponManager->weapon.size() - 1]);

	//	weaponManager->weapon[(int)weaponManager->weapon.size() - 1]->ActiveWeapon();
	//	break;
	//}

	if (m_weaponID == -1) {
		m_weaponID = 0;
		m_weapon[m_weaponID]->activeWeapon();
	}
}

void player::nowWeaponUpdate()
{
	//ボーン計算のための更新
	{
		m_mesh.pos = m_pos;

		m_mesh._model->calculateLocalTransform();
		m_mesh._model->calculateWorldTransform(m_mesh.getWorldMatrix());
	}

	DirectX::XMFLOAT4 boneQuaternion;
	DirectX::XMFLOAT3 weaponRight;
	DirectX::XMFLOAT3 weaponUp;
	DirectX::XMFLOAT3 weaponForward;

	//ボーンクォータニオンの生成
	{
		DirectX::XMFLOAT4X4 boneM =
		{
			m_mesh.getBoneDirectionY("SKT_Weapon").x,m_mesh.getBoneDirectionY("SKT_Weapon").y,m_mesh.getBoneDirectionY("SKT_Weapon").z,0.0f,
			m_mesh.getBoneDirectionZ("SKT_Weapon").x,fabsf(m_mesh.getBoneDirectionZ("SKT_Weapon").y),m_mesh.getBoneDirectionZ("SKT_Weapon").z,0.0f,
			-m_mesh.getBoneDirectionX("SKT_Weapon").x,-m_mesh.getBoneDirectionX("SKT_Weapon").y,-m_mesh.getBoneDirectionX("SKT_Weapon").z,0.0f,
			0.0f,0.0f,0.0f,1.0f
		};

		//回転行列ー＞クォータニオンへの変換
		//{
		//	float elem[4] = { 0.0f };

		//	elem[0] = boneM._11 - boneM._22 - boneM._33 + 1.0f;
		//	elem[1] = -boneM._11 + boneM._22 - boneM._33 + 1.0f;
		//	elem[2] = -boneM._11 - boneM._22 + boneM._33 + 1.0f;
		//	elem[3] = boneM._11 + boneM._22 + boneM._33 + 1.0f;

		//	int biggestIndex = 0;
		//	for (int i = 0; i < 4; i++) {
		//		if (elem[i] > elem[biggestIndex]) {
		//			biggestIndex = i;
		//		}
		//	}

		//	float v = sqrtf(elem[biggestIndex]) * 0.5f;

		//	float mult = 0.25f / v;

		//	switch (biggestIndex) {
		//	case 0:
		//		boneQuaternion.x = v;
		//		boneQuaternion.y = (boneM._12 + boneM._21) * mult;
		//		boneQuaternion.z = (boneM._31 + boneM._13) * mult;
		//		boneQuaternion.w = (boneM._23 - boneM._32) * mult;
		//		break;

		//	case 1:
		//		boneQuaternion.x = (boneM._12 + boneM._21) * mult;
		//		boneQuaternion.y = v;
		//		boneQuaternion.z = (boneM._23 + boneM._32) * mult;
		//		boneQuaternion.w = (boneM._31 - boneM._13) * mult;
		//		break;

		//	case 2:
		//		boneQuaternion.x = (boneM._31 + boneM._13) * mult;
		//		boneQuaternion.y = (boneM._23 + boneM._32) * mult;
		//		boneQuaternion.z = v;
		//		boneQuaternion.w = (boneM._12 - boneM._21) * mult;
		//		break;

		//	case 3:
		//		boneQuaternion.x = (boneM._23 - boneM._32) * mult;
		//		boneQuaternion.y = (boneM._31 - boneM._13) * mult;
		//		boneQuaternion.z = (boneM._12 - boneM._21) * mult;
		//		boneQuaternion.w = v;
		//		break;
		//	}
		//}

		weaponRight = { boneM._11,boneM._12, boneM._13 };
		weaponUp = { boneM._21,boneM._22, boneM._23 };
		weaponForward = { boneM._31,boneM._32, boneM._33 };

		DirectX::XMMATRIX _mat = DirectX::XMLoadFloat4x4(&boneM);
		DirectX::XMVECTOR _orient = DirectX::XMQuaternionRotationMatrix(_mat);

		DirectX::XMStoreFloat4(&boneQuaternion, _orient);
	}

	m_weapon[m_weaponID]->m_orient = boneQuaternion;

	DirectX::XMFLOAT3 weaponPos = m_mesh.getBonePos("SKT_Weapon");

	//static float adjust = 0.0f;
	//static float adjust1 = 0.0f;

	//if (pad.PushTrigger(XINPUT_GAMEPAD_DPAD_UP, GamePad::PlayerID::_1P))
	//{
	//	adjust += 0.01f;
	//}

	//if (pad.PushTrigger(XINPUT_GAMEPAD_DPAD_DOWN, GamePad::PlayerID::_1P))
	//{
	//	adjust -= 0.01f;
	//}
	//if (pad.PushTrigger(XINPUT_GAMEPAD_DPAD_RIGHT, GamePad::PlayerID::_1P))
	//{
	//	adjust1 += 0.01f;
	//}

	//if (pad.PushTrigger(XINPUT_GAMEPAD_DPAD_LEFT, GamePad::PlayerID::_1P))
	//{
	//	adjust1 -= 0.01f;
	//}

	//if (pad.PushTrigger(XINPUT_GAMEPAD_LEFT_THUMB, GamePad::PlayerID::_1P))
	//{
	//	adjust = 0.0f;
	//	adjust1 = 0.0f;
	//}

	//走ってたら武器の位置の補正設定
	//if (m_animID == m_runAnim) {
	//	adjust = 0.1f;
	//}
	//else {
	//	adjust = 0.0f;
	//}

	weaponPos = Vec3Add(weaponPos, Vec3Multiply(weaponRight, 0.13f));
	weaponPos = Vec3Add(weaponPos, Vec3Multiply(weaponUp, 0.1f));
	weaponPos = Vec3Add(weaponPos, Vec3Multiply(weaponForward, 0.7f));

	m_weapon[m_weaponID]->setPos(weaponPos);
}
