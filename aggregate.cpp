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
			if (temp.getDistance() < h.getDistance())//１回目は右項は10000なので左項に値が入ってれば中に入る//2回目以降はどの球に一番最初に当たるか判定
			{
				result = true;
				h = temp;
			}
		}
	}
	return result;
}

const int MAX_DEPTH = 500;//反射階数
const float ROULETTE = 0.99f;//ロシアンルーレット確率
DirectX::XMFLOAT3 radiance(ray* init_ray, aggregate* agg)
{
	DirectX::XMFLOAT3 col = DirectX::XMFLOAT3(0, 0, 0);
	DirectX::XMFLOAT3 throughput = DirectX::XMFLOAT3(1, 1, 1);
	ray update_ray = *init_ray;

	//級数の評価
	for (int depth = 0; depth < MAX_DEPTH; depth++)
	{
		hit res;
		//rayがシーンと衝突した場合
		if (agg->intersect(update_ray, res))
		{
			//法線
			DirectX::XMFLOAT3 n = res.getHitNormal();//衝突地点の法線
			//ローカル座標の構築
			DirectX::XMFLOAT3 s, t;
			orthoNormalBasis(n, t, s);//ワールドにおける座標系
			//射出方向をローカル座標に変換
			DirectX::XMFLOAT3 wo_local = worldToLocal(-update_ray.getDirection(), s, n, t);
			//DirectX::XMFLOAT3 wo_local = worldToLocal(reflect(-update_ray.getDirection(), n), s, n, t);

			//マテリアルと光源
			auto hit_material = res.getHitSphere()->getMaterial();
			auto hit_light = res.getHitSphere()->getLight();

			//Leの加算
			col += throughput*hit_light->Le();

			//方向のサンプリングとBRDFの評価
			DirectX::XMFLOAT3 brdf;
			DirectX::XMFLOAT3 wi_local;
			float pdf;
			brdf = hit_material->sample(wo_local, wi_local, pdf);

			//コサイン
			float cos = cosTheta(wi_local);

			//ワールド変換
			DirectX::XMFLOAT3 wi = localToWorld(wi_local, s, n, t);

			//スループットの更新
			throughput *= brdf*cos / pdf;

			//次のray
			update_ray = ray(res.getHitPos() + 0.001f*res.getHitNormal(), wi);
		}
		else//当たらなかった場合
		{
			//col += throughput*DirectX::XMFLOAT3(1, 1, 1);
			col += throughput*DirectX::XMFLOAT3(0, 0, 0);//光らせる
			break;
		}

		//ロシアンルーレット
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