#ifndef	_SPHERE_H_
#define	_SPHERE_H_

#include <cmath>

#include "path_vec.h"
#include "path_ray.h"
#include "path_material.h"
#include "path_constant.h"
#include "path_intersection.h"

namespace path_tracing 
{
	struct Sphere 
	{
		double radius;
		Vec position;
		Color emission;
		Color color;
		ReflectionType reflection_type;

		Sphere(const double radius, const Vec &position, const Color &emission, const Color &color, const ReflectionType reflection_type) :
			radius(radius), position(position), emission(emission), color(color), reflection_type(reflection_type) {}

		// ���͂�ray�ɑ΂�������_�܂ł̋�����Ԃ��B�������Ȃ�������0��Ԃ��B
		// ray�Ƃ̌���������s���B����������true,�����Ȃ���false��Ԃ��B
		bool intersect(const Ray &ray, Hitpoint *hitpoint) const {
			const Vec p_o = position - ray.org;
			const double b = dot(p_o, ray.dir);
			const double D4 = b * b - dot(p_o, p_o) + radius * radius;

			if (D4 < 0.0)
				return false;

			const double sqrt_D4 = sqrt(D4);
			const double t1 = b - sqrt_D4, t2 = b + sqrt_D4;

			if (t1 < kEPS && t2 < kEPS)
				return false;

			if (t1 > kEPS) {
				hitpoint->distance = t1;
			}
			else {
				hitpoint->distance = t2;
			}

			hitpoint->position = ray.org + hitpoint->distance * ray.dir;
			hitpoint->normal = normalize(hitpoint->position - position);
			return true;
		}
	};
};

#endif
