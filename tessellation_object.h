#pragma once
#include <memory>
#include <directxmath.h>
#include <d3d11.h>
#include "shader.h"
#include "tessellation_mesh.h"
#include "camera.h"
#include "texture.h"
#include "dynamic_shader_link.h"

class camera;

class tessellation_object
{
private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 angle;
	DirectX::XMFLOAT4X4 world;

	DirectX::XMFLOAT3 forward;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 right;
	float angle_axis;

	DirectX::XMFLOAT3 position_for_raypick = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 scale_for_raypick = DirectX::XMFLOAT3(1, 1, 1);
	DirectX::XMFLOAT3 angle_for_raypick = DirectX::XMFLOAT3(0, 0, 0);

	std::shared_ptr<tessellation_mesh> mesh;
	std::unique_ptr<BASE_SHADING> base_shading;
	bool DSLflg;
	int DSLMode;
public:
	//初期化
	tessellation_object(std::shared_ptr<tessellation_mesh>& s_mesh);
	~tessellation_object();

	//更新
	virtual void update();

	//描画
	void render(ID3D11DeviceContext *immediate_context, camera* m_camera, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color = DirectX::XMFLOAT4(0, 0, 1, 0), bool wire_frame = false, int tess_factor = 1, int inside_tess_factor = 1, float depth = 0.1f);
	void render(ID3D11DeviceContext *immediate_context, camera* m_camera, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color, bool wire_frame, int tess_factor, int inside_tess_factor, float depth, ID3D11ShaderResourceView *srv);
	//情報更新用関数
	void setPosition(const DirectX::XMFLOAT3& p) { position = p; };
	void setScale(const DirectX::XMFLOAT3& s) { scale = s; };
	void setAngle(const DirectX::XMFLOAT3& a) { angle = a; };
	void setWorld(const DirectX::XMFLOAT4X4 m) { world = m; };

	void setOldPosition(const DirectX::XMFLOAT3& p) { position_for_raypick = p; };
	void setOldScale(const DirectX::XMFLOAT3& a) { scale_for_raypick = a; };
	void setOldAngle(const DirectX::XMFLOAT3& a) { angle_for_raypick = a; };

	void setForward(DirectX::XMFLOAT3 forward) { this->forward = forward; };
	void setUp(DirectX::XMFLOAT3 up) { this->up = up; };
	void setRight(DirectX::XMFLOAT3 right) { this->right = right; };
	void setAngleAxis(float angle) { angle_axis = angle; }

	const DirectX::XMFLOAT3& getPosition() const { return position; };
	const DirectX::XMFLOAT3& getScale() const { return scale; };
	const DirectX::XMFLOAT3& getAngle() const { return angle; };
	const DirectX::XMFLOAT4X4& getWorld() const { return world; };
	const std::shared_ptr<tessellation_mesh>& getMesh() const { return mesh; };

	const DirectX::XMFLOAT3& getOldPosition() const { return position_for_raypick; };
	const DirectX::XMFLOAT3& getOldScale() const { return scale_for_raypick; };
	const DirectX::XMFLOAT3& getOldAngle() const { return angle_for_raypick; };

	const DirectX::XMFLOAT3 getForward() { return forward; };
	const DirectX::XMFLOAT3 getUp() { return up; };
	const DirectX::XMFLOAT3 getRight() { return right; };
	const float getAngleAxis() { return angle_axis; };

	ID3D11ShaderResourceView* getSRV() { return mesh->Materials.at(0).pTexture; };

public:
	//ゲーム用
	void move();
};
