#include "light.h"
//#include <d3d11.h>

DirectX::XMFLOAT4 Light::LightDir(1, 1, 1, 1);
DirectX::XMFLOAT4 Light::DirLightColor(1, 1, 1, 1);
DirectX::XMFLOAT4 Light::Ambient(1, 1, 1, 1);
POINTLIGHT Light::PointLight[POINTMAX];
SPOTLIGHT Light::SpotLight[SPOTMAX];

void Light::Init()
{
	ZeroMemory(PointLight, sizeof(POINTLIGHT)*POINTMAX);
	ZeroMemory(SpotLight, sizeof(SPOTLIGHT)*SPOTMAX);

}


void Light::SetDirLight(DirectX::XMFLOAT3 dir, DirectX::XMFLOAT3 color)
{
	float d = sqrtf(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);

	if (d > 0) { dir.x /= d, dir.y /= d, dir.z /= d; }
	LightDir = DirectX::XMFLOAT4(dir.x, dir.y, dir.z, 0);
	DirLightColor = DirectX::XMFLOAT4(color.x, color.y, color.z, 1);
}


void Light::SetAmbient(DirectX::XMFLOAT3 amb)
{
	Ambient = { amb.x,amb.y,amb.z,0 };

}
void Light::SetPointLight(int index, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 color, float range)
{
	if (index < 0) return;
	if (index >= POINTMAX)return;
	PointLight[index].index = (float)index;
	PointLight[index].range = range;
	PointLight[index].type = 1.0f;
	PointLight[index].dumy = 0.0f;
	PointLight[index].pos = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 0);
	PointLight[index].color = DirectX::XMFLOAT4(color.x, color.y, color.z, 0);

}
void Light::SetSpotLight(int index, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 dir,
	float range, float near_area, float far_area)
{
	if (index < 0) return;
	if (index >= SPOTMAX)return;
	float d = sqrtf(dir.x*dir.x + dir.y*dir.y + dir.z*dir.z);
	if (d > 0) {
		dir.x /= d; dir.y /= d; dir.z /= d;
	}

	SpotLight[index].index = (float)index;
	SpotLight[index].range = range;
	SpotLight[index].type = 1.0f;
	SpotLight[index].near_area = near_area;
	SpotLight[index].far_area = far_area;
	SpotLight[index].dumy0 = 0.0f;
	SpotLight[index].dumy1 = 0.0f;
	SpotLight[index].dumy2 = 0.0f;

	SpotLight[index].pos = DirectX::XMFLOAT4(pos.x, pos.y, pos.z, 0);
	SpotLight[index].color = DirectX::XMFLOAT4(color.x, color.y, color.z, 0);
	SpotLight[index].dir = DirectX::XMFLOAT4(dir.x, dir.y, dir.z, 0);
}

void Light::ResetLight()
{
	Light::SetAmbient(DirectX::XMFLOAT3(1, 1, 1));
	Light::SetDirLight(DirectX::XMFLOAT3(0.6, 0.6, 0.6), DirectX::XMFLOAT3(1, 1, 1));
}

