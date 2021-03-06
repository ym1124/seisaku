#pragma once
#include <DirectXMath.h>
#include "sphere.h"

class sphere;

class hit
{
private:
	float distance;//rayΜΥΛ_άΕΜ£
	DirectX::XMFLOAT3 hit_pos;//ΥΛn_
	DirectX::XMFLOAT3 hit_normal;//ΥΛn_@ό
	sphere* hit_sphere;
public:
	hit(float t = 10000) :distance(t) {};

	//setter
	void setDistance(float d) { distance = d; }
	void setHitPos(DirectX::XMFLOAT3 hp) { hit_pos = hp; }
	void setHitNormal(DirectX::XMFLOAT3 hn) { hit_normal = hn; }
	void setSphere(sphere* s) { hit_sphere = s; }

	//getter
	float getDistance() {return distance;}//rayΜΥΛ_άΕΜ£
	DirectX::XMFLOAT3 getHitPos() {return hit_pos;}//ΥΛn_
	DirectX::XMFLOAT3 getHitNormal() {return hit_normal;}//ΥΛn_@ό
	sphere* getHitSphere() { return hit_sphere; }
};