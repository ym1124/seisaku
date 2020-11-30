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

	float cameraAngle;	//カメラの初期値から回転した角度

	void init();
	void update();
	void render(std::unique_ptr<shader_ex>& shader, main_camera* m_camera, DirectX::XMFLOAT4& light_dir);
	void render(std::unique_ptr<shader>& shader, main_camera* m_camera, DirectX::XMFLOAT4& light_dir);
	void release();

	void newWeapon(weapon_manager* weaponManager, bullet_manager* bulletManager);
	void nowWeaponUpdate();	//持っている武器の位置と姿勢の更新

	void addDamage(const float damage) { m_damage += damage; }

public:
	//セッター関数
	void setPos(DirectX::XMFLOAT3& pos) { m_pos = pos; }		//位置

																//ゲッター関数
	DirectX::XMFLOAT3 &getPos() { return m_pos; }		//位置
	DirectX::XMFLOAT3 getForward() { return Vec3Normalize(m_mesh.forward); }	//プレイヤーから正面へのベクトル(正規化)

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
	my_mesh m_mesh;			//モデル
	animation anim;	//アニメーション

	std::vector<gun*> m_weapon;	//所有武器

	DirectX::XMFLOAT3 m_pos;	//位置

	float m_damage;			//蓄積ダメージ(%)

	int m_weaponID;			//使用中武器のID

	int m_animID;			//現在のアニメーション
	int m_prevAnimID;		//１フレーム前のアニメーション

	int m_idolAnim;			//待機アニメーションのID
	int m_runAnim;			//走りアニメーションのID
	int m_omakeAnim;			//アニメーションのID
};
