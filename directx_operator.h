#pragma once

#include <DirectXMath.h>
#include <iostream>
#include <random>


//XMFLOAT3同士
DirectX::XMFLOAT3 operator+(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);

DirectX::XMFLOAT3& operator+= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3& operator-= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3& operator*= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3& operator/= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);

//XMFLOAT3とスカラー
DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 &a, float b);
DirectX::XMFLOAT3 operator*(float a, DirectX::XMFLOAT3 & b);
DirectX::XMFLOAT3 operator/(DirectX::XMFLOAT3 &a, float b);
DirectX::XMFLOAT3 operator/(float a, DirectX::XMFLOAT3 & b);

DirectX::XMFLOAT3& operator/=(float a, DirectX::XMFLOAT3 & b);
DirectX::XMFLOAT3& operator/=(DirectX::XMFLOAT3& a, float b);

//その他
DirectX::XMFLOAT3& operator-(DirectX::XMFLOAT3 &a);
