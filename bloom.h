#pragma once
//#include <memory>
#include "blur.h"

class shader;
class Texture;
class frame_buffer;
class sprite;
struct ConstantBufferForGaussianBlur;

class bloom : public blur
{
protected:
	std::unique_ptr<shader> bright_shader;

	std::unique_ptr<Texture> bright_tex;
public:
	bloom();
	~bloom() {};

	void render(frame_buffer *f_buffer, float threshold, float variance, int weight_num = 8);
};

//class bloom
//{
//protected:
//	float weight_array[8];
//
//	std::unique_ptr<shader> bright_shader;
//	std::unique_ptr<shader> blur_shader_horizontal;
//	std::unique_ptr<shader> blur_shader_vertical;
//
//	std::unique_ptr<Texture> scene_tex;
//	std::unique_ptr<Texture> blur_tex;
//	std::unique_ptr<Texture> bright_tex;
//
//	std::unique_ptr<sprite> spr;
//public:
//	bloom();
//	~bloom() {};
//
//	//settter
//	void setSceneTexBegine(frame_buffer *f_buffer);
//	void setSceneTexEnd(frame_buffer *f_buffer);
//
//	//getter
//	Texture* getSceneTexture() { return scene_tex.get(); }
//
//	void render(frame_buffer *f_buffer, float threshold, float variance, int weight_num = 8);
//};

class kawase_bloom : public bloom
{
public:
	const static int TEXES_NUM = 10;
	kawase_bloom();
	~kawase_bloom() {};
private:
	std::unique_ptr<Texture> texes[TEXES_NUM];
	std::unique_ptr<Texture> temp[TEXES_NUM];
public:
	void createDownSampleTextures(std::unique_ptr<Texture> texes[], std::unique_ptr<Texture> temp[], int size);
	void downSamplingAndBlur(std::unique_ptr<Texture> texes[], std::unique_ptr<Texture> temp[], frame_buffer* f_buffer, ConstantBufferForGaussianBlur cbgb, int size , float threshold,float variance);
	//2d_shader‚ÍŠO•”‚©‚ç‚à‚ç‚¤(‚Ç‚¤‚¹Žg‚¢‚Ü‚í‚·‚Ì‚Å)
	void render(frame_buffer *f_buffer, shader* _2d_shader, float threshold, float variance, int down_sampling_num,int weight_num = 8);
};