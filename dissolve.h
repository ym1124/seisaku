#pragma once
class shader_ex;
class Texture;

class dissolve
{
private:
	std::unique_ptr<shader_ex> dissolve_shader;
	std::unique_ptr<Texture> dissolve_tex, dissolve_edge_tex;
public:
	dissolve();
	~dissolve() {};

	void render(float emissive_width, float threshold);

	shader_ex* getDissolveShader() { return dissolve_shader.get(); }
};