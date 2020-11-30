#include "dissolve.h"
#include "shader_ex.h"
#include "texture.h"

dissolve::dissolve()
{
	dissolve_tex = std::make_unique<Texture>();
	dissolve_tex->Load(L"images/dissolve/dissolve_animation2.png");//dissolve_animation1,dissolve_animation3‚à‚ ‚é
	dissolve_edge_tex = std::make_unique<Texture>();
	dissolve_edge_tex->Load(L"images/dissolve/dissolve_edgecolor.png");

	dissolve_shader = std::make_unique<shader_ex>();
	dissolve_shader->create("Shader/dissolve_vs.cso", "Shader/dissolve_ps.cso");
	dissolve_shader->createConstantBufferOriginal(sizeof(ConstantBufferForDissolve));
}

void dissolve::render(float emissive_width,float threshold)
{
	ConstantBufferForDissolve cbd;
	cbd.DissolveEmissiveWidth = emissive_width;
	cbd.DissolveThreshold = threshold;
	UpdateConstantBuffer(dissolve_shader.get(), cbd);
	dissolve_tex->Set(1);
	dissolve_edge_tex->Set(2);
}