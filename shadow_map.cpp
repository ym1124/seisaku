#include "shadow_map.h"
//
//#include "framework.h"
#include "blur.h"
#include "framework.h"
//
#include "shader.h"
#include "shader_ex.h"
#include "camera.h"
#include "frame_buffer.h"
#include "texture.h"
#include "sprite.h"

shadow_map::shadow_map(frame_buffer *f_buffer,main_camera *m_camera, main_camera *l_camera)
{
	//shadow_shader->Create(L"variance_shadow.fx", "VSMain", "PSMain");
	//shadow_shader->Create(L"shadow_map.fx", "VSMain", "PSMain");

	this->f_buffer = f_buffer;

	//shadow_shader = std::make_unique<shader>();
	//shadow_shader->Create(L"shadow_map.fx", "VSMain", "PSMain");

	shadow_to_tex_shader = std::make_unique<shader>();
	shadow_to_tex_shader->Create(L"shadow_map_to_tex.fx", "VSMain", "PSMain");
	shadow_to_tex_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForShadowMap));

	shadow_to_tex_ex_shader = std::make_unique<shader_ex>();
	shadow_to_tex_ex_shader->create("Shader/shadow_map_to_tex_ex_vs.cso", "Shader/shadow_map_to_tex_ex_ps.cso");

	this->m_camera = m_camera;
	this->l_camera = l_camera;

	depth_tex = std::make_unique<Texture_d>();
	depth_tex->Create(12800, 12800, 39, 40, 41);
	shadow_tex = std::make_unique<Texture>();
	shadow_tex->Create();
	depth_blur_tex = std::make_unique<Texture>();
	depth_blur_tex->Create();

	m_blur = std::make_unique<blur>();
}

//ŽB‰e‚Íl_camera‚ð—˜—p‚µ‚Äs‚¤
void shadow_map::createDepthMapBegine()
{
	f_buffer->createDepthMapBegin(depth_tex.get(), Light::LightDir, DirectX::XMFLOAT2(12800, 12800));
}

void shadow_map::createDepthMapEnd()
{
	f_buffer->createDepthMapEnd();
}

//ê—pƒVƒF[ƒ_[‚Å•`‰æ‚·‚é
void shadow_map::createShadowBegine(float variance,DirectX::XMFLOAT4 shadow_color)
{
	//‰e•t‚«‚Å•`‰æ
	ConstantBufferForShadowMap cbsm;
	DirectX::XMStoreFloat4x4(&cbsm.matCameraVP, DirectX::XMLoadFloat4x4(&m_camera->getView())*DirectX::XMLoadFloat4x4(&m_camera->getProjection()));
	DirectX::XMStoreFloat4x4(&cbsm.matLightVP, DirectX::XMLoadFloat4x4(&l_camera->getView())*DirectX::XMLoadFloat4x4(&l_camera->getProjection()));
	cbsm.color = shadow_color;
	cbsm.variance_pow = variance;

	UpdateConstantBuffer(shadow_to_tex_shader.get(), cbsm);
	depth_tex->Set(1);
	f_buffer->swapRenderTargetViewBegin(shadow_tex.get(),Light::LightDir,1, DirectX::XMFLOAT2(1280, 720),DirectX::XMFLOAT4(1,1,1,1));//”wŒiF‚ðy”’z‚É‚µ‚È‚¢‚Æ‡¬‚µ‚½Žž‚É”wŒi‚ª•‚­‚È‚Á‚Ä‚µ‚Ü‚¤I
}

void shadow_map::createShadowEnd()
{
	f_buffer->swapRenderTargetViewEnd();
	depth_tex->Set(1, FALSE);
}


void shadow_map::render(sprite* spr, shader* _2d_shader,float variance,bool soft_flg)
{
	
	if (soft_flg)
	{
		//ƒuƒ‰[ˆ—‚µ‚Ä•`‰æ‚¹‚¸•Û‘¶
		m_blur->render(f_buffer, variance, shadow_tex.get(), false, true, FRAMEWORK.SM::LINER_BORDER_WHITE);
		//‡¬
		FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_MULTIPLY), nullptr, 0xFFFFFFFF);
		spr->render(_2d_shader, m_blur->getResultTexture());
	}

	else
	{
		//‡¬
		FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_MULTIPLY), nullptr, 0xFFFFFFFF);
		spr->render(_2d_shader, shadow_tex.get());
	}
}