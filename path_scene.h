#ifndef	_SCENE_H_
#define	_SCENE_H_

#include "path_constant.h"
#include "path_sphere.h"
#include "path_intersection.h"

namespace path_tracing 
{
	// �����_�����O����V�[���f�[�^
	const Sphere spheres[] = {
		Sphere(1e5, Vec(1e5 + 1, 40.8, 81.6), Color(),      Color(0.75, 0.25, 0.25), REFLECTION_TYPE_DIFFUSE), // ��
		Sphere(1e5, Vec(-1e5 + 99, 40.8, 81.6),Color(),      Color(0.25, 0.25, 0.75), REFLECTION_TYPE_DIFFUSE), // �E
		Sphere(1e5, Vec(50, 40.8, 1e5),      Color(),      Color(0.75, 0.75, 0.75), REFLECTION_TYPE_DIFFUSE), // ��
		Sphere(1e5, Vec(50, 40.8, -1e5 + 250), Color(),      Color(),                 REFLECTION_TYPE_DIFFUSE), // ��O
		Sphere(1e5, Vec(50, 1e5, 81.6),      Color(),      Color(0.75, 0.75, 0.75), REFLECTION_TYPE_DIFFUSE), // ��
		Sphere(1e5, Vec(50, -1e5 + 81.6, 81.6),Color(),      Color(0.75, 0.75, 0.75), REFLECTION_TYPE_DIFFUSE), // �V��
		Sphere(20,Vec(65, 20, 20),           Color(),      Color(0.25, 0.75, 0.25), REFLECTION_TYPE_DIFFUSE), // �΋�
		Sphere(16.5,Vec(27, 16.5, 47),       Color(),      Color(0.99, 0.99, 0.99), REFLECTION_TYPE_SPECULAR), // ��
		Sphere(16.5,Vec(77, 16.5, 78),       Color(),      Color(0.99, 0.99, 0.99), REFLECTION_TYPE_REFRACTION), //�K���X
		Sphere(15.0,Vec(50.0, 90.0, 81.6),   Color(36,36,36), Color(),              REFLECTION_TYPE_DIFFUSE), //�Ɩ�
	};

	// �V�[���Ƃ̌�������֐�
	inline bool intersect_scene(const Ray &ray, Intersection *intersection) {
		const double n = sizeof(spheres) / sizeof(Sphere);

		// ������
		intersection->hitpoint.distance = kINF;
		intersection->object_id = -1;

		// ���`�T��
		for (int i = 0; i < int(n); i++) {
			Hitpoint hitpoint;
			if (spheres[i].intersect(ray, &hitpoint)) {
				if (hitpoint.distance < intersection->hitpoint.distance) {
					intersection->hitpoint = hitpoint;
					intersection->object_id = i;
				}
			}
		}
		return (intersection->object_id != -1);
	}
};

#endif
