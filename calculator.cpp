#include "calculator.h"

//���̑�
float dot(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b)
{
	float temp = a.x*b.x + a.y*b.y + a.z*b.z;
	return temp;
}

DirectX::XMFLOAT3 cross(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b)
{
	DirectX::XMFLOAT3 temp = DirectX::XMFLOAT3(a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
	return temp;
}

float length(DirectX::XMFLOAT3 &a)
{
	float temp = sqrtf(a.x*a.x + a.y*a.y + a.z*a.z);
	return temp;
}


float length2(DirectX::XMFLOAT3 &a)
{
	float temp = a.x*a.x + a.y*a.y + a.z*a.z;
	return temp;
}

DirectX::XMFLOAT3 normalize(DirectX::XMFLOAT3 &a)
{
	DirectX::XMFLOAT3 temp = DirectX::XMFLOAT3(a.x / length(a), a.y / length(a), a.z / length(a));
	return temp;
}

//���K�����������߂�
//https://mathwords.net/seikityokkoukitei
void orthoNormalBasis(DirectX::XMFLOAT3 &v1, DirectX::XMFLOAT3 &v2, DirectX::XMFLOAT3 &v3)
{
	v1 = normalize(v1);
	if (std::abs(v1.x) > 0.9f)//���̎������߂�?
	{
		v2 = DirectX::XMFLOAT3(0, 1, 0);//���������������͂�
	}
	else
	{
		v2 = DirectX::XMFLOAT3(1, 0, 0);//
	}
	v2 = normalize(v2 - dot(v2, v1)*v1);//��{�I��x���ɂȂ镔��
	v3 = normalize(cross(v1, v2));
	//z,x,y
}

std::random_device rnd_dev;
std::mt19937 mt(rnd_dev());
std::uniform_real_distribution<> dist(0, 1);

float rnd()
{
	return dist(mt);
}

float f(float x)
{
	return (x + 1)*std::cos(x);
}

//�m�����x�֐�
float p(float x)
{
	return std::cos(x);
}

//�ݐϕ��z�֐��̋t�֐�
float pInv(float x)
{
	return std::asin(x);
}

//DirectX::XMFLOAT3 localToWorld(DirectX::XMFLOAT3& v, DirectX::XMFLOAT3& s, DirectX::XMFLOAT3& n, DirectX::XMFLOAT3& t)//����
//{
//	//return DirectX::XMFLOAT3(dot(v, s), dot(v, t), dot(v, n));
//	return DirectX::XMFLOAT3(dot(v, s), dot(v, n), dot(v, t));//�������H
//}

DirectX::XMFLOAT3 localToWorld(DirectX::XMFLOAT3& v, DirectX::XMFLOAT3& s, DirectX::XMFLOAT3& t, DirectX::XMFLOAT3& n)//����
{
	return DirectX::XMFLOAT3(dot(v, s), dot(v, t), dot(v, n));
}

//DirectX::XMFLOAT3 worldToLocal(DirectX::XMFLOAT3& v, DirectX::XMFLOAT3& s, DirectX::XMFLOAT3& n, DirectX::XMFLOAT3& t)//
//{
//	DirectX::XMFLOAT3 a = DirectX::XMFLOAT3(s.x, n.x, t.x);
//	DirectX::XMFLOAT3 b = DirectX::XMFLOAT3(s.y, n.y, t.y);
//	DirectX::XMFLOAT3 c = DirectX::XMFLOAT3(s.z, n.z, t.z);
//	return DirectX::XMFLOAT3(dot(v, a), dot(v, b), dot(v, c));
//}

DirectX::XMFLOAT3 worldToLocal(DirectX::XMFLOAT3& v, DirectX::XMFLOAT3& s, DirectX::XMFLOAT3& t, DirectX::XMFLOAT3& n)//����
{
	DirectX::XMFLOAT3 a = DirectX::XMFLOAT3(s.x, n.x, t.x);
	DirectX::XMFLOAT3 b = DirectX::XMFLOAT3(s.y, n.y, t.y);
	DirectX::XMFLOAT3 c = DirectX::XMFLOAT3(s.z, n.z, t.z);
	return DirectX::XMFLOAT3(dot(v, a), dot(v, b), dot(v, c));
}