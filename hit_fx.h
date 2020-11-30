#pragma once
#include <memory>
#include <d3d11.h>
#include <wrl.h>
#include <DirectXMath.h>
#include <string>
#include "camera.h"
#include "blend.h"
#include "billboard.h"

class hit_fx : public billboard_obj
{
private:
	float life_timer;
	float life_max;
	bool exist;
public:
	hit_fx(ID3D11Device *, std::shared_ptr<billboard> bb, float life_timer);
	~hit_fx();

	//‰ñ“]‚Ì’†SÀ•W
	void update(camera* m_camera);
	void render(ID3D11DeviceContext* context, const DirectX::XMFLOAT4X4& world, camera *camera, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color);
	void setExist(bool exist) { this->exist = exist; }
	void setLifeTimer(float life_timer) { this->life_timer = life_timer; }
	float getLifeTimer() { return life_timer; }
};

class hit_fx_manager
{
public:
	hit_fx *effects[10];
	std::shared_ptr<billboard> bb;
	hit_fx_manager(ID3D11Device* device, std::shared_ptr<billboard> bb, DirectX::XMFLOAT3 pos, float life_timer);
	~hit_fx_manager();
	void update(ID3D11Device *, camera* m_camera, DirectX::XMFLOAT3, float life_timer, int pos_rng = 0.0f);
	void render(ID3D11DeviceContext* context, camera *camera, const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color);
	void setBillboard(std::shared_ptr<billboard> bb) { this->bb = bb; }
};
