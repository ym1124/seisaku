#pragma once
#include "camera.h"
#include "ray.h"

class pinhole_camera : public camera 
{
public:
	float pinhole_dist;
	DirectX::XMFLOAT3 right;
	DirectX::XMFLOAT3 up;

	pinhole_camera(DirectX::XMFLOAT3 e, DirectX::XMFLOAT3 f, float dist)
	{
		this->setEye(e);
		this->setFocus(f);
		pinhole_dist = dist;
	}
	~pinhole_camera() {};

	ray getRay(float u, float v);
};