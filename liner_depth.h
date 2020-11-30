#pragma once
#include <memory>
#include <DirectXMath.h>
//#include "texture.h"
//#include "shader.h"
//#include "shader_ex.h"

class shader;
class shader_ex;
class Texture;
class main_camera;



class liner_depth
{
private:
	std::unique_ptr<Texture> liner_z_tex;
	DirectX::XMFLOAT4X4 customProjection;//カスタムしたプロジェクション行列
	std::unique_ptr<shader> liner_depth_shader;//
	std::unique_ptr<shader_ex> liner_depth_shader_ex;//
public:
	liner_depth();
	~liner_depth() {};
	void begine(frame_buffer *f_buffer, main_camera *m_camera);
	void end(frame_buffer *f_buffer);

	//getter
	shader* getShader() { return liner_depth_shader.get(); }
	shader_ex* getShaderEX() { return liner_depth_shader_ex.get(); }
	Texture* getTexture() { return liner_z_tex.get(); }
};