#pragma once
#include <DirectXMath.h>

struct SPOTLIGHT 
{
	float index;
	float range;//Œõ‚Ì“Í‚­”ÍˆÍ
	float type; //—LŒø‚©–³Œø‚©
	float near_area;
	float far_area;
	float dumy0;
	float dumy1;
	float dumy2;
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT4 color;
	DirectX::XMFLOAT4 dir;
};

struct POINTLIGHT 
{
	float index;
	float range;//Œõ‚Ì“Í‚­”ÍˆÍ
	float type; //—LŒø‚©–³Œø‚©
	float dumy;
	DirectX::XMFLOAT4 pos;
	DirectX::XMFLOAT4 color;
};

struct DIRLIGHT
{
	DirectX::XMFLOAT4 direction;
	DirectX::XMFLOAT4 color;
};

class Light
{
private:
public:
	static const int POINTMAX = 32;
	static const int SPOTMAX = 32;

	static DirectX::XMFLOAT4 LightDir;
	static DirectX::XMFLOAT4 DirLightColor;
	static DirectX::XMFLOAT4 Ambient;
	static POINTLIGHT PointLight[POINTMAX];
	static SPOTLIGHT SpotLight[SPOTMAX];
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;

	static void Init();
	static void SetDirLight(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 color);

	static void SetAmbient(DirectX::XMFLOAT3 amb);
	static void SetPointLight(int index, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 color, float range);
	static void SetSpotLight(int index, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 dir, float range, float near, float far);
	static void ResetLight();

	//void SetView(DirectX::XMFLOAT4X4 view) { this->view = view; };
	//void SetProjection(DirectX::XMFLOAT4X4 projection) { this->projection = projection; };

	//const DirectX::XMFLOAT4X4& GetView() const { return view; };
	//const DirectX::XMFLOAT4X4& GetProjection() const { return projection; };
};
