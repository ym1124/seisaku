#include "weapon.h"

void weapon_manager::init()
{
	weapon.clear();
}

void weapon_manager::update()
{
	for (int i = 0; i < (int)weapon.size(); i++) {
		if (!weapon[i]->getIsActivate()) continue;

		weapon[i]->update();
	}
}

void weapon_manager::render(std::unique_ptr<shader_ex>& shader, MainCamera* m_camera, DirectX::XMFLOAT4 light_dir)
{
	for (int i = 0; i < (int)weapon.size(); i++) {
		if (!weapon[i]->getIsActivate()) continue;

		weapon[i]->render(shader, m_camera, light_dir);
	}
}

void weapon_manager::release()
{
	for (int i = 0; i < (int)weapon.size(); i++) {
		delete weapon[i];
		weapon[i] = nullptr;
	}

	weapon.clear();
}
