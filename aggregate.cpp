#pragma once
#include "aggregate.h"

bool aggregate::intersect(ray& r, hit& h)
{
	bool result = false;
	for (auto s : spheres)
	{
		hit temp;
		if (s->intersect(&r, &temp))
		{
			if (temp.getDistance() < h.getDistance())//�P��ڂ͉E����10000�Ȃ̂ō����ɒl�������Ă�Β��ɓ���//2��ڈȍ~�͂ǂ̋��Ɉ�ԍŏ��ɓ����邩����
			{
				result = true;
				h = temp;
			}
		}
	}
	return result;
}

const int MAX_DEPTH = 500;//���ˊK��
const float ROULETTE = 0.99f;//���V�A�����[���b�g�m��
DirectX::XMFLOAT3 radiance(ray* init_ray, aggregate* agg)
{
	DirectX::XMFLOAT3 col = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 throughput = DirectX::XMFLOAT3(1, 1, 1);
	ray update_ray = *init_ray;

	//�����̕]��
	for (int depth = 0; depth < MAX_DEPTH; depth++)
	{
		hit res;
		//ray���V�[���ƏՓ˂����ꍇ
		if (agg->intersect(update_ray, res))
		{
			//�@��
			DirectX::XMFLOAT3 n = res.getHitNormal();//�Փ˒n�_�̖@��
			//���[�J�����W�̍\�z
			DirectX::XMFLOAT3 s, t;
			orthoNormalBasis(n, t, s);//���[���h�ɂ�������W�n
			//�ˏo���������[�J�����W�ɕϊ�
			DirectX::XMFLOAT3 wo_local = worldToLocal(-update_ray.getDirection(), s, n, t);
			//DirectX::XMFLOAT3 wo_local = worldToLocal(reflect(-update_ray.getDirection(), n), s, n, t);

			//�}�e���A���ƌ���
			auto hit_material = res.getHitSphere()->getMaterial();
			auto hit_light = res.getHitSphere()->getLight();

			//Le�̉��Z
			col += throughput*hit_light->Le();

			//�����̃T���v�����O��BRDF�̕]��
			DirectX::XMFLOAT3 brdf;
			DirectX::XMFLOAT3 wi_local;
			float pdf;
			brdf = hit_material->sample(wo_local, wi_local, pdf);

			//�R�T�C��
			float cos = cosTheta(wi_local);

			//���[���h�ϊ�
			DirectX::XMFLOAT3 wi = localToWorld(wi_local, s, n, t);

			//�X���[�v�b�g�̍X�V
			throughput *= brdf*cos / pdf;

			//����ray
			update_ray = ray(res.getHitPos() + 0.001f*res.getHitNormal(), wi);
		}
		else//������Ȃ������ꍇ
		{
			//col += throughput*DirectX::XMFLOAT3(1, 1, 1);
			col += throughput*DirectX::XMFLOAT3(0, 0, 0);//���点��
			break;
		}

		//���V�A�����[���b�g
		float rng = rnd();
		if (rng >= ROULETTE)
		{
			break;
		}
		else
		{
			throughput /= ROULETTE;
		}
	}
	return col;
}