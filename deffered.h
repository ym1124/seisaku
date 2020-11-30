#pragma once

//class shader;
//class shader_ex;
//class g_buffer;
//class environment_cubemap;
//class Texture;
//class sprite;
//class frame_buffer;

#include "g_buffer.h"
#include "shader.h"
#include "shader_ex.h"
#include "texture.h"
#include "sprite.h"
class frame_buffer;
class environment_cubemap;

class deffered
{
private:
	//g_buffer
	std::unique_ptr<g_buffer> g_buf;
	//shader
	std::unique_ptr<shader> g_buffer_shader,g_buf_normal_map_shader, diffuse_specular_shader, pbr_deffered_shader;
	//shader_ex
	std::unique_ptr<shader_ex> 	gbuffer_ex_shader; 
	//texture
	std::vector<Texture> normal_maps;
	//sprite
	std::unique_ptr<sprite> spr;

public:
	deffered();
	~deffered();

	void render(shader* _2d_shader);//phong�prender//begine-end,ks_kd���K�v
	void render(environment_cubemap* env_cubemap, float metalness, float roughness);//pbr�prender//begine-end��g_buffer�̏��������͕K�v

	//g_buffer�ւ̏�������(color,normal,position,depth)
	void begine(frame_buffer *f_buffer,bool dsv_flg = true);//�m�[�}���}�b�v�v���Ȃ�R�b�`//RTV4�Z�b�g���邾��
	void begine_ex(frame_buffer *f_buffer);
	void begine(frame_buffer *f_buffer, int slot);//�����`�悷��ꍇ�e�X�̕`��O�Ńm�[�}���}�b�v�Z�b�g���Ă��
	void end(frame_buffer *f_buffer);

	//g_buffer�ւ̏�������2���//���̃e�N�X�`���g����(specular,diffuse)��������
	void ks_kd_begine(frame_buffer *f_buffer);
	void ks_kd_end(frame_buffer *f_buffer);

	void loadNormalMap(const wchar_t* file_name);

	//getter
	g_buffer* getGbuffer() { return g_buf.get(); }
	shader* getGBufferShader() { return g_buffer_shader.get(); }//g_buffer�̏�������(���[���h���)(color,normal,position,depth)
	shader_ex* getGBufferShaderEX() { return gbuffer_ex_shader.get(); }
	shader* getNormalMapShader() { return g_buf_normal_map_shader.get(); }//�m�[�}���}�b�v����ꍇ��g_buffer�̏�������
	shader* getDiffuseSpecularShader() { return diffuse_specular_shader.get(); }//phong�pspecular,diffuse�̏�������
};