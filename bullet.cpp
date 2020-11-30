#include "bullet.h"

void bullet::update()
{
	m_prevPos = m_pos;
	if (lifeTime > 0) {
		lifeTime--;
	}

	switch (m_type) {
	case bulletType::Shot:
		//�ړ����x�̍X�V
		m_speed = Vec3Add(m_speed, m_accel);

		//�ʒu�̍X�V
		m_pos = Vec3Add(m_pos, m_speed);
		break;

	case bulletType::Bomb:
		//�ړ����x�̍X�V
		if (lifeTime < 170) {
			m_speed = Vec3Add(m_speed, m_accel);
		}

		//�ʒu�̍X�V
		m_pos = Vec3Add(m_pos, m_speed);
		break;
	}
}

//#include "..\..\Lib\Header\3dDebugRender.h"

void bullet::render(my_mesh& mesh, std::unique_ptr<shader_ex>& shader, MainCamera *m_camera, DirectX::XMFLOAT4& light_dir)
{
	//�`��p�ϐ��̍X�V
	mesh.orientation = m_orient;
	mesh.pos = m_pos;
	mesh.scale = DirectX::XMFLOAT3(-1.0f, 1.0f, 1.0f);

	//�X�N���[�����ɂ���Ε`��
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
	//���˕Ԃ�W��
	const float reflection = 0.77f;

	m_pos = pos;
	m_speed.y = reflectSpeed;
	reflectSpeed *= reflection;
}



void bullet_manager::init()
{
	bullets.clear();

	//���b�V���̃��[�h
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

	//�������Ԃ��߂����e�̍폜
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
	//�e�̒ǉ�
	bullets.push_back(new bullet(orient, pos, accel, speed, attackPower, type));
}
