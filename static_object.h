#pragma once
#include <memory>
#include <directxmath.h>
#include <d3d11.h>
#include "shader.h"
#include "static_mesh.h"
#include "camera.h"
#include "texture.h"
#include "dynamic_shader_link.h"

class camera;

class static_object
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

	std::shared_ptr<static_mesh> mesh;
	std::unique_ptr<BASE_SHADING> base_shading;
	bool DSLflg;
	int DSLMode;
public:
	//������
	static_object(std::shared_ptr<static_mesh>& s_mesh, bool DSLflg = false);
	~static_object();

	//�X�V
	virtual void update();
	virtual void update(DirectX::XMFLOAT3 axis, float speed = 2.0f, float rotation = 0.1f);

	//�`��
	void render(ID3D11DeviceContext *immediate_context, camera* m_camera, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color = DirectX::XMFLOAT4(1, 1, 1, 1), bool wire_frame = false, bool cull = true);
	//�V�F�[�_�[�w�肵�ĕ`��
	void render(ID3D11DeviceContext *immediate_context, shader* shader, camera* m_camera,const DirectX::XMFLOAT4& light_direction = DirectX::XMFLOAT4(1, 1, 1, 1), /*const DirectX::XMFLOAT4& material_color = DirectX::XMFLOAT4(1, 1, 1, 1),*/ bool wire_frame = false, bool cull = true);
	//�e�N�X�`�����߂ĕ`��
	void render(ID3D11DeviceContext *immediate_context, shader* shader, Texture *pTex, camera* m_camera, const DirectX::XMFLOAT4& light_direction = DirectX::XMFLOAT4(1, 1, 1, 1), /*const DirectX::XMFLOAT4& material_color = DirectX::XMFLOAT4(1, 1, 1, 1),*/ bool wire_frame = false);
	//�e�b�Z���[�V�������݂ŕ`��
	void static_object::renderTessellation(ID3D11DeviceContext *immediate_context, shader *shader, camera *m_camera, const DirectX::XMFLOAT4& light_direction /*, const DirectX::XMFLOAT4& material_color*/, bool wire_frame);
	//PBR(IBL����)�`��
	//void render(ID3D11DeviceContext *immediate_context, shader* shader, camera* m_camera, environment_cubemap *env_map, const DirectX::XMFLOAT4& light_direction = DirectX::XMFLOAT4(0, 0, 1, 0),/*const DirectX::XMFLOAT4& material_color = DirectX::XMFLOAT4(1, 1, 1, 1),*/ bool wire_frame = false);

	//���X�V�p�֐�
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

	void setGeomrtyShader(shader* sh) { mesh->gs = sh->gs.Get(); };

	const DirectX::XMFLOAT3& getPosition() const { return position; };
	const DirectX::XMFLOAT3& getScale() const { return scale; };
	const DirectX::XMFLOAT3& getAngle() const { return angle; };
	const DirectX::XMFLOAT4X4& getWorld() const { return world; };
	const std::shared_ptr<static_mesh>& getMesh() const { return mesh; };

	const DirectX::XMFLOAT3& getOldPosition() const { return position_for_raypick; };
	const DirectX::XMFLOAT3& getOldScale() const { return scale_for_raypick; };
	const DirectX::XMFLOAT3& getOldAngle() const { return angle_for_raypick; };

	const DirectX::XMFLOAT3 getForward() { return forward; };
	const DirectX::XMFLOAT3 getUp() { return up; };
	const DirectX::XMFLOAT3 getRight() { return right; };
	const float getAngleAxis() { return angle_axis; };

public:
	//DSL�ɂ��`�惂�[�h�ύX
	enum DSLB_MODE { LAMBERT, HALF_LAMBERT, PHONG, NO };
	void changeDSLBasic(int mode);
	//���C�s�b�N
	int RayPick(const DirectX::XMFLOAT3& startPosition, const DirectX::XMFLOAT3& endPosition, DirectX::XMFLOAT3* outPosition, DirectX::XMFLOAT3* outNormal, float* outLength);

	//�Q�[���p
	void move();
	void focusMove(camera *cam);
	void setPosture(DirectX::XMFLOAT3 outNormal);//RayPick�Ǝg��//world����蒼���̂�update�s�v
};
