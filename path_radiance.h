#ifndef _RADIANCE_H_
#define _RADIANCE_H_

#include <algorithm>

#include "path_ray.h"
#include "path_scene.h"
#include "path_sphere.h"
#include "path_intersection.h"
#include "path_random.h"

namespace path_tracing
{
	const Color kBackgroundColor = Color(0.0, 0.0, 0.0);//�w�i�F
	const int kDepth = 5; // ���V�A�����[���b�g�őł��؂�Ȃ��ő�[�x
	const int kDepthLimit = 64;

	//ray��������̕��ˋP�x�����߂�֐�//Lo�̓����e�J�����ɂ��m���I�ɋ��܂�̂ł��̊֐����m���I�Ȋ֐�
	Color radiance(const Ray &ray, Random *rnd, const int depth)
	{
		Intersection intersection;
		// �V�[���ƌ�������
		if (!intersect_scene(ray, &intersection))//���̂ƌ������Ȃ�������
			return kBackgroundColor;//�w�i�F��Ԃ��ďI��

		const Sphere &now_object = spheres[intersection.object_id];//����������
		const Hitpoint &hitpoint = intersection.hitpoint;//�����_���
		//ray�ƕ��̖@���̓��ς̐����ɂ���ĕ��̂̒�����Փ˂���̂��O����Փ˂���̂�����ł���
		const Vec orienting_normal = dot(hitpoint.normal, ray.dir) < 0.0 ? hitpoint.normal : (-1.0 * hitpoint.normal); //�����ʒu�̖@���i���̂���̃��C�̓��o���l���j


		//���˂����I�u�W�F�N�g�̂����A�F�̔��˗��ő�̂��̂𓾂�//���V�A�����[���b�g�Ŏg��
		//���˗����傫�������̌�̍ċA�̉e�����͂��Ȃ̂őł��؂���m����������
		double russian_roulette_probability = std::max(now_object.color.x, std::max(now_object.color.y, now_object.color.z));

		// ���ˉ񐔂����ȏ�ɂȂ����烍�V�A�����[���b�g�̊m�����}�㏸������i�X�^�b�N�I�[�o�[�t���[�΍�j
		//�ċA������ł��؂��₷���Ȃ�
		if (depth > kDepthLimit)
			russian_roulette_probability *= pow(0.5, depth - kDepthLimit);

		//���V�A�����[���b�g�����s���ǐՂ�ł��؂邩�ǂ����𔻒f����
		//������Depth��̒ǐՂ͕ۏႳ���
		if (depth > kDepth) {
			if (rnd->next01() >= russian_roulette_probability)
				return now_object.emission;
		}
		else
			russian_roulette_probability = 1.0; //���V�A�����[���b�g���s���Ȃ�����//1.0f�Ȃ�ł��؂��Ȃ�

		Color incoming_radiance;//���ˋP�x(���ʂ̏o�͐�)
		Color weight = 1.0;

		switch (now_object.reflection_type)
		{//���˂��镨�̂ɂ���ď������ω�
			// ���S�g�U��
		case REFLECTION_TYPE_DIFFUSE:
		{
			// orienting_normal�̕�������Ƃ������K�������(w, u, v)�����B���̊��ɑ΂��锼�����Ŏ��̃��C���΂��B
			Vec w, u, v;
			w = orienting_normal;
			if (fabs(w.x) > kEPS) // �x�N�g��w�ƒ�������x�N�g�������Bw.x��0�ɋ߂��ꍇ�Ƃ����łȂ��ꍇ�ƂŎg���x�N�g����ς���B
				u = normalize(cross(Vec(0.0, 1.0, 0.0), w));
			else
				u = normalize(cross(Vec(1.0, 0.0, 0.0), w));
			v = cross(w, u);
			// �R�T�C�������g�����d�_�I�T���v�����O
			const double r1 = 2 * kPI * rnd->next01();
			const double r2 = rnd->next01(), r2s = sqrt(r2);
			Vec dir = normalize((
				u * cos(r1) * r2s +
				v * sin(r1) * r2s +
				w * sqrt(1.0 - r2)));

			incoming_radiance = radiance(Ray(hitpoint.position, dir), rnd, depth + 1);
			// �����_�����O�������ɑ΂��郂���e�J�����ϕ����l����ƁAoutgoing_radiance = weight * incoming_radiance
			// �����ŁAweight = (��/��) * cos�� / pdf(��) / R �ɂȂ�
			// ��/�΂͊��S�g�U�ʂ�BRDF�Ńς͔��˗��Acos�Ƃ̓����_�����O�������ɂ�����R�T�C�����Apdf(��)�̓T���v�����O�����ɂ��Ă̊m�����x�֐�
			// R�̓��V�A�����[���b�g�̊m��
			// ���A�R�T�C�����ɔ�Ⴕ���m�����x�֐��ɂ��T���v�����O���s���Ă��邽�߁Apdf(��) = cos��/��
			// ����āAweight = ��/ R
			weight = now_object.color / russian_roulette_probability;
		} break;

		// ���S����
		case REFLECTION_TYPE_SPECULAR:
		{
			// ���S���ʂȂ̂Ń��C�̔��˕����͌���I�B
			// ���V�A�����[���b�g�̊m���ŏ��Z����̂͏�Ɠ����B
			incoming_radiance = radiance(Ray(hitpoint.position, ray.dir - hitpoint.normal * 2.0 * dot(hitpoint.normal, ray.dir)), rnd, depth + 1);
			weight = now_object.color / russian_roulette_probability;
		} break;

		// ���ܗ�kIor�̃K���X
		case REFLECTION_TYPE_REFRACTION:
		{
			const Ray reflection_ray = Ray(hitpoint.position, ray.dir - hitpoint.normal * 2.0 * dot(hitpoint.normal, ray.dir));
			const bool into = dot(hitpoint.normal, orienting_normal) > 0.0; // ���C���I�u�W�F�N�g����o��̂��A����̂�

			// Snell�̖@��
			const double nc = 1.0; // �^��̋��ܗ�
			const double nt = kIor; // �I�u�W�F�N�g�̋��ܗ�
			const double nnt = into ? nc / nt : nt / nc;
			const double ddn = dot(ray.dir, orienting_normal);
			const double cos2t = 1.0 - nnt * nnt * (1.0 - ddn * ddn);

			if (cos2t < 0.0) { // �S����
				incoming_radiance = radiance(reflection_ray, rnd, depth + 1);
				weight = now_object.color / russian_roulette_probability;
				break;
			}

			// ���܂̕���
			const Ray refraction_ray = Ray(hitpoint.position,
				normalize(ray.dir * nnt - hitpoint.normal * (into ? 1.0 : -1.0) * (ddn * nnt + sqrt(cos2t))));

			// Schlick�ɂ��Fresnel�̔��ˌW���̋ߎ����g��
			const double a = nt - nc, b = nt + nc;
			const double R0 = (a * a) / (b * b);

			const double c = 1.0 - (into ? -ddn : dot(refraction_ray.dir, -1.0 * orienting_normal));
			const double Re = R0 + (1.0 - R0) * pow(c, 5.0); // ���˕����̌������˂���ray.dir�̕����ɉ^�Ԋ���//�����ɋ��ܕ����̌������˂�������ɉ^�Ԋ���
			const double nnt2 = pow(into ? nc / nt : nt / nc, 2.0); // ���C�̉^�ԕ��ˋP�x�͋��ܗ��̈قȂ镨�̊Ԃ��ړ�����Ƃ��A���ܗ��̔�̓��̕������ω�����
			const double Tr = (1.0 - Re) * nnt2; // ���ܕ����̌������܂���ray.dir�̕����ɉ^�Ԋ���

			// ���ȏヌ�C��ǐՂ�������܂Ɣ��˂̂ǂ��炩�����ǐՂ���(�����Ȃ��Ǝw���I�Ƀ��C��������)
			// ���V�A�����[���b�g�Ō��肷��
			const double probability = 0.25 + 0.5 * Re;
			if (depth > 2)
			{
				if (rnd->next01() < probability)
				{ // ����
					incoming_radiance = radiance(reflection_ray, rnd, depth + 1) * Re;
					weight = now_object.color / (probability * russian_roulette_probability);
				}
				else
				{ // ����
					incoming_radiance = radiance(refraction_ray, rnd, depth + 1) * Tr;
					weight = now_object.color / ((1.0 - probability) * russian_roulette_probability);
				}
			}
			else
			{ // ���܂Ɣ��˂̗�����ǐ�
				incoming_radiance =
					radiance(reflection_ray, rnd, depth + 1) * Re +
					radiance(refraction_ray, rnd, depth + 1) * Tr;
				weight = now_object.color / (russian_roulette_probability);
			}
		}
		break;
		}
		return now_object.emission + multiply(weight, incoming_radiance);
	}
};

#endif
