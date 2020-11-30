#pragma once
#include <DirectXMath.h>


class View
{
private:
protected:
	DirectX::XMFLOAT3  Pos, Target, Up;
	DirectX::XMFLOAT4X4 ViewMatrix;
	DirectX::XMFLOAT4X4 ProjectionMatrix;
	float Fov;
	float Aspect;
	float Near;
	float Far;
	float Width;
	float Height;
	bool bView;

public:
	View();
	virtual ~View();
	void Set(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up);
	void SetProjection(float fov, float aspect, float min, float max);
	void SetOrtho(float width, float height, float min, float max);

	void Activate();
	DirectX::XMMATRIX GetView() { return XMLoadFloat4x4(&ViewMatrix); }
	DirectX::XMMATRIX GetProjection() { return XMLoadFloat4x4(&ProjectionMatrix); }
	DirectX::XMFLOAT3 GetPos() { return Pos; }

};
