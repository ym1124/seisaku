#pragma once

#include <memory>
#include <vector>
#include "ray.h"
#include "hit.h"
#include "sphere.h"

class aggregate
{
public:
	std::vector<std::shared_ptr<sphere>> spheres;//���̂̔z��

	aggregate() {};
	aggregate(const std::vector<std::shared_ptr<sphere>> &_spheres) : spheres(_spheres) {};//�����̃~�X?
	~aggregate() {};

	void add(std::shared_ptr<sphere> &s) { spheres.push_back(s); };

	bool intersect(ray& r, hit& h);
};

DirectX::XMFLOAT3 radiance(ray* init_ray, aggregate* agg);