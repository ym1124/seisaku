#pragma once
#include <DirectXMath.h>
#include "sphere.h"

class sphere;

class hit
{
private:
	float distance;//rayの衝突点までの距離
	DirectX::XMFLOAT3 hit_pos;//衝突地点
	DirectX::XMFLOAT3 hit_normal;//衝突地点法線
	sphere* hit_sphere;
public:
	hit(float t = 10000) :distance(t) {};

	//setter
	void setDistance(float d) { distance = d; }
	void setHitPos(DirectX::XMFLOAT3 hp) { hit_pos = hp; }
	void setHitNormal(DirectX::XMFLOAT3 hn) { hit_normal = hn; }
	void setSphere(sphere* s) { hit_sphere = s; }

	//getter
	float getDistance() {return distance;}//rayの衝突点までの距離
	DirectX::XMFLOAT3 getHitPos() {return hit_pos;}//衝突地点
	DirectX::XMFLOAT3 getHitNormal() {return hit_normal;}//衝突地点法線
	sphere* getHitSphere() { return hit_sphere; }
};