#pragma once

#include <DirectXMath.h>
#include <iostream>
#include <random>


//XMFLOAT3���m
DirectX::XMFLOAT3 operator+(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3 operator-(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);

DirectX::XMFLOAT3& operator+= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3& operator-= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3& operator*= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3& operator/= (DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);

//XMFLOAT3�ƃX�J���[
DirectX::XMFLOAT3 operator*(DirectX::XMFLOAT3 &a, float b);
DirectX::XMFLOAT3 operator*(float a, DirectX::XMFLOAT3 & b);
DirectX::XMFLOAT3 operator/(DirectX::XMFLOAT3 &a, float b);
DirectX::XMFLOAT3 operator/(float a, DirectX::XMFLOAT3 & b);

DirectX::XMFLOAT3& operator/=(float a, DirectX::XMFLOAT3 & b);
DirectX::XMFLOAT3& operator/=(DirectX::XMFLOAT3& a, float b);

//���̑�
DirectX::XMFLOAT3& operator-(DirectX::XMFLOAT3 &a);
