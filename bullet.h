#pragma once

#include "mesh.h"
#include "physics.h"

#include <vector>

enum bulletType
{
	Shot = 0,
	Bomb,
	Max,
};

class bullet :public physics
{
public:
	bullet() :m_orient(DirectX::XMFLOAT4()), m_pos(DirectX::XMFLOAT3()), m_prevPos(DirectX::XMFLOAT3()), m_accel(DirectX::XMFLOAT3()), m_speed(DirectX::XMFLOAT3()), m_attackPower(0), lifeTime(180), m_type(0) {}
	bullet(const DirectX::XMFLOAT4& orient, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& accel, const DirectX::XMFLOAT3& speed, const float attackPower, const int type = (int)bulletType::Shot)
		:m_orient(orient), m_pos(pos), m_prevPos(pos), m_accel(accel), m_speed(speed), m_attackPower(attackPower), lifeTime(180), m_type(type)
	{
	}

	~bullet() { release(); }

	void update();
	void render(my_mesh& mesh, std::unique_ptr<shader_ex>& shader, main_camera *m_camera, DirectX::XMFLOAT4& light_dir);
	void release();

public:
	//�Q�b�^�[�֐�
	DirectX::XMFLOAT3 getPos() { return m_pos; }	//�ʒu
	DirectX::XMFLOAT3 getPrevPos() { return m_prevPos; }	//1�t���[���O�̈ʒu

	int getLifeTime() { return lifeTime; }			//��������

	int getType() { return m_type; }

	void setLifeTime(const int time) { lifeTime = time; }

	void reflect(const DirectX::XMFLOAT3 pos);	//���˕Ԃ�

private:
	DirectX::XMFLOAT4 m_orient;	//�p��
	DirectX::XMFLOAT3 m_pos;	//�ʒu
	DirectX::XMFLOAT3 m_prevPos;	//1�t���[���O�̈ʒu

	DirectX::XMFLOAT3 m_accel;	//�ړ������x
	DirectX::XMFLOAT3 m_speed;	//�ړ����x

	float reflectSpeed = 1.3f;		//���˕Ԃ�Ƃ��̃X�s�[�h
	float m_attackPower;	//�U����

	int lifeTime;		//��������

	int m_type;		//�e�̎�ޔ��ʂ�test����
};


class bullet_manager
{
public:
	std::vector<bullet*> bullets;

public:
	bullet_manager() :bullets(), m_mesh(my_mesh()) { init(); }
	~bullet_manager() { release(); }

	void init();
	void update();
	void render(std::unique_ptr<shader_ex>& shader, main_camera *m_camera, DirectX::XMFLOAT4& light_dir);
	void release();

public:
	//�V�����e�̐ݒ�ƒǉ�
	void bulletSet(const DirectX::XMFLOAT4& orient, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& accel, const DirectX::XMFLOAT3& speed, const float attackPower, const int type = bulletType::Shot);

private:
	my_mesh m_mesh;	//���f��
};