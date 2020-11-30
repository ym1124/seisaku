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

	void render(shader* _2d_shader);//phong用render//begine-end,ks_kdが必要
	void render(environment_cubemap* env_cubemap, float metalness, float roughness);//pbr用render//begine-endでg_bufferの書きだしは必要

	//g_bufferへの書きだし(color,normal,position,depth)
	void begine(frame_buffer *f_buffer,bool dsv_flg = true);//ノーマルマップ要らんならコッチ//RTV4つセットするだけ
	void begine_ex(frame_buffer *f_buffer);
	void begine(frame_buffer *f_buffer, int slot);//複数描画する場合各々の描画前でノーマルマップセットしてやる
	void end(frame_buffer *f_buffer);

	//g_bufferへの書きだし2回目//↑のテクスチャ使って(specular,diffuse)書きだし
	void ks_kd_begine(frame_buffer *f_buffer);
	void ks_kd_end(frame_buffer *f_buffer);

	void loadNormalMap(const wchar_t* file_name);

	//getter
	g_buffer* getGbuffer() { return g_buf.get(); }
	shader* getGBufferShader() { return g_buffer_shader.get(); }//g_bufferの書きだし(ワールド空間)(color,normal,position,depth)
	shader_ex* getGBufferShaderEX() { return gbuffer_ex_shader.get(); }
	shader* getNormalMapShader() { return g_buf_normal_map_shader.get(); }//ノーマルマップある場合のg_bufferの書きだし
	shader* getDiffuseSpecularShader() { return diffuse_specular_shader.get(); }//phong用specular,diffuseの書きだし
};