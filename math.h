#pragma once

#include <DirectXMath.h>

int IntRandom(const int& start, const int& end);
float FloatRandom(const float& start, const float& end);

int clamp(int x, int low, int high);
float clamp(float x, float low, float high);

//x==highでlowになる
int wrap(int x, int low, int high);
float wrap(float x, float low, float high);

//ratio(0.0f～1.0f)の間の値にする
void lerp(float& start, const float& end, const float& ratio);

//線分と線分の当たり判定
bool				LineVsLine(const DirectX::XMFLOAT2& vec1point1, const DirectX::XMFLOAT2& vec1point2, const DirectX::XMFLOAT2& vec2point1, const DirectX::XMFLOAT2& vec2point2, const bool isOn = false);

//矩形と矩形の当たり判定
bool	HitBoxCheak(const DirectX::XMFLOAT2& min1, const DirectX::XMFLOAT2& max1,
	const DirectX::XMFLOAT2& min2, const DirectX::XMFLOAT2& max2);

//min(左下手前)、max(右上奥)
bool	HitCubeCheak(const DirectX::XMFLOAT3& min1, const DirectX::XMFLOAT3& max1,
	const DirectX::XMFLOAT3& min2, const DirectX::XMFLOAT3& max2);


void WorldToScreen(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT3& worldPosition, DirectX::XMFLOAT3* screenPosition);

bool InScreenCheak(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT3& worldPosition, const DirectX::XMFLOAT3& size);


///////////////////////////////////////////////////////////////////////////////////////////////////
//												VECTOR2											//
///////////////////////////////////////////////////////////////////////////////////////////////////

DirectX::XMFLOAT2	Vec2Add(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2);
DirectX::XMFLOAT2	Vec2Subtract(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2);
DirectX::XMFLOAT2	Vec2Multiply(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2);
DirectX::XMFLOAT2	Vec2Div(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2);

DirectX::XMFLOAT2	Vec2Multiply(const DirectX::XMFLOAT2& vec, const float& scalar);
DirectX::XMFLOAT2	Vec2Div(const DirectX::XMFLOAT2& vec, const float& scalar);

DirectX::XMFLOAT2	Vec2Normalize(const DirectX::XMFLOAT2& vec);
float				Vec2Length(const DirectX::XMFLOAT2& vec);
float				Vec2Dot(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2);
float				Vec2Cross(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2);

DirectX::XMFLOAT2	Vec2Projection(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2);
float				Vec2ProjectionLength(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2);

float				Vec2Distance(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2);

///////////////////////////////////////////////////////////////////////////////////////////////////
//												VECTOR3											//
///////////////////////////////////////////////////////////////////////////////////////////////////

DirectX::XMFLOAT3	Vec3Add(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2);
DirectX::XMFLOAT3	Vec3Subtract(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2);
DirectX::XMFLOAT3	Vec3Multiply(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2);
DirectX::XMFLOAT3	Vec3Div(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2);

DirectX::XMFLOAT3	Vec3Multiply(const DirectX::XMFLOAT3& vec, const float& scalar);
DirectX::XMFLOAT3	Vec3Div(const DirectX::XMFLOAT3& vec, const float& scalar);

DirectX::XMFLOAT3	Vec3Normalize(const DirectX::XMFLOAT3& vec);
float				Vec3Length(const DirectX::XMFLOAT3& vec);
float				Vec3Dot(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2);
DirectX::XMFLOAT3	Vec3Cross(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2);

float				Vec3Distance(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2);

DirectX::XMFLOAT3	Vec3Projection(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2);
float				Vec3ProjectionLength(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2);

