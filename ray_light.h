#pragma once
#include "calculator.h"

class ray_light
{
public:
	DirectX::XMFLOAT3 color;//åıåπÇÃêF

	ray_light(DirectX::XMFLOAT3 _color) : color(_color) {};

	DirectX::XMFLOAT3 Le() { return color; };
};