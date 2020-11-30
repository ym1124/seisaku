#include "math.h"
#include <random>
#include <math.h>

int IntRandom(const int& start, const int& end)
{
	std::random_device rnd;								//非決定的な乱数生成器を生成
	std::mt19937 mt(rnd());								//メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	std::uniform_int_distribution<> random(start, end);	//[start,end]範囲の一様乱数

	return random(mt);
}

float FloatRandom(const float& start, const float& end)
{
	std::random_device rnd;								//非決定的な乱数生成器を生成
	std::mt19937 mt(rnd());								//メルセンヌ・ツイスタの32ビット版、引数は初期シード値
	std::uniform_real_distribution<float> random(start, end);	//[start,end]範囲の一様乱数

	return random(mt);
}

int clamp(int x, int low, int high)
{
	assert(low <= high);
	return std::fmin(std::fmax(x, low), high);
}

float clamp(float x, float low, float high)
{
	assert(low <= high);
	return  std::fmin(std::fmax(x, low), high);
}

int wrap(int x, int low, int high)
{
	assert(low < high);
	const int n = (x - low) % (high - low);
	return (n >= 0) ? (n + low) : (n + high);
}

float wrap(float x, float low, float high)
{
	assert(low < high);
	const float n = std::fmodf(x - low, high - low);
	return (n >= 0.0f) ? (n + low) : (n + high);
}

void lerp(float& start, const float& end, const float& ratio)
{
	start = start - (start - end) * ratio;
}

bool LineVsLine(const DirectX::XMFLOAT2& vec1point1, const DirectX::XMFLOAT2& vec1point2, const DirectX::XMFLOAT2& vec2point1, const DirectX::XMFLOAT2& vec2point2, const bool isOn)
{
	DirectX::XMFLOAT2 AB = Vec2Subtract(vec1point2, vec1point1);
	DirectX::XMFLOAT2 AC = Vec2Subtract(vec2point1, vec1point1);
	DirectX::XMFLOAT2 AD = Vec2Subtract(vec2point2, vec1point1);
	DirectX::XMFLOAT2 CA = Vec2Subtract(vec1point1, vec2point1);
	DirectX::XMFLOAT2 CB = Vec2Subtract(vec1point2, vec2point1);
	DirectX::XMFLOAT2 CD = Vec2Subtract(vec2point2, vec2point1);

	if (AB.x * AB.x + AB.y * AB.y <= 0.0f) return false;
	if (CD.x * CD.x + CD.y * CD.y <= 0.0f) return false;

	if (isOn) {
		if (Vec2Cross(AB, AC) * Vec2Cross(AB, AD) > 0.0f)return false;
		if (Vec2Cross(CD, CA) * Vec2Cross(CD, CB) > 0.0f)return false;
	}
	else {
		if (Vec2Cross(AB, AC) * Vec2Cross(AB, AD) >= 0.0f)return false;
		if (Vec2Cross(CD, CA) * Vec2Cross(CD, CB) >= 0.0f)return false;
	}
	return true;
}

bool HitBoxCheak(const DirectX::XMFLOAT2& min1, const DirectX::XMFLOAT2& max1, const DirectX::XMFLOAT2& min2, const DirectX::XMFLOAT2& max2)
{
	float right1 = max1.x;	float right2 = max2.x;
	float left1 = min1.x;	float left2 = min2.x;
	float top1 = max1.y;	float top2 = max2.y;
	float bottom1 = min1.y;	float bottom2 = min2.y;

	if (right1 < left2)	return false;
	if (left1 > right2)	return false;
	if (top1 < bottom2)	return false;
	if (bottom1 > top2)		return false;

	return true;
}

//min(左下手前)、max(右上奥)
bool HitCubeCheak(const DirectX::XMFLOAT3& min1, const DirectX::XMFLOAT3& max1,
	const DirectX::XMFLOAT3& min2, const DirectX::XMFLOAT3& max2)
{
	float right1 = max1.x;	float right2 = max2.x;
	float left1 = min1.x;	float left2 = min2.x;
	float top1 = max1.y;	float top2 = max2.y;
	float bottom1 = min1.y;	float bottom2 = min2.y;
	float back1 = max1.z;	float back2 = max2.z;
	float front1 = min1.z;	float front2 = min2.z;

	if (right1 < left2)	return false;
	if (left1 > right2)	return false;
	if (top1 < bottom2)	return false;
	if (bottom1 > top2)		return false;
	if (back1 < front2)	return false;
	if (front1 > back2)	return false;

	return true;
}

void WorldToScreen(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT3& worldPosition, DirectX::XMFLOAT3* screenPosition)
{
	DirectX::XMFLOAT4X4 matrix;
	DirectX::XMFLOAT4X4 view_port_matrix;
	view_port_matrix._11 = 1280.0f / 2.0f; view_port_matrix._12 = 0.0f; view_port_matrix._13 = 0.0f; view_port_matrix._14 = 0.0f;
	view_port_matrix._21 = 0.0f; view_port_matrix._22 = -720.0f / 2.0f; view_port_matrix._23 = 0.0f; view_port_matrix._24 = 0.0f;
	view_port_matrix._31 = 0.0f; view_port_matrix._32 = 0.0f; view_port_matrix._33 = 1.0f; view_port_matrix._34 = 0.0f;
	view_port_matrix._41 = 1280.0f / 2.0f; view_port_matrix._42 = 720.0f / 2.0f; view_port_matrix._43 = 0.0f; view_port_matrix._44 = 1.0f;

	DirectX::XMStoreFloat4x4(&matrix, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&projection) * DirectX::XMLoadFloat4x4(&view_port_matrix));

	DirectX::XMFLOAT4 pos;
	pos = {
		worldPosition.x * matrix._11 + worldPosition.y * matrix._21 + worldPosition.z * matrix._31 + 1.0f * matrix._41,
		worldPosition.x * matrix._12 + worldPosition.y * matrix._22 + worldPosition.z * matrix._32 + 1.0f * matrix._42,
		worldPosition.x * matrix._13 + worldPosition.y * matrix._23 + worldPosition.z * matrix._33 + 1.0f * matrix._43,
		worldPosition.x * matrix._14 + worldPosition.y * matrix._24 + worldPosition.z * matrix._34 + 1.0f * matrix._44
	};

	*screenPosition = { pos.x / pos.w,pos.y / pos.w, pos.z / pos.w };
}

bool InScreenCheak(const DirectX::XMFLOAT4X4& view, const DirectX::XMFLOAT4X4& projection, const DirectX::XMFLOAT3& worldPosition, const DirectX::XMFLOAT3& size)
{
	DirectX::XMFLOAT3 cheak_pos[8] =
	{
		{ worldPosition.x - size.x,worldPosition.y - size.y,worldPosition.z + size.z },
		{ worldPosition.x - size.x,worldPosition.y - size.y,worldPosition.z - size.z },
		{ worldPosition.x + size.x,worldPosition.y - size.y,worldPosition.z - size.z },
		{ worldPosition.x + size.x,worldPosition.y - size.y,worldPosition.z + size.z },
		{ worldPosition.x - size.x,worldPosition.y + size.y,worldPosition.z + size.z },
		{ worldPosition.x - size.x,worldPosition.y + size.y,worldPosition.z - size.z },
		{ worldPosition.x + size.x,worldPosition.y + size.y,worldPosition.z - size.z },
		{ worldPosition.x + size.x,worldPosition.y + size.y,worldPosition.z + size.z }
	};

	int cheak_num = 0;

	for (int i = 0; i < 8; i++)
	{
		WorldToScreen(view, projection, cheak_pos[i], &cheak_pos[i]);

		if ((cheak_pos[i].x < 0.0f || cheak_pos[i].x >1280.0f ||
			cheak_pos[i].y < 0.0f || cheak_pos[i].y >720.0f ||
			cheak_pos[i].z > 1.0f))
			cheak_num++;
	}


	if (cheak_num == 8)
		return false;

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//												VECTOR2											//
///////////////////////////////////////////////////////////////////////////////////////////////////

DirectX::XMFLOAT2 Vec2Add(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2)
{
	DirectX::XMFLOAT2 add = {};

	add.x = vec1.x + vec2.x;
	add.y = vec1.y + vec2.y;

	return add;
}

DirectX::XMFLOAT2 Vec2Subtract(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2)
{
	DirectX::XMFLOAT2 subtract = {};

	subtract.x = vec1.x - vec2.x;
	subtract.y = vec1.y - vec2.y;

	return subtract;
}

DirectX::XMFLOAT2 Vec2Multiply(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2)
{
	DirectX::XMFLOAT2 multiply = {};

	multiply.x = vec1.x * vec2.x;
	multiply.y = vec1.y * vec2.y;

	return multiply;
}

DirectX::XMFLOAT2 Vec2Div(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2)
{
	DirectX::XMFLOAT2 div = {};

	if (vec2.x == 0.0f || vec2.y == 0.0f) return DirectX::XMFLOAT2();

	div.x = vec1.x / vec2.x;
	div.y = vec1.y / vec2.y;

	return div;
}

DirectX::XMFLOAT2 Vec2Multiply(const DirectX::XMFLOAT2& vec, const float& scalar)
{
	DirectX::XMFLOAT2 multiply = {};

	multiply.x = vec.x * scalar;
	multiply.y = vec.y * scalar;

	return multiply;
}

DirectX::XMFLOAT2 Vec2Div(const DirectX::XMFLOAT2& vec, const float& scalar)
{
	DirectX::XMFLOAT2 div = {};

	if (scalar == 0.0f) return DirectX::XMFLOAT2();

	div.x = vec.x / scalar;
	div.y = vec.y / scalar;

	return div;
}

DirectX::XMFLOAT2 Vec2Normalize(const DirectX::XMFLOAT2& vec)
{
	DirectX::XMFLOAT2 normalize = vec;

	float dx = normalize.x * normalize.x;
	float dy = normalize.y * normalize.y;

	float length = sqrtf(dx + dy);

	normalize.x /= length;
	normalize.y /= length;

	return normalize;
}

float Vec2Length(const DirectX::XMFLOAT2& vec)
{
	float dx = vec.x * vec.x;
	float dy = vec.y * vec.y;

	return sqrtf(dx + dy);
}

float Vec2Dot(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2)
{
	float dot = vec1.x * vec2.x + vec1.y * vec2.y;
	return dot;
}

float Vec2Cross(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2)
{
	float cross = vec1.x * vec2.y - vec1.y * vec2.x;
	return cross;
}

DirectX::XMFLOAT2 Vec2Projection(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2)
{
	DirectX::XMFLOAT2 projection = {};

	DirectX::XMFLOAT2 normalize = Vec2Normalize(vec2);

	float dot = Vec2Dot(vec1, vec2);

	return Vec2Multiply(normalize, dot);
}

float Vec2ProjectionLength(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2)
{
	DirectX::XMFLOAT2 projection = {};

	DirectX::XMFLOAT2 normalize = Vec2Normalize(vec2);

	float dot = Vec2Dot(vec1, vec2);

	return dot;
}

float Vec2Distance(const DirectX::XMFLOAT2& vec1, const DirectX::XMFLOAT2& vec2)
{
	return Vec2Length(Vec2Subtract(vec1, vec2));
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//												VECTOR3											//
///////////////////////////////////////////////////////////////////////////////////////////////////

DirectX::XMFLOAT3 Vec3Add(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2)
{
	DirectX::XMFLOAT3 add = {};

	add.x = vec1.x + vec2.x;
	add.y = vec1.y + vec2.y;
	add.z = vec1.z + vec2.z;

	return add;
}

DirectX::XMFLOAT3 Vec3Subtract(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2)
{
	DirectX::XMFLOAT3 subtract = {};

	subtract.x = vec1.x - vec2.x;
	subtract.y = vec1.y - vec2.y;
	subtract.z = vec1.z - vec2.z;

	return subtract;
}

DirectX::XMFLOAT3 Vec3Multiply(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2)
{
	DirectX::XMFLOAT3 multiply = {};

	multiply.x = vec1.x * vec2.x;
	multiply.y = vec1.y * vec2.y;
	multiply.z = vec1.z * vec2.z;

	return multiply;
}

DirectX::XMFLOAT3 Vec3Div(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2)
{
	DirectX::XMFLOAT3 div = {};

	if (vec2.x == 0.0f || vec2.y == 0.0f || vec2.z == 0.0f) return DirectX::XMFLOAT3();

	div.x = vec1.x / vec2.x;
	div.y = vec1.y / vec2.y;
	div.z = vec1.z / vec2.z;

	return div;
}

DirectX::XMFLOAT3 Vec3Multiply(const DirectX::XMFLOAT3& vec, const float& scalar)
{
	DirectX::XMFLOAT3 multiply = {};

	multiply.x = vec.x * scalar;
	multiply.y = vec.y * scalar;
	multiply.z = vec.z * scalar;

	return multiply;
}

DirectX::XMFLOAT3 Vec3Div(const DirectX::XMFLOAT3& vec, const float& scalar)
{
	DirectX::XMFLOAT3 div = {};

	if (scalar == 0.0f) return DirectX::XMFLOAT3();

	div.x = vec.x / scalar;
	div.y = vec.y / scalar;
	div.z = vec.z / scalar;

	return div;
}

DirectX::XMFLOAT3 Vec3Normalize(const DirectX::XMFLOAT3& vec)
{
	DirectX::XMFLOAT3 normalize = vec;

	float dx = normalize.x * normalize.x;
	float dy = normalize.y * normalize.y;
	float dz = normalize.z * normalize.z;

	float length = sqrtf(dx + dy + dz);

	normalize.x /= length;
	normalize.y /= length;
	normalize.z /= length;

	return normalize;
}

float Vec3Length(const DirectX::XMFLOAT3& vec)
{
	float dx = vec.x * vec.x;
	float dy = vec.y * vec.y;
	float dz = vec.z * vec.z;

	return sqrtf(dx + dy + dz);
}


float Vec3Dot(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2)
{
	float dot = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
	return dot;
}

DirectX::XMFLOAT3 Vec3Cross(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2)
{
	DirectX::XMFLOAT3 cross = {};

	cross.x = vec1.y * vec2.z - vec1.z * vec2.y;
	cross.y = vec1.z * vec2.x - vec1.x * vec2.z;
	cross.z = vec1.x * vec2.y - vec1.y * vec2.x;

	return cross;
}

float Vec3Distance(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2)
{
	return Vec3Length(Vec3Subtract(vec1, vec2));
}

DirectX::XMFLOAT3 Vec3Projection(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2)
{
	DirectX::XMFLOAT3 projection = {};

	DirectX::XMFLOAT3 normalize = Vec3Normalize(vec2);

	float dot = Vec3Dot(vec1, vec2);

	return Vec3Multiply(normalize, dot);
}

float Vec3ProjectionLength(const DirectX::XMFLOAT3& vec1, const DirectX::XMFLOAT3& vec2)
{
	DirectX::XMFLOAT3 projection = {};

	DirectX::XMFLOAT3 normalize = Vec3Normalize(vec2);

	float dot = Vec3Dot(vec1, vec2);

	return dot;
}
