#pragma once

#include "sphere.h"
#include "calculator.h"

bool sphere::intersect(ray* _ray, hit* _hit)
{
	//判別式を利用してrayとsphereが交差するか検証
	float b = dot(_ray->getDirection(), _ray->getOrigin() - getCenter());
	float c = length2(_ray->getOrigin() - getCenter()) - getRadius()*getRadius();

	//判別式
	float D = b*b - c;

	if (D < 0.0f)//接しないため
		return false;

	//解
	float t1 = -b - sqrtf(D);
	float t2 = -b + sqrtf(D);

	if (t1 > 10000 || t2 < 0)
		return false;
	float t = t1;
	if (t < 0)
	{
		t = t2;
		if (t2 > 10000)
			return false;
	}

	_hit->setDistance(t);
	_hit->setHitPos(_ray->getEndPoint(t));
	_hit->setHitNormal(normalize(_hit->getHitPos() - getCenter()));
	_hit->setSphere(this);

	return true;
}

//DirectX::XMVECTOR dir_v = DirectX::XMLoadFloat3(&_ray->getDirection());
//DirectX::XMVECTOR origin_v = DirectX::XMLoadFloat3(&_ray->getOrigin());
//DirectX::XMVECTOR center_v = DirectX::XMLoadFloat3(&getCenter());
//
//DirectX::XMVECTOR bv = DirectX::XMVector3Dot(dir_v, DirectX::XMVectorSubtract(origin_v, center_v));
//float b;
//DirectX::XMStoreFloat(&b, bv);

