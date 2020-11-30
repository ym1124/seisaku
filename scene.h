#pragma once

class scene
{
public:
	scene() {}
	virtual ~scene() {}
	virtual void Initialize() = 0;
	virtual void Update(float elapsedTime) = 0;
	virtual void Render(float elapsedTime) = 0;
	DirectX::XMFLOAT4X4 LightViewProjection;

private:
	friend class scene_load;
	friend class scene_manager;
	bool initialized = false;//‰Šú‰»Š®—¹ƒtƒ‰ƒO
};