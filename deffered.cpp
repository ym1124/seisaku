#include "deffered.h"
#include "frame_buffer.h"
#include "environment_cubemap.h"

deffered::deffered()
{
	g_buffer_shader = std::make_unique<shader>();
	g_buffer_shader->Create(L"Shader/g_buffer");
	gbuffer_ex_shader = std::make_unique<shader_ex>();
	gbuffer_ex_shader->create("Shader/g_buffer_ex_vs.cso", "Shader/g_buffer_ex_ps.cso");
	g_buf_normal_map_shader = std::make_unique<shader>();
	g_buf_normal_map_shader->Create(L"Shader/g_buffer_normal_map");
	diffuse_specular_shader = std::make_unique<shader>();
	diffuse_specular_shader->Create(L"Shader/deffered_light_to_tex");
	pbr_deffered_shader = std::make_unique<shader>();
	pbr_deffered_shader->Create(L"Shader/deffered_pbr");
	pbr_deffered_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForPBREX));

	spr = std::make_unique<sprite>();
	g_buf = std::make_unique<g_buffer>();
}

deffered::~deffered()
{
}

void deffered::render(shader* _2d_shader)
{
	//�A���t�@�u�����h�ݒ�
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ALPHA), nullptr, 0x000000FF);
	//�A���x�h
	spr->render(_2d_shader, g_buf->tex_buffer[0].get());
	//��Z�u�����h�ݒ�
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_MULTIPLY), nullptr, 0xFFFFFFFF);
	//�g�U��
	spr->render(_2d_shader, g_buf->tex_buffer2[0].get());
	//���Z�u�����h�ݒ�
	FRAMEWORK.getImmediateContext()->OMSetBlendState(FRAMEWORK.getBlendState(FRAMEWORK.BS::BS_ADD), nullptr, 0xFFFFFFFF);
	//���ʔ���
	spr->render(_2d_shader, g_buf->tex_buffer2[1].get());
}

void deffered::render(environment_cubemap *env_cubemap,float metalness,float roughness)
{
	//PBRv2�p�̃e�N�X�`��
	//metalness_tex->Set(2);
	FRAMEWORK.getImmediateContext()->PSSetShaderResources(4, 1, env_cubemap->getShaderResourceViewAddress());
	//FRAMEWORK.getImmediateContext()->PSSetShaderResources(5, 1, ir_map->getICTShaderResourceViewAddress());
	FRAMEWORK.getImmediateContext()->PSSetShaderResources(5, 1, env_cubemap->getShaderResourceViewAddress());
	FRAMEWORK.getImmediateContext()->PSSetShaderResources(6, 1, env_cubemap->getShaderResourceViewAddress());
	FRAMEWORK.getImmediateContext()->PSSetShaderResources(7, 1, g_buf->tex_buffer[2]->GetShaderResourceViewPointer());//pos
	FRAMEWORK.getImmediateContext()->PSSetShaderResources(8, 1, g_buf->tex_buffer[1]->GetShaderResourceViewPointer());//normal
	ConstantBufferForPBREX cbpbrex;
	cbpbrex.metalness = metalness;
	cbpbrex.roughness = roughness;
	UpdateConstantBuffer(pbr_deffered_shader.get(), cbpbrex);
	spr->render(pbr_deffered_shader.get(), g_buf->tex_buffer[0].get());
}

//���I�Ɋm��
void deffered::loadNormalMap(const wchar_t* file_name)
{
	Texture tmp;
	tmp.SetShaderResourceView(nullptr);
	tmp.Create(1280, 720, DXGI_FORMAT_R8G8B8A8_UNORM);
	tmp.Load(file_name);
	normal_maps.push_back(tmp);
}

void deffered::begine(frame_buffer *f_buffer,bool dsv_flg)
{
	//g_buffer��������
	g_buf->begine(f_buffer, dsv_flg);
}

void deffered::begine(frame_buffer *f_buffer,int slot)
{
	//g_buffer��������
	g_buf->begine(f_buffer);
	normal_maps.at(slot).Set(1);//normal�}�b�v�Z�b�g//�����I�u�W�F�N�g�`�悷��Ȃ玟�̕`��̑O�ɂ��̃I�u�W�F�N�g�̃m�[�}���}�b�v���Z�b�g�I
}

void deffered::end(frame_buffer *f_buffer)
{
	g_buf->end(f_buffer);
}


void deffered::ks_kd_begine(frame_buffer *f_buffer)
{
	g_buf->ks_kd_begine(f_buffer);
	g_buf->tex_buffer[1]->Set(1);
	g_buf->tex_buffer[2]->Set(2);
}

void deffered::ks_kd_end(frame_buffer *f_buffer)
{
	g_buf->ks_kd_end(f_buffer);
}
