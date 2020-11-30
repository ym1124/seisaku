#pragma once

#include "mesh.h"

#include <vector>
#include "camera.h"

class weapon
{
public:
	weapon() :m_mesh(my_mesh()), m_orient(DirectX::XMFLOAT4()), m_pos(DirectX::XMFLOAT3()), m_attackPower(0.0f), m_isActivate(false) {}
	virtual ~weapon() {}

	DirectX::XMFLOAT4 m_orient;	//�p��

public:
	virtual void update() = 0;
	virtual void render(std::unique_ptr<shader_ex>& shader, main_camera *m_camera, DirectX::XMFLOAT4& light_dir) = 0;

	void activeWeapon() { m_isActivate = true; }		//�g�p����Ƃ��Ɉ��Ă�
	void unActiveWeapon() { m_isActivate = false; }		//�g�p�I������Ƃ��Ɉ��Ă�

														//�Q�b�^�[�֐�
	DirectX::XMFLOAT3 getPos() { return m_pos; }		//�ʒu
	DirectX::XMFLOAT3 getForward() { return Vec3Normalize(m_mesh.forward); }	//�v���C���[���琳�ʂւ̃x�N�g��(���K��)

	float getAttackPower() { return m_attackPower; }	//�U����
	bool getIsActivate() { return m_isActivate; }		//�g�p�����ǂ���

	void setPos(const DirectX::XMFLOAT3 pos) { m_pos = pos; }

protected:
	my_mesh m_mesh;	//���f��

	DirectX::XMFLOAT3 m_pos;	//�ʒu

	float m_attackPower;	//�U����

	bool m_isActivate;	//�g�p�����ǂ���
};

class weapon_manager
{
public:
	std::vector<weapon*> weapon;

public:
	weapon_manager() :weapon() { init(); }
	~weapon_manager() { release(); }

	void init();
	void update();
	void render(std::unique_ptr<shader_ex>& shader, main_camera* m_camera, DirectX::XMFLOAT4 light_dir);
	void release();

	//public:
	//	//�V�����e�̐ݒ�ƒǉ�
	//	void BulletSet(const DirectX::XMFLOAT4& orient, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& accel, const DirectX::XMFLOAT3& speed, const float attackPower);
	//
	//private:
};