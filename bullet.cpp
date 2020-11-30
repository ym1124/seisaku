#include "bullet.h"

void bullet::update()
{
	m_prevPos = m_pos;
	if (lifeTime > 0) {
		lifeTime--;
	}

	switch (m_type) {
	case bulletType::Shot:
		//移動速度の更新
		m_speed = Vec3Add(m_speed, m_accel);

		//位置の更新
		m_pos = Vec3Add(m_pos, m_speed);
		break;

	case bulletType::Bomb:
		//移動速度の更新
		if (lifeTime < 170) {
			m_speed = Vec3Add(m_speed, m_accel);
		}

		//位置の更新
		m_pos = Vec3Add(m_pos, m_speed);
		break;
	}
}

//#include "..\..\Lib\Header\3dDebugRender.h"

void bullet::render(my_mesh& mesh, std::unique_ptr<shader_ex>& shader, MainCamera *m_camera, DirectX::XMFLOAT4& light_dir)
{
	//描画用変数の更新
	mesh.orientation = m_orient;
	mesh.pos = m_pos;
	mesh.scale = DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f);

	//スクリーン内にあれば描画
	if (InScreenCheak(m_camera->GetView(), m_camera->GetProjection(), mesh.pos, DirectX::XMFLOAT3(5.0f, 5.0f, 5.0f))) {
		if (m_type == (int)bulletType::Bomb) {
			mesh.scale = Vec3Multiply(mesh.scale, 1.5f);
		}

		mesh.render(shader, m_camera, light_dir, true);
	}
}

void bullet::release()
{
}

void bullet::reflect(const DirectX::XMFLOAT3 pos)
{
	//跳ね返り係数
	const float reflection = 0.77f;

	m_pos = pos;
	m_speed.y = reflectSpeed;
	reflectSpeed *= reflection;
}



void bullet_manager::init()
{
	bullets.clear();

	//メッシュのロード
	m_mesh.initialize();
	m_mesh.load("FBX/Bullet/Bullet.bin");
	m_mesh.setMaterialTex(-1, L"FBX/Texture/Bullet/Bullet_B.png");

	m_mesh.scale = DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f);
}

void bullet_manager::update()
{
	for (int i = 0; i < (int)bullets.size(); i++) {
		bullets[i]->update();
	}

	//生存時間を過ぎた弾の削除
	for (int i = 0; i < (int)bullets.size(); i++) {
		if (bullets[i]->getLifeTime() == 0) {
			delete bullets[i];
			bullets[i] = nullptr;
			bullets.erase(bullets.begin() + i);
			i--;
		}
	}
}

void bullet_manager::render(std::unique_ptr<shader_ex>& shader,MainCamera *m_camera, DirectX::XMFLOAT4& light_dir)
{
	for (int i = 0; i < (int)bullets.size(); i++) {
		bullets[i]->render(m_mesh, shader, m_camera, light_dir);
	}
}

void bullet_manager::release()
{
	for (int i = 0; i < (int)bullets.size(); i++) {
		delete bullets[i];
		bullets[i] = nullptr;
	}

	bullets.clear();
}

void bullet_manager::bulletSet(const DirectX::XMFLOAT4& orient, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& accel, const DirectX::XMFLOAT3& speed, const float attackPower, const int type)
{
	//弾の追加
	bullets.push_back(new bullet(orient, pos, accel, speed, attackPower, type));
}
