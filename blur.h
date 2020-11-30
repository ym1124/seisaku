#pragma once
//#include "frame_buffer.h"
//#include "shader.h"

class frame_buffer;
class shader;
class Texture;
class sprite;
struct ConstantBufferForGaussianBlur;

//variance��0�ȊO�ɂ��邱��
class blur
{
protected:
	float weight_array[8];
	std::unique_ptr<shader> blur_shader_horizontal;
	std::unique_ptr<shader> blur_shader_vertical;

	std::unique_ptr<Texture> scene_tex;
	std::unique_ptr<Texture> blur_tex;
	std::unique_ptr<Texture> result_tex;

	std::unique_ptr<sprite> spr;
public:
	blur();
	~blur() {};

	//settter
	void setSceneTexBegine(frame_buffer *f_buffer);
	void setSceneTexEnd(frame_buffer *f_buffer);
	void setSceneTex(Texture* tex);

	//getter
	Texture* getSceneTexture() { return scene_tex.get(); }
	Texture* getResultTexture() { return result_tex.get(); }

	//bloom_flg:�u���[���ɗ��p���邩//result_flg:���ʂ�ۑ�����//tex==nullptr�Ȃ�scene_tex�ɕۑ����ꂽ���̂𗘗p����
	void render(frame_buffer *f_buffer, float variance,Texture* tex = nullptr, bool bloom_flg = false , bool result_tex = false, int sampler_type = 1/*LINER_BORODER*/, int blend_type = 1 ,int weight_num = 8);
	void render(frame_buffer *f_buffer, float variance, Texture_d* tex, bool result_tex = false, int weight_num = 8);
};