#pragma once
#include "directx_operator.h"

//XMFLOAT3ìØém
DirectX::XMFLOAT3 operator+(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b)
{
	DirectX::XMFLOAT3 temp = DirectX::XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
	return temp;
}

DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b)
{
	DirectX::XMFLOAT3 temp = DirectX::XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
	return temp;
}

DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b)
{
	DirectX::XMFLOAT3 temp = DirectX::XMFLOAT3(a.x * b.x, a.y * b.y, a.z * b.z);
	return temp;
}

DirectX::XMFLOAT3& operator+= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b)
{
	a = DirectX::XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
	return a;
}

DirectX::XMFLOAT3& operator-= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b)
{
	a = DirectX::XMFLOAT3(a.x - b.x, a.y - b.y, a.z - b.z);
	return a;
}

DirectX::XMFLOAT3& operator*= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b)
{
	a = DirectX::XMFLOAT3(a.x * b.x, a.y * b.y, a.z * b.z);
	return a;
}

DirectX::XMFLOAT3& operator/= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b)
{
	a = DirectX::XMFLOAT3(a.x / b.x, a.y / b.y, a.z / b.z);
	return a;
}


//XMFLOAT3Ç∆ÉXÉJÉâÅ[
DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 &a, float b)
{
	DirectX::XMFLOAT3 temp = DirectX::XMFLOAT3(a.x * b, a.y * b, a.z * b);
	return temp;
}

DirectX::XMFLOAT3 operator*(float a, DirectX::XMFLOAT3 & b)
{
	DirectX::XMFLOAT3 temp = b*a;
	return temp;
}

DirectX::XMFLOAT3 operator/(DirectX::XMFLOAT3 &a, float b)
{
	DirectX::XMFLOAT3 temp = DirectX::XMFLOAT3(a.x / b, a.y / b, a.z / b);
	return temp;
}

DirectX::XMFLOAT3 operator/(float a, DirectX::XMFLOAT3 & b)
{
	DirectX::XMFLOAT3 temp = b / a;
	return temp;
}

DirectX::XMFLOAT3& operator/=(float a, DirectX::XMFLOAT3 & b)
{
	b = b / a;
	return b;
}

DirectX::XMFLOAT3& operator/=(DirectX::XMFLOAT3& a, float b)
{
	a = a / b;
	return a;
}

//ÇªÇÃëº
DirectX::XMFLOAT3& operator-(DirectX::XMFLOAT3 &a)
{
	a = DirectX::XMFLOAT3(-a.x, -a.y, -a.z);
	return a;
}
