#pragma once
#include <DirectXMath.h>
#include <d3d11.h>
#include "static_object.h"
#include "skin_object.h"

class static_object;
class skin_object;

//���f���̑傫���ɍ��킹�Đ؂���͈͕ς��Ȃ��Ƃ���
class camera
{
private:
	DirectX::XMFLOAT4X4 view;			// ����ϊ��s��
	DirectX::XMFLOAT4X4 projection;
	DirectX::XMFLOAT4X4 view_projection;
	//XMMATRIX�͌v�Z�ŕK�v�ɂȂ������ɐ錾���Ďg��
	//DirectX::XMMATRIX viewM;			// ����ϊ��s��
	//DirectX::XMMATRIX projectionM;
	DirectX::XMFLOAT3 eye;		// ���_
	DirectX::XMFLOAT3 focus;	// �����_
	DirectX::XMFLOAT3 up;		// ��x�N�g��
	float fov;			// ��p
	float aspect;		// �A�X�y�N�g��
	float nearZ;		// �j�A�N���b�v�ʂ܂ł̋���
	float farZ;		// �t�@�[�N���b�v�ʂ܂ł̋���
	float width; //orthographic�̕�
	float height; //orthographic�̍���

public:
	//�J�����p�R���X�^���g�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> constant_buffer_light;
	struct camera_cb
	{
		DirectX::XMFLOAT4 eye;
		DirectX::XMFLOAT4 dir;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4X4 view_proj;
		DirectX::XMFLOAT4X4 inv_view;
		DirectX::XMFLOAT4X4 inv_projection;
		DirectX::XMFLOAT4X4 inv_view_proj;
	};

	/*camera_cb ccb;*/

	// �s��X�V
	void active(bool PerspectiveFlg = true);

	//�Z�b�^�[
	void setPerspective(float fov, float aspect, float nearZ, float farZ);
	void setOrthographic(float width, float height, float nearZ, float farZ);
	void setEye(const DirectX::XMFLOAT3& eye) { this->eye = eye; };
	void setFocus(const DirectX::XMFLOAT3& focus) { this->focus = focus; };
	void setUp(const DirectX::XMFLOAT3& up) { this->up = up; };
	void setFov(const float fov) { this->fov = fov; };
	void setAspect(const float aspect) { this->aspect = aspect; };
	void setWidthandHeight(const DirectX::XMFLOAT2 wh) { this->width = wh.x; this->height = wh.y; };
	void setNearZ(const float nz) { nearZ = nz; };
	void setFarZ(const float fz) { farZ = fz; };

	//�Q�b�^�[
	const DirectX::XMFLOAT4X4& getView() const { return view; };
	const DirectX::XMFLOAT4X4& getProjection() const { return projection; };
	const DirectX::XMFLOAT4X4& getViewProjection() const { return view_projection; };

	const DirectX::XMFLOAT3& getEye() const { return eye; };
	const DirectX::XMFLOAT3& getFocus() const { return focus; };
	const DirectX::XMFLOAT3& getUp() const { return up; };

	float getFarZ() { return farZ; }
	float getNearZ() { return nearZ; }
};

class main_camera : public camera
{
public:
	enum Mode
	{
		MODE_NORMAL,
		MODE_SET,
		MODE_DIR_LIGHT,
		MODE_FOCUS,
		MODE_TPS,
		MODE_NUM
	};
private:
#define TPS_RANGE 12
#define DIR_LIGHT_RANGE /*9*/25
	Mode mode = MODE_NORMAL;
	float time = 0.0f;
	float range = 0.0f;
	float angle_x, angle_y;
	float length;
	static_object* target_obj;
	skin_object* target_obj_s;
	DirectX::XMFLOAT4 directional_light;

	void vibrate(float elapsedTime);
	void normal();
	void set(DirectX::XMFLOAT3 setPos);
	void focus();
	void dir_light();
	void tps();

public:
	static const constexpr float FAR_DIST = 150.0f;
	static const constexpr float NEAR_DIST = 50.0f;

	main_camera();
	~main_camera();
	void update(float elapsedTime);
	void setMode(Mode mode);
	void setVibration(float range, float time);
	void setTarget(static_object* target) { this->target_obj = target; }
	void setTarget(skin_object* target) { this->target_obj_s = target; }
	void setDirLight(DirectX::XMFLOAT4 dir_light) { DirectX::XMStoreFloat4(&directional_light, DirectX::XMVector4Normalize(DirectX::XMLoadFloat4(&dir_light)));}//���K�� 
	void setReversePos(static_object* target, main_camera *mainCamera);
	void setReversePosY(DirectX::XMFLOAT3 target_pos, main_camera *mainCamera);
	static_object* getTarget() { return target_obj; }
	skin_object* getTarget_s() { return target_obj_s; }
	ID3D11Buffer** getConstantBufferAdress() { return constant_buffer.GetAddressOf(); }
	DirectX::XMFLOAT3 getRandom();

	void move();
	void focusMove(float times = 1.0f);
	void setFocusMoveEye(const DirectX::XMFLOAT2 eye,const float length);
};
