#pragma once
#include <DirectXMath.h>
#include <iostream>

class Float3 : public DirectX::XMFLOAT3
{
public:
	//float x;
	//float y;
	//float z;

	Float3() = default;

	Float3(const Float3&) = default;
	Float3& operator=(const Float3&) = default;

	Float3(Float3&&) = default;
	Float3& operator=(Float3&&) = default;
	Float3(float x, float y, float z) : DirectX::XMFLOAT3(x, y, z) {}
	Float3(DirectX::XMVECTOR v) { DirectX::XMStoreFloat3(this, v); }

	const Float3& operator+=(const Float3& rhs)
	{
		DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
		DirectX::XMVECTOR rh = DirectX::XMLoadFloat3(&rhs);
		v = DirectX::XMVectorAdd(v, rh);
		DirectX::XMStoreFloat3(this, v);

		return *this;
	}

	Float3 operator+(const Float3& rhs)const
	{
		DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
		DirectX::XMVECTOR rh = DirectX::XMLoadFloat3(&rhs);
		v = DirectX::XMVectorAdd(v, rh);

		Float3 re;
		DirectX::XMStoreFloat3(&re, v);

		return re;
	}

	const Float3& operator-=(const Float3& rhs)
	{
		DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
		DirectX::XMVECTOR rh = DirectX::XMLoadFloat3(&rhs);
		v = DirectX::XMVectorSubtract(v, rh);
		DirectX::XMStoreFloat3(this, v);
		return *this;
	}

	Float3 operator-(const Float3& rhs)const
	{
		DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
		DirectX::XMVECTOR rh = DirectX::XMLoadFloat3(&rhs);
		v = DirectX::XMVectorSubtract(v, rh);

		Float3 re;
		DirectX::XMStoreFloat3(&re, v);

		return re;
	}

	const Float3& operator*=(const float& rhs)
	{
		DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
		DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, 0.f);;
		v = DirectX::XMVectorMultiply(v, rh);
		DirectX::XMStoreFloat3(this, v);
		return *this;
	}

	Float3 operator*(const float& rhs)const
	{
		DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
		DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, 0.f);;
		v = DirectX::XMVectorMultiply(v, rh);
		Float3 re;
		DirectX::XMStoreFloat3(&re, v);

		return re;
	}

	Float3 operator*(const Float3& rhs)const
	{
		DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
		DirectX::XMVECTOR rh = DirectX::XMLoadFloat3(&rhs);
		v = DirectX::XMVectorMultiply(v, rh);
		Float3 re;
		DirectX::XMStoreFloat3(&re, v);

		return re;
	}

	const Float3& operator/=(const float& rhs)
	{
		DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
		DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, 1.f);;
		v = DirectX::XMVectorDivide(v, rh);
		DirectX::XMStoreFloat3(this, v);
		return *this;
	}

	Float3 operator/(const float& rhs)const
	{
		DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
		DirectX::XMVECTOR rh = DirectX::XMVectorSet(rhs, rhs, rhs, 1.f);;
		v = DirectX::XMVectorDivide(v, rh);
		Float3 re;
		DirectX::XMStoreFloat3(&re, v);

		return re;
	}

	Float3 operator/(const Float3& rhs)const
	{
		DirectX::XMVECTOR v = DirectX::XMLoadFloat3(this);
		DirectX::XMVECTOR rh = DirectX::XMLoadFloat3(&rhs);
		v = DirectX::XMVectorDivide(v, rh);
		Float3 re;
		DirectX::XMStoreFloat3(&re, v);

		return re;
	}


	//bool operator==(const Float3& rhs)const
	//{
	//	return (isEqual(x, rhs.x) && isEqual(y, rhs.y) && isEqual(z, rhs.z));
	//}
	//bool operator==(const float& rhs)const
	//{
	//	return (isEqual(x, rhs) && isEqual(y, rhs) && isEqual(z, rhs));
	//}

	bool operator!=(const Float3& rhs)const
	{
		return (x != rhs.x) || (y != rhs.y) || (z != rhs.z);
	}

	bool operator!=(const float& rhs)const
	{
		return (x != rhs) || (y != rhs) || (z != rhs);
	}

	DirectX::XMVECTOR ConvertToXMVECTOR()
	{
		return DirectX::XMLoadFloat3(this);
	}


	inline float Length() const
	{
		return DirectX::XMVector3Length(DirectX::XMLoadFloat3(this)).m128_f32[0];
	}

	inline float LengthSq() const
	{
		return DirectX::XMVector3LengthSq(DirectX::XMLoadFloat3(this)).m128_f32[0];
	}

	inline float Dot(const Float3& f) const
	{
		return DirectX::XMVector3Dot(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&f)).m128_f32[0];
	}

	inline Float3 Cross(const Float3& f) const
	{
		return DirectX::XMVector3Cross(DirectX::XMLoadFloat3(this), DirectX::XMLoadFloat3(&f));
	}

	inline void Normalize()
	{
		float vector_length = this->Length();

		if (vector_length > FLT_EPSILON)
		{
			*this /= vector_length;
		}
	}

	inline Float3 GetNormalize() const
	{
		Float3 vec = *this;

		float vector_length = vec.Length();

		if (vector_length > FLT_EPSILON)
		{
			vec /= vector_length;
		}

		return vec;
	}

	inline Float3 Reverse() const
	{
		return Float3(-this->x, -this->y, -this->z);
	}

	inline void Limit(float max)
	{
		if (this->LengthSq() > powf(max, 2.f))
		{
			this->Normalize();

			*this *= max;
		}
	}
};