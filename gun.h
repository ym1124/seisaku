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
	//ゲッター関数
	int getBulletNum() { return m_bulletNum; }			//弾数
	int getShotInterval() { return m_shotInterval; }	//連射速度	

protected:
	bullet_manager* m_bulletManager;	//弾の管理クラス

	int m_bulletNum;	//弾数
	int m_bulletNumMax;	//弾数上限
	int m_shotInterval;	//連射速度
	int m_shotTimer;	//弾発射するための時間

	void reLoad();

private:
	bool isReLoad; //リロードするかどうかをきめる trueならする
};

//ハンドガン
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

//ショットガン
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

//マシンガン
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

