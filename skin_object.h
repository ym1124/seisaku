#pragma once
#include <memory>
#include <directxmath.h>
#include <d3d11.h>
#include "skin_mesh.h"
#include "shader.h"
#include "camera.h"

class main_camera;

class skin_object
{
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 angle;

	DirectX::XMFLOAT3 forward;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 right;

	DirectX::XMFLOAT4X4 world;
	std::shared_ptr<skin_mesh> mesh;
	int sum_timer = 0;
public:
	//初期化
	skin_object(std::shared_ptr<skin_mesh>& s_mesh);
	~skin_object();

	//更新
	void update();

	//描画
	void render(ID3D11DeviceContext *immediate_context, main_camera *m_camera,const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color, bool wire_frame, float elapsed_time);
	//シェーダー指定して描画
	void render(ID3D11DeviceContext *immediate_context, shader* shader, main_camera *m_camera,const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color, bool wire_frame, float elapsed_time);

	//情報更新用関数
	void setPosition(const DirectX::XMFLOAT3& p) { position = p; };
	void setScale(const DirectX::XMFLOAT3& s) { scale = s; };
	void setAngle(const DirectX::XMFLOAT3& a) { angle = a; };
	void setWorld(const DirectX::XMFLOAT4X4 m) { world = m; };

	void setForward(DirectX::XMFLOAT3 forward) { this->forward = forward; };
	void setUp(DirectX::XMFLOAT3 up) { this->up = up; };
	void setRight(DirectX::XMFLOAT3 right) { this->right = right; };

	const DirectX::XMFLOAT3& getPosition() const { return position; };
	const DirectX::XMFLOAT3& getScale() const { return scale; };
	const DirectX::XMFLOAT3& getAngle() const { return angle; };
	const DirectX::XMFLOAT4X4& getWorld() const { return world; };
	const std::shared_ptr<skin_mesh>& getMesh() const { return mesh; };//


	//ゲーム用
	//移動パターン
	enum MPATTERN { DEFAULT, CIRCLE };
	struct move_desc
	{
		float timer = 0;
		DirectX::XMFLOAT3 center = DirectX::XMFLOAT3(0, 0, 0);
		float radius = 100.0f;
	};
	void move(int pattern = 0, move_desc md = move_desc());

	void setPosture(DirectX::XMFLOAT3 outNormal);//RayPickと使う//worldを作り直すのでupdate不要
};

