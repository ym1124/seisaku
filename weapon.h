#pragma once

#include "mesh.h"

#include <vector>
#include "camera.h"

class weapon
{
public:
	weapon() :m_mesh(my_mesh()), m_orient(DirectX::XMFLOAT4()), m_pos(DirectX::XMFLOAT3()), m_attackPower(0.0f), m_isActivate(false) {}
	virtual ~weapon() {}

	DirectX::XMFLOAT4 m_orient;	//姿勢

public:
	virtual void update() = 0;
	virtual void render(std::unique_ptr<shader_ex>& shader, main_camera *m_camera, DirectX::XMFLOAT4& light_dir) = 0;

	void activeWeapon() { m_isActivate = true; }		//使用するときに一回呼ぶ
	void unActiveWeapon() { m_isActivate = false; }		//使用終了するときに一回呼ぶ

														//ゲッター関数
	DirectX::XMFLOAT3 getPos() { return m_pos; }		//位置
	DirectX::XMFLOAT3 getForward() { return Vec3Normalize(m_mesh.forward); }	//プレイヤーから正面へのベクトル(正規化)

	float getAttackPower() { return m_attackPower; }	//攻撃力
	bool getIsActivate() { return m_isActivate; }		//使用中かどうか

	void setPos(const DirectX::XMFLOAT3 pos) { m_pos = pos; }

protected:
	my_mesh m_mesh;	//モデル

	DirectX::XMFLOAT3 m_pos;	//位置

	float m_attackPower;	//攻撃力

	bool m_isActivate;	//使用中かどうか
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
	//	//新しい弾の設定と追加
	//	void BulletSet(const DirectX::XMFLOAT4& orient, const DirectX::XMFLOAT3& pos, const DirectX::XMFLOAT3& accel, const DirectX::XMFLOAT3& speed, const float attackPower);
	//
	//private:
};