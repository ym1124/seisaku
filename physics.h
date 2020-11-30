#pragma once

#include <DirectXMath.h>

class physics
{
private:
	const float gravity = 9.80665f;//unit::m/s^2
	 //enable flg(false時はInfluence関数の中身を実行しない)
	bool gravity_enable;
	bool friction_enable;

protected:
	float weight;//質量 unit::kg
	float max_static_friction;//最大静止摩擦力 unit::N
	float dynamic_friction;//動摩擦力 unit::N
	float coefficient_of_restitution;//反発係数
	DirectX::XMFLOAT3 position;//world_position
	DirectX::XMFLOAT3 velocity;//速度 unit::m/s
	DirectX::XMFLOAT3 accelerator;//加速度 unit::m/s^2

private://PhysicalUpdateで実行しているので呼ばなくて大丈夫
	inline void influenceOfGravity(const float _delta_time)
	{
		//フラグがfalse時は重力の影響を受けない
		if (gravity_enable == false)
			return;

		//重力による運動なので質量には影響を受けない
		velocity.y -= gravity * _delta_time;
	}
	inline void positionUpdate(const float _delta_time)
	{
		position.x += velocity.x * _delta_time;
		position.y += velocity.y * _delta_time;
		position.z += velocity.z * _delta_time;
	}

protected:

public:
	inline void physicalUpdate(const float _delta_time)
	{
		influenceOfGravity(_delta_time);
		positionUpdate(_delta_time);
	}

	//他の物体と接しているときに使用する(接している物体の最大静止摩擦力と動摩擦力を引っ張ってくる)
	inline void influenceOfFriction(const float _delta_time, const float _max_static_friction, const float _dynamic_friction)
	{
		//フラグがfalse時は摩擦力の影響を受けない
		if (friction_enable == false)
			return;

		//水平方向に速度があるかの判別(摩擦なので水平方向のみでいけるはず)
		float velocity_h = sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);

		//速度があるとき(移動しているので動摩擦力が作用する)
		if (velocity_h > .0f)
		{
			//水平方向の力(加速度と質量をかけることで力に変換可能にする)
			float power = sqrtf(accelerator.x * accelerator.x + accelerator.z * accelerator.z) * weight;

			//接している物体(地面など)の動摩擦力より力が等しくない(加速、原則する)場合
			if (power != _dynamic_friction)
			{
				//加速量(加速度)の算出
				float length = ((power - _dynamic_friction) / power) / weight;

				//速度に加算する
				velocity.x += (accelerator.x * _delta_time) * length;
				velocity.z += (accelerator.z * _delta_time) * length;
			}

			//等しい場合(等速運動)は加速も減速もしないため何も記述しない
		}

		//速度がないとき(移動していないので静止摩擦力が作用する)
		else if (velocity_h == .0f)
		{
			//水平方向の加速度に質量をかけることによって力(N)に変換する
			float power = sqrtf(accelerator.x * accelerator.x + accelerator.z * accelerator.z) * weight;

			//接している物体(地面など)の静止摩擦力より力が大きい場合
			if (power > _max_static_friction)
			{
				//ベクトルの長さ(割合)を算出
				float length = ((power - _max_static_friction) / power) / weight;

				//速度に加算する
				velocity.x += (accelerator.x * length) * _delta_time;
				velocity.z += (accelerator.z * length) * _delta_time;
			}

			//小さい場合と等しい場合は何も起きないので記述しない
		}

		//速度のベクトルの長さがマイナスはおかしいのでエラー処理(debug時のみでいいと思ったのでassertを使用)
		else
		{
			assert(0 && "the value of this variable is an improbable value");
		}
	}

	//他の物体に衝突した際に使用する(positionの修正も関数内で処理済み)
	//物体が水平または垂直な場合にのみ使用(衝突した物体の反発力と衝突位置、衝突方向を渡す)
	inline void verticalRebound(const float _coefficient_of_restitution, const DirectX::XMFLOAT3 _pos, const bool _horizontal_direction)
	{
		//反発係数は0<=e<=1なので範囲チェック
		assert((.0f <= _coefficient_of_restitution && _coefficient_of_restitution <= 1.0f) && "the value of this variable is an improbable value");

		//とりあえずpositionの修正
		position = _pos;

		//水平方向の跳ね返り
		if (_horizontal_direction)
		{
			//衝突した対象物の反発力(ぶつかった際の反発係数)を速度にかける
			velocity.x *= _coefficient_of_restitution;
			velocity.z *= _coefficient_of_restitution;

			//跳ね返ったので次のフレームからの加速度は逆向きになる
			accelerator.x *= -(_coefficient_of_restitution);
			accelerator.z *= -(_coefficient_of_restitution);
		}

		//垂直方向の跳ね返り
		else
		{
			//衝突した対象物の反発力(ぶつかった際の反発係数)を速度にかける
			velocity.y *= _coefficient_of_restitution;
			//跳ね返ったので次のフレームからの加速度は逆向きになる
			accelerator.y *= -(_coefficient_of_restitution);
		}
	}
	//垂直方向以外での衝突時に使用(衝突した物体の反発力と衝突位置(world座標)、衝突した面の向いている方向(法線)を渡す)
	//めり込んでいるときを想定して組んでる
	inline void nonVerticalRebound(const float _coefficient_of_restitution, const DirectX::XMFLOAT3 _pos, const DirectX::XMFLOAT3 _vector)
	{
		//反発係数は0<=e<=1なので範囲チェック
		assert((.0f <= _coefficient_of_restitution && _coefficient_of_restitution <= 1.0f) && "the value of this variable is an improbable value");

		//進行方向と衝突面の向きベクトルで当たってるか判定(内積計算)
		//TODO 変数名変えたい！
		DirectX::XMVECTOR norm_vec = DirectX::XMLoadFloat3(&_vector);
		norm_vec = DirectX::XMVector3Normalize(norm_vec);
		DirectX::XMVECTOR norm_velo = XMLoadFloat3(&velocity);
		norm_velo = DirectX::XMVector3Normalize(norm_velo);
		DirectX::XMVECTOR result = DirectX::XMVector3Dot(norm_vec, norm_velo);
		float judge;
		DirectX::XMStoreFloat(&judge, result);

		//表面から斜めに当たっている時
		if (0 < judge && judge < 1.0f)
		{
			//衝突箇所と現在のポジションでベクトルを結ぶ(命名適当になってる)
			DirectX::XMFLOAT3 vec_b = { _pos.x - position.x,_pos.y - position.y,_pos.z - position.z };

			//衝突面の向き(法線)に射影する(なんとなくだけどXMVECTOR使わずやってる)
			//衝突面の向き(法線)を正規化する
			float length_vec = sqrtf(_vector.x * _vector.x + _vector.y * _vector.y + _vector.z * _vector.z);
			DirectX::XMFLOAT3 n_vec = { _vector.x / length_vec,_vector.y / length_vec,_vector.z / length_vec };

			//ベクトル2つを内積計算する
			float dot = vec_b.x * _vector.x + vec_b.y * _vector.y + vec_b.z * _vector.z;
			float value = dot / length_vec;
			DirectX::XMFLOAT3 projection_vec = { n_vec.x * value,n_vec.y * value,n_vec.z * value };

			//現在のポジションに射影ベクトルの2倍を加算し移動させて位置の修正をする
			position.x += projection_vec.x * 2;
			position.y += projection_vec.y * 2;
			position.z += projection_vec.z * 2;

			//移動後のポジションと衝突箇所でベクトルを引く(単位ベクトルにする)
			DirectX::XMFLOAT3 reflection_vec = { position.x - _pos.x,position.y - _pos.y,position.z - _pos.z };
			float length_reflection = sqrtf(reflection_vec.x * reflection_vec.x + reflection_vec.y * reflection_vec.y + reflection_vec.z * reflection_vec.z);
			reflection_vec.x /= length_reflection;
			reflection_vec.y /= length_reflection;
			reflection_vec.z /= length_reflection;

			//計算によって求まったベクトルを元々の速度ベクトルの大きさにする
			float length_velo = sqrtf(velocity.x * velocity.x + velocity.y * velocity.y + velocity.z * velocity.z);
			velocity.x = reflection_vec.x * length_velo;
			velocity.y = reflection_vec.y * length_velo;
			velocity.z = reflection_vec.z * length_velo;

			//反発係数を速度にかける
			velocity.x *= -_coefficient_of_restitution;
			velocity.y *= -_coefficient_of_restitution;
			velocity.z *= -_coefficient_of_restitution;
		}

		//表面から垂直に当たっている場合
		else if (judge == 1.0f)
		{
			//反発係数を速度にかける
			velocity.x *= -_coefficient_of_restitution;
			velocity.y *= -_coefficient_of_restitution;
			velocity.z *= -_coefficient_of_restitution;
		}

		//裏面から当たるのはおかしいのでエラー処理(debug時のみでいいと判断したためassert関数を使用)
		else
			assert(0 && "the value of this variable is an improbable value");
	}

	//reset
	inline void resetAllVariable()
	{
		gravity_enable = true;
		friction_enable = true;

		weight = .0f;
		max_static_friction = .0f;
		dynamic_friction = .0f;
		coefficient_of_restitution = .0f;
		position = { .0f,.0f,.0f };
		velocity = { .0f,.0f,.0f };
		accelerator = { .0f,.0f,.0f };
	}
	inline void resetAccelerator() { accelerator = { .0f,.0f,.0f }; }
	inline void resetVelocity() { velocity = { .0f,.0f,.0f }; }

	//getter
	inline float getCoefficientOfRestitution() { return coefficient_of_restitution; }
	inline float getMaxStaticFriction() { return max_static_friction; }
	inline float getDynamicFriction() { return dynamic_friction; }
	//setter

};
