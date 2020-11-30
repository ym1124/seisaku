#pragma once
#include <DirectXMath.h>

class ray
{
private:
	DirectX::XMFLOAT3 origin;//Œ´“_
	DirectX::XMFLOAT3 direction;//Œü‚«

public:
	ray(DirectX::XMFLOAT3 o, DirectX::XMFLOAT3 d) :origin(o), direction(d) {};

	//getter
	DirectX::XMFLOAT3 getOrigin() { return origin; }
	DirectX::XMFLOAT3 getDirection() { return direction; }
	DirectX::XMFLOAT3 getEndPoint(float t)
	{
		DirectX::XMFLOAT3 result(origin.x + (direction.x*t), origin.y + (direction.y*t), origin.z + (direction.z*t));
		return result;
	}
};