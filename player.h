#pragma once

#include "mesh.h"
#include "physics.h"
#include "gun.h"

#include <vector>

#include "animation.h"

#include "camera.h"
#include "shader_ex.h"
#include "shader.h"

class bullet_manager;

class player :public physics
{
public:
	player() :anim("Idle") { init(); }
	~player() { release(); }

	float cameraAngle;	//�J�����̏����l�����]�����p�x

	void init();
	void update();
	void render(std::unique_ptr<shader_ex>& shader, main_camera* m_camera, DirectX::XMFLOAT4& light_dir);
	void render(std::unique_ptr<shader>& shader, main_camera* m_camera, DirectX::XMFLOAT4& light_dir);
	void release();

	void newWeapon(weapon_manager* weaponManager, bullet_manager* bulletManager);
	void nowWeaponUpdate();	//�����Ă��镐��̈ʒu�Ǝp���̍X�V

	void addDamage(const float damage) { m_damage += damage; }

public:
	//�Z�b�^�[�֐�
	void setPos(DirectX::XMFLOAT3& pos) { m_pos = pos; }		//�ʒu

																//�Q�b�^�[�֐�
	DirectX::XMFLOAT3 &getPos() { return m_pos; }		//�ʒu
	DirectX::XMFLOAT3 getForward() { return Vec3Normalize(m_mesh.forward); }	//�v���C���[���琳�ʂւ̃x�N�g��(���K��)

	DirectX::XMFLOAT3 getBonePos(const char* boneName) { return m_mesh.getBonePos(boneName); }
	DirectX::XMFLOAT3 getBoneDirection(const char* boneName) { return m_mesh.getBoneDirectionY(boneName); }

	DirectX::XMFLOAT4X4 getBoneLotateMatrix(const char* boneName) { return m_mesh._model->getBoneLocalMatrix(boneName); }

	gun* getNowWeapon()
	{
		if (m_weaponID < 0) return nullptr;

		return m_weapon[m_weaponID];
	}

	const float getDamage() { return m_damage; }

	my_mesh* getMesh() { return &m_mesh; };

	int getWeaponID() { return m_weaponID; }

private:
	my_mesh m_mesh;			//���f��
	animation anim;	//�A�j���[�V����

	std::vector<gun*> m_weapon;	//���L����

	DirectX::XMFLOAT3 m_pos;	//�ʒu

	float m_damage;			//�~�σ_���[�W(%)

	int m_weaponID;			//�g�p�������ID

	int m_animID;			//���݂̃A�j���[�V����
	int m_prevAnimID;		//�P�t���[���O�̃A�j���[�V����

	int m_idolAnim;			//�ҋ@�A�j���[�V������ID
	int m_runAnim;			//����A�j���[�V������ID
	int m_omakeAnim;			//�A�j���[�V������ID
};
