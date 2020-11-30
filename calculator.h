#pragma once

#include "directx_operator.h"
#include <cmath>

//その他
float dot(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3 cross(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
float length(DirectX::XMFLOAT3 &a);
float length2(DirectX::XMFLOAT3 &a);
DirectX::XMFLOAT3 normalize(DirectX::XMFLOAT3 &a);
void orthoNormalBasis(DirectX::XMFLOAT3 &v1, DirectX::XMFLOAT3 &v2, DirectX::XMFLOAT3 &v3);

//一様乱数の生成
float rnd();

//被積分関数の生成
float f(float x);

//確率密度関数
float p(float x);

//累積分布関数の逆関数
float pInv(float x);




////////////////////////////////////////????
//ワールドからローカルに変換
DirectX::XMFLOAT3 localToWorld(DirectX::XMFLOAT3& v, DirectX::XMFLOAT3& s, DirectX::XMFLOAT3& n, DirectX::XMFLOAT3& t);

//ローカルからワールド
DirectX::XMFLOAT3 worldToLocal(DirectX::XMFLOAT3& v, DirectX::XMFLOAT3& s, DirectX::XMFLOAT3& n, DirectX::XMFLOAT3& t);