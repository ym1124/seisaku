#pragma once

#include <DirectXMath.h>
#include <iostream>
#include <random>


//XMFLOAT3ìØém
DirectX::XMFLOAT3 operator+(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);

DirectX::XMFLOAT3& operator+= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3& operator-= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3& operator*= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3& operator/= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);

//XMFLOAT3Ç∆ÉXÉJÉâÅ[
DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 &a, float b);
DirectX::XMFLOAT3 operator*(float a, DirectX::XMFLOAT3 & b);
DirectX::XMFLOAT3 operator/(DirectX::XMFLOAT3 &a, float b);
DirectX::XMFLOAT3 operator/(float a, DirectX::XMFLOAT3 & b);

DirectX::XMFLOAT3& operator/=(float a, DirectX::XMFLOAT3 & b);
DirectX::XMFLOAT3& operator/=(DirectX::XMFLOAT3& a, float b);

//ÇªÇÃëº
DirectX::XMFLOAT3& operator-(DirectX::XMFLOAT3 &a);
