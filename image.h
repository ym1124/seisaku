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

	//全ピクセルをkで割る
	void divide(float k);

	//ガンマ補正

	//ppm画像出力

	void ppm_output(const std::string &filename);
};