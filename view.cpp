
//#include <d3d11.h>
//#include "view.h"

View::View()
{
	ZeroMemory(this, 0);
	ViewMatrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
	ProjectionMatrix = { 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 };
}
View::~View()
{
}

void View::Set(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 up)
{
	Pos = pos;
	Target = target;
	Up = up;
}

void View::SetProjection(FLOAT fov, FLOAT aspect, FLOAT min, FLOAT max)
{
	bView = true;
	Fov = fov;
	Aspect = aspect;
	Near = min;
	Far = max;

}

void View::SetOrtho(FLOAT width, FLOAT height, FLOAT min, FLOAT max)
{
	bView = false;
	Width = width;
	Height = height;
	Near = min;
	Far = max;

}


void View::Activate()
{
	DirectX::XMVECTOR vPos, vTarget, vUp;
	vPos = DirectX::XMVectorSet(Pos.x, Pos.y, Pos.z, 0);
	vTarget = DirectX::XMVectorSet(Target.x, Target.y, Target.z, 0);
	vUp = DirectX::XMVectorSet(Up.x, Up.y, Up.z, 0);


	DirectX::XMMATRIX vm = DirectX::XMMatrixLookAtLH(vPos, vTarget, vUp);
	XMStoreFloat4x4(&ViewMatrix, vm);


	DirectX::XMMATRIX pm = DirectX::XMMatrixIdentity();
	if (bView) {
		pm = DirectX::XMMatrixPerspectiveFovLH(Fov, Aspect, Near, Far);
		XMStoreFloat4x4(&ProjectionMatrix, pm);
	}
	else {
		pm = DirectX::XMMatrixOrthographicLH(Width, Height, Near, Far);
		XMStoreFloat4x4(&ProjectionMatrix, pm);
	}
}
