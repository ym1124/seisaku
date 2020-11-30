#pragma once
class main_camera;
class shader;
class shader_ex;
class frame_buffer;
class sprite;
class blur;

class shadow_map
{
private:
	//2d_shaderとかphongとかは何度も作るのもったいない
	frame_buffer* f_buffer;
	std::unique_ptr<shader> shadow_shader, shadow_to_tex_shader;
	std::unique_ptr<shader_ex> shadow_to_tex_ex_shader;
	main_camera *m_camera, *l_camera;
	std::unique_ptr<Texture_d> depth_tex;
	std::unique_ptr<Texture> shadow_tex, depth_blur_tex;
	std::unique_ptr<blur> m_blur;
public:
	shadow_map(frame_buffer *f_buffer,main_camera *m_camera,main_camera *l_camera);
	~shadow_map() {};

	//getter
	shader* getShadowMapToTexShader() { return shadow_to_tex_shader.get(); }
	shader_ex* getShadowMapToTexShaderEX() { return shadow_to_tex_ex_shader.get(); }
	Texture* getShadowTex() { return shadow_tex.get(); }

	//setter
	void setDepthMap(Texture_d *depth_tex) { this->depth_tex->SetShaderResourceView(depth_tex->GetShaderResourceView()); }

	//シャドウマップ用深度マップ//撮影はl_cameraを利用して行う
	void createDepthMapBegine();
	void createDepthMapEnd();

	//影描画//getShadowMapToTexShaderを利用して描画
	void createShadowBegine(float variance, DirectX::XMFLOAT4 shadow_color=DirectX::XMFLOAT4(0.3f,0.3f,0.3f,1.0f));
	void createShadowEnd();

	//描画
	void render(sprite* spr, shader* _2d_shader, float variance, bool soft_flg=true);
};