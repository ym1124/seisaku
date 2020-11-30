#pragma once

#include "weapon.h"

class bullet_manager;

class gun :public weapon
{
public:
	gun() :m_bulletNum(0), m_bulletNumMax(0), m_shotInterval(0), m_shotTimer(0), m_bulletManager(nullptr), isReLoad(false), weapon() {}
	virtual ~gun() {};

public:
	virtual void update() = 0;
	virtual void render(std::unique_ptr<shader_ex>& shader, main_camera *m_camera, DirectX::XMFLOAT4& light_dir) = 0;

	void reLoadOn() { isReLoad = true; }
public:
	//�Q�b�^�[�֐�
	int getBulletNum() { return m_bulletNum; }			//�e��
	int getShotInterval() { return m_shotInterval; }	//�A�ˑ��x	

protected:
	bullet_manager* m_bulletManager;	//�e�̊Ǘ��N���X

	int m_bulletNum;	//�e��
	int m_bulletNumMax;	//�e�����
	int m_shotInterval;	//�A�ˑ��x
	int m_shotTimer;	//�e���˂��邽�߂̎���

	void reLoad();

private:
	bool isReLoad; //�����[�h���邩�ǂ��������߂� true�Ȃ炷��
};

//�n���h�K��
class hand_gun :public gun
{
public:
	hand_gun(bullet_manager* bulletManager, const DirectX::XMFLOAT3 pos, const DirectX::XMFLOAT4 orient) :gun() { init(bulletManager, pos, orient); }
	~hand_gun() { release(); }

	void init(bullet_manager* bulletManager, const DirectX::XMFLOAT3 pos, const DirectX::XMFLOAT4 orient);
	void update();
	void render(std::unique_ptr<shader_ex>& shader, main_camera *m_camera, DirectX::XMFLOAT4& light_dir);
	void release();
};

//�V���b�g�K��
class shot_gun :public gun
{
public:
	shot_gun(bullet_manager* bulletManager, const DirectX::XMFLOAT3 pos, const DirectX::XMFLOAT4 orient) :gun() { init(bulletManager, pos, orient); }
	~shot_gun() { release(); }

	void init(bullet_manager* bulletManager, const DirectX::XMFLOAT3 pos, const DirectX::XMFLOAT4 orient);
	void update();
	void render(std::unique_ptr<shader_ex>& shader, main_camera *m_camera, DirectX::XMFLOAT4& light_dir);
	void release();
};

//�}�V���K��
class machine_gun :public gun
{
public:
	machine_gun(bullet_manager* bulletManager, const DirectX::XMFLOAT3 pos, const DirectX::XMFLOAT4 orient) :gun() { init(bulletManager, pos, orient); }
	~machine_gun() { release(); }

	void init(bullet_manager* bulletManager, const DirectX::XMFLOAT3 pos, const DirectX::XMFLOAT4 orient);
	void update();
	void render(std::unique_ptr<shader_ex>& shader, main_camera *m_camera, DirectX::XMFLOAT4& light_dir);
	void release();
};

