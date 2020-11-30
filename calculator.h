#pragma once

#include "directx_operator.h"
#include <cmath>

//���̑�
float dot(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
DirectX::XMFLOAT3 cross(DirectX::XMFLOAT3 &a, DirectX::XMFLOAT3 &b);
float length(DirectX::XMFLOAT3 &a);
float length2(DirectX::XMFLOAT3 &a);
DirectX::XMFLOAT3 normalize(DirectX::XMFLOAT3 &a);
void orthoNormalBasis(DirectX::XMFLOAT3 &v1, DirectX::XMFLOAT3 &v2, DirectX::XMFLOAT3 &v3);

//��l�����̐���
float rnd();

//��ϕ��֐��̐���
float f(float x);

//�m�����x�֐�
float p(float x);

//�ݐϕ��z�֐��̋t�֐�
float pInv(float x);




////////////////////////////////////////????
//���[���h���烍�[�J���ɕϊ�
DirectX::XMFLOAT3 localToWorld(DirectX::XMFLOAT3& v, DirectX::XMFLOAT3& s, DirectX::XMFLOAT3& n, DirectX::XMFLOAT3& t);

//���[�J�����烏�[���h
DirectX::XMFLOAT3 worldToLocal(DirectX::XMFLOAT3& v, DirectX::XMFLOAT3& s, DirectX::XMFLOAT3& n, DirectX::XMFLOAT3& t);