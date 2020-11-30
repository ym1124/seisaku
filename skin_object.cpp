#include "skin_object.h"

skin_object::skin_object(std::shared_ptr<skin_mesh>& s_mesh)
{
	mesh = s_mesh;
}

skin_object::~skin_object()
{
}

void skin_object::update()
{
	//���[���h�ϊ��s��̏�����
	DirectX::XMMATRIX s, r, t;
	DirectX::XMFLOAT4X4 rm;

	//�g��s��
	s = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//��]�s��
	r = DirectX::XMMatrixRotationRollPitchYaw(angle.x*(DirectX::XM_PI / 180.0f), angle.y*(DirectX::XM_PI / 180.0f), angle.z*(DirectX::XM_PI / 180.0f));
	DirectX::XMStoreFloat4x4(&rm, r);
	setRight(DirectX::XMFLOAT3(rm._11, rm._12, rm._13));
	setUp(DirectX::XMFLOAT3(rm._21, rm._22, rm._23));
	setForward(DirectX::XMFLOAT3(rm._31, rm._32, rm._33));
	//�ړ��s��
	t = DirectX::XMMatrixTranslation(position.x, position.y, position.z);

	//�s��̍����ƕϊ�
	DirectX::XMStoreFloat4x4(&world, s*r*t);
}

void skin_object::render(ID3D11DeviceContext * immediate_context, main_camera *m_camera, const DirectX::XMFLOAT4 & light_direction, const DirectX::XMFLOAT4 & material_color, bool wire_frame, float elapsed_time)
{
	////���[���h�r���[�v���W�F�N�V�����쐬
	//DirectX::XMMATRIX�͌v�Z���ɐ錾���Ďg���ĕϐ��Ƃ��Ă͎����Ȃ��l�ɂ���
	DirectX::XMMATRIX wrl = DirectX::XMLoadFloat4x4(&world);
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMMATRIX wvp = wrl*DirectX::XMLoadFloat4x4(&m_camera->getView())*DirectX::XMLoadFloat4x4(&m_camera->getProjection());
	DirectX::XMStoreFloat4x4(&world_view_projection, wvp);

	////���[���h�t�s��
	//DirectX::XMMATRIX world_invM = DirectX::XMMatrixInverse(nullptr, wrl);
	//DirectX::XMFLOAT4X4 world_inv;
	//DirectX::XMStoreFloat4x4(&world_inv, world_invM);

	//Projection,View
	/*DirectX::XMFLOAT4X4 Projection;
	DirectX::XMFLOAT4X4 View;
	Projection=projection;
	View=view;*/

	mesh->render(immediate_context, world_view_projection, world, light_direction, material_color, wire_frame, elapsed_time );
}

void skin_object::render(ID3D11DeviceContext * immediate_context,shader *shader, main_camera *m_camera, const DirectX::XMFLOAT4 & dir_light, const DirectX::XMFLOAT4 & material_color, bool wire_frame, float elapsed_time)
{
	////���[���h�r���[�v���W�F�N�V�����쐬
	//DirectX::XMMATRIX�͌v�Z���ɐ錾���Ďg���ĕϐ��Ƃ��Ă͎����Ȃ��l�ɂ���
	DirectX::XMMATRIX wrl = DirectX::XMLoadFloat4x4(&world);
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMMATRIX wvp = wrl*DirectX::XMLoadFloat4x4(&m_camera->getView())*DirectX::XMLoadFloat4x4(&m_camera->getProjection());
	DirectX::XMStoreFloat4x4(&world_view_projection, wvp);

	////���[���h�t�s��
	//DirectX::XMMATRIX world_invM = DirectX::XMMatrixInverse(nullptr, wrl);
	//DirectX::XMFLOAT4X4 world_inv;
	//DirectX::XMStoreFloat4x4(&world_inv, world_invM);

	//Projection,View
	/*DirectX::XMFLOAT4X4 Projection;
	DirectX::XMFLOAT4X4 View;
	Projection=projection;
	View=view;*/

	//�萔�o�b�t�@�X�V
	ConstantBufferForPhong cb2;
	cb2.ambient_color = Light::Ambient;
	cb2.dir_light.direction = Light::LightDir;
	cb2.dir_light.color = Light::DirLightColor;
	UpdateConstantBuffer(shader, cb2, 1);//�����V�F�[�_�[�g������蒼�����ƂɂȂ�

	mesh->render(immediate_context, shader, world_view_projection, world, dir_light, material_color, wire_frame, elapsed_time);
}

void skin_object::move(int pattern,move_desc md)
{
	switch (pattern)
	{
	case MPATTERN::DEFAULT:
		//static DirectX::XMFLOAT3 position(0, 0, 10);
		if (GetAsyncKeyState(VK_SHIFT) < 0)
		{
			if (GetAsyncKeyState(VK_UP) & 1)
				position.z += 3;
			if (GetAsyncKeyState(VK_DOWN) & 1)
				position.z -= 3;
			if (GetAsyncKeyState(VK_LEFT) & 1)
				position.x -= 3;
			if (GetAsyncKeyState(VK_RIGHT) & 1)
				position.x += 3;
		}

		//DirectX::XMFLOAT3 dimensions(1, 1, 1);

		//static DirectX::XMFLOAT3 angles(0, 0, 0);
		if (GetAsyncKeyState(VK_CONTROL) < 0)
		{
			if (GetAsyncKeyState(VK_UP) & 1)
				angle.x++;
			if (GetAsyncKeyState(VK_DOWN) & 1)
				angle.x--;
			if (GetAsyncKeyState(VK_LEFT) & 1)
				angle.y++;
			if (GetAsyncKeyState(VK_RIGHT) & 1)
				angle.y--;
			//if (GetAsyncKeyState('UP') & 1)
			angle.z = 0;
		}
		break;
	case MPATTERN::CIRCLE:
		float x = cosf((md.timer/10.0f)*(DirectX::XM_PI / 180.0f));
		float z = sinf((md.timer / 10.0f)*(DirectX::XM_PI / 180.0f));
		setPosition(DirectX::XMFLOAT3(x*md.radius + md.center.x,  md.center.y, z*md.radius + md.center.z));
		//float xd = cosf(((md.timer+0.1f) / 10.0f)*(DirectX::XM_PI / 180.0f));//���傢�����̈ʒu
		//float zd = sinf(((md.timer+0.1f) / 10.0f)*(DirectX::XM_PI / 180.0f));
		//DirectX::XMFLOAT3 p = DirectX::XMFLOAT3(xd*md.radius + md.center.x, getPosition().y + md.center.y, zd*md.radius + md.center.z);//���傢�����̈ʒu
		//DirectX::XMFLOAT3 d = DirectX::XMFLOAT3(p.x - getPosition().x, p.y - getPosition().y, p.z - getPosition().z);//�ڕW�Ɍ�����vec
		//DirectX::XMFLOAT3 v = DirectX::XMFLOAT3(md.center.x - getPosition().x, md.center.y - getPosition().y, md.center.z - getPosition().z);
		//DirectX::XMVECTOR v_v = DirectX::XMLoadFloat3(&v);
		//DirectX::XMVECTOR d_v = DirectX::XMLoadFloat3(&d);
		//DirectX::XMVECTOR angle = DirectX::XMVector3Dot(v_v, d_v);
		//float ang;
		//DirectX::XMVECTOR vl = DirectX::XMVector3Length(v_v);
		//DirectX::XMVECTOR dl =DirectX::XMVector3Length(d_v);
		//float v_l,d_l;
		//DirectX::XMStoreFloat(&v_l, vl);
		//DirectX::XMStoreFloat(&d_l, dl);
		//DirectX::XMStoreFloat(&ang, angle);
		//ang = acosf(ang/(v_l*d_l));
		//ang = ang*(180.0f / DirectX::XM_PI);
		//setAngle(DirectX::XMFLOAT3(0, ang, 0));
	}
}

void skin_object::setPosture(DirectX::XMFLOAT3 outNormal)
{
	//���̂R�����O�ς��g���Čv�Z����
	DirectX::XMVECTOR vy = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&outNormal));//�n�ʂ̖@������y��
	DirectX::XMFLOAT3 front = DirectX::XMFLOAT3(sin(this->getAngle().y), 0, cos(this->getAngle().y));
	DirectX::XMVECTOR vz = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&front));//����z
	DirectX::XMVECTOR vx = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vy, vz));//x
	vz = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(vx, vy));//�^��z

																	  // �X�P�[���ݒ� (XMVectorScale��vec��float�̊|���Z)
	const DirectX::XMFLOAT3& s = this->getScale();
	vx = DirectX::XMVectorScale(vx, s.x);
	vy = DirectX::XMVectorScale(vy, s.y);
	vz = DirectX::XMVectorScale(vz, s.z);

	// �e�����擾
	//�ϊ����đ��
	DirectX::XMFLOAT3 x, y, z;
	DirectX::XMStoreFloat3(&x, vx);
	DirectX::XMStoreFloat3(&y, vy);
	DirectX::XMStoreFloat3(&z, vz);

	// �ʒu�擾
	const DirectX::XMFLOAT3& p = this->getPosition();

	// ���[���h�ϊ��s��ݒ�
	DirectX::XMFLOAT4X4 world = {
		x.x,x.y,x.z,0,
		y.x,y.y,y.z,0,
		z.x,z.y,z.z,0,
		p.x,p.y,p.z,1
	};
	this->setWorld(world);
}

