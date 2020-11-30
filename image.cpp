#pragma once
#include "image.h"
#include "util.h"
#include "directx_operator.h"

image::image(int w, int h)
{
	width = w;
	height = h;
	data = new DirectX::XMFLOAT3[width*height];

	//0クリア
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			data[i + width*j] = DirectX::XMFLOAT3(0, 0, 0);
		}
	}
};

image::~image()
{
	delete[] data;
};

DirectX::XMFLOAT3 image::getPixel(int i, int j)
{
	return data[i + width*j];
};

DirectX::XMFLOAT3 image::setPixel(int i, int j, const DirectX::XMFLOAT3 &c)
{
	return data[i + width*j] = c;
};

void image::addPixel(int i, int j, DirectX::XMFLOAT3 &c)
{
	DirectX::XMFLOAT3 temp = getPixel(i,j);
	setPixel(i, j, c + temp);
};

//全ピクセルをkで割る
void image::divide(float k)
{
	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			DirectX::XMFLOAT3 col = getPixel(i,j) / k;
			this->setPixel(i, j, col);
		}
	}
};

//ガンマ補正

//ppm画像出力

void image::ppm_output(const std::string &filename)
{
	std::ofstream file(filename);
	file << "P3" << std::endl;
	file << width << " " << height << std::endl;
	file << "255" << std::endl;
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			DirectX::XMFLOAT3 c = this->getPixel(i, j);
			int r = clamp(int(255 * c.x), 0, 255);
			int g = clamp(int(255 * c.y), 0, 255);
			int b = clamp(int(255 * c.z), 0, 255);
			file << r << " " << g << " " << b << std::endl;
		}
	}
	file.close();
};