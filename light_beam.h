#pragma once
//#include <memory>

//#include "frame_buffer.h"
//#include "shader.h"
//#include "blur.h"

class shader;
class blur;
class frame_buffer;
class Texture;
class sprite;
struct ConstantBufferForBrightnessExtraction;

class light_beam
{
protected:
	std::unique_ptr<shader> irradiation_shader,bright_shader,_2d_shader;
	std::unique_ptr<Texture> scene_tex,bright_tex, blur_tex[3],irradiation_tex[4];
	std::unique_ptr<sprite> spr;
public:
	light_beam();
	~light_beam() {};

	//settter
	//ƒV[ƒ“‚Ì•`‰æ‚©‚çscene_tex‚ğì¬‚·‚éê‡
	void setSceneTexBegine(frame_buffer *f_buffer);
	void setSceneTexEnd(frame_buffer *f_buffer);
	//ŠO•”‚©‚ç–á‚¦‚é‚È‚ç‚±‚Á‚¿
	void setSceneTexture(Texture* scene_tex);

	//getter
	Texture* getSceneTexture() { return scene_tex.get(); }

	//render
	void render(frame_buffer *f_buffer, float threshold,DirectX::XMFLOAT2 screen_size = DirectX::XMFLOAT2(1280,720));
};
