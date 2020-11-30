#include "hit_fx.h"

hit_fx::hit_fx(ID3D11Device* device, std::shared_ptr<billboard> bb, float life_timer) :billboard_obj(bb)
{
	this->life_timer = life_timer;
	life_max = life_timer;
	exist = true;
}

hit_fx::~hit_fx() {}

void hit_fx::update(Camera* m_camera)
{
	if (life_timer > 0)
		life_timer--;
	else exist = false;
	billboard_obj::update(m_camera);
}

void hit_fx::render(ID3D11DeviceContext* context,
	const DirectX::XMFLOAT4X4& world, Camera *camera,
	const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color
	)
{
	DirectX::XMFLOAT4 mtl_color = material_color;
	mtl_color.w *= life_timer / life_max;
	billboard_obj::render(context, camera, light_direction, mtl_color);
}

hit_fx_manager::hit_fx_manager(ID3D11Device* device, std::shared_ptr<billboard> bb, DirectX::XMFLOAT3 pos, float life_timer)
{
	this->bb = bb;
	for (auto &it : effects)
	{
		it = new hit_fx(device, this->bb, life_timer);
		it->setPosition(DirectX::XMFLOAT3(pos));
		it->setAngle(DirectX::XMFLOAT3(0, 0, 0));
		it->setScale(DirectX::XMFLOAT3(1, 1, 1));
		it->setExist(true);
	}
}

hit_fx_manager::~hit_fx_manager()
{
	for (auto &it : effects)
	{
		delete it;
		it = nullptr;
	}
}

void hit_fx_manager::update(ID3D11Device* device, Camera *m_camera, DirectX::XMFLOAT3 pos, float life_timer, int pos_rng)
{
	int temp, temp2;
	for (auto &it : effects)
	{
		it->update(m_camera);//
		if (it->getLifeTimer() <= 0)
		{
			if (pos_rng != 0)
				temp = rand() % pos_rng;
			temp2 = rand() % 360;
			if (it != nullptr)
				delete it;
			it = new hit_fx(device, bb, life_timer);
			it->setPosition(DirectX::XMFLOAT3(pos.x + temp, pos.y + temp, pos.z + temp));
			it->setAngle(DirectX::XMFLOAT3(0.0f, 0.0f, temp2));
			it->setScale(DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
			it->setExist(true);
		}
	}
}

void hit_fx_manager::render(ID3D11DeviceContext* context, Camera *camera,
	const DirectX::XMFLOAT4& light_direction, const DirectX::XMFLOAT4& material_color)
{
	for (auto &it : effects)
	{
		if (it != nullptr)
		{
			if (it->getLifeTimer() >= 0)
			{
				it->render(context, it->world, camera, light_direction, material_color);
			}
		}
	}
}