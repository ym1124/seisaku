#pragma once
#include <DirectXMath.h>
#include <fstream>
#include <iostream>
#include <cmath>
#include <string>

class image
{
public:
	int width;
	int height;

	DirectX::XMFLOAT3* data;

	image(int w, int h);

	~image();

	DirectX::XMFLOAT3 getPixel(int i, int j);

	DirectX::XMFLOAT3 setPixel(int i, int j, const DirectX::XMFLOAT3 &c);

	void addPixel(int i, int j, DirectX::XMFLOAT3 &c);

	//�S�s�N�Z����k�Ŋ���
	void divide(float k);

	//�K���}�␳

	//ppm�摜�o��

	void ppm_output(const std::string &filename);
};