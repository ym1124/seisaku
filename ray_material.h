#pragma once
#include "calculator.h"

class ray_material
{
public:
	virtual DirectX::XMFLOAT3 sample(DirectX::XMFLOAT3& wo, DirectX::XMFLOAT3& wi, float &pdf) = 0;
};

class diffuse : public ray_material
{
public:
	DirectX::XMFLOAT3 rho;//”½ŽË—¦

	diffuse(DirectX::XMFLOAT3& _rho) : rho(_rho) {};

	DirectX::XMFLOAT3 sample(DirectX::XMFLOAT3& wo, DirectX::XMFLOAT3& wi, float &pdf);
};

class mirror : public ray_material
{
public:
	mirror() {};

	DirectX::XMFLOAT3 sample(DirectX::XMFLOAT3& wo, DirectX::XMFLOAT3& wi, float &pdf);
};

DirectX::XMFLOAT3 reflect(DirectX::XMFLOAT3& v, DirectX::XMFLOAT3& n);

float cosTheta(DirectX::XMFLOAT3 &v);

