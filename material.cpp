#include "ray_material.h"	



DirectX::XMFLOAT3 diffuse::sample(DirectX::XMFLOAT3& wo, DirectX::XMFLOAT3& wi, float &pdf)
{
	//��l����
	float u = rnd();
	float v = rnd();

	//theta,phi
	float theta = 0.5f*std::acos(1 - 2 * u);
	float phi = 2 * DirectX::XM_PI*v;

	//x,y,z�v�Z
	float x = std::cos(phi)*std::sin(theta);
	float y = std::cos(theta);
	float z = std::sin(phi)*std::sin(theta);

	if (x < 0)
		int a=0;

	//�T���v�����O���ꂽ����
	wi = DirectX::XMFLOAT3(x, y, z);

	//�m�����x�֐��̒l
	pdf = std::cos(theta) / DirectX::XM_PI;

	return rho / DirectX::XM_PI;
}

DirectX::XMFLOAT3 mirror::sample(DirectX::XMFLOAT3& wo, DirectX::XMFLOAT3& wi, float &pdf)
{
	wi = reflect(wo, DirectX::XMFLOAT3(0, 1, 0));

	pdf = 1.0f;
	return 1 / cosTheta(wi)*DirectX::XMFLOAT3(1, 1, 1);
}

//�x�N�g��v�͐N�������Ƌt����(n�Ɠ��������������Ă�)�ł���O��I
DirectX::XMFLOAT3 reflect(DirectX::XMFLOAT3& v, DirectX::XMFLOAT3& n)
{
	return -v + 2 * dot(v, n)*n;
}

float cosTheta(DirectX::XMFLOAT3 &v)
{
	return v.y;
}