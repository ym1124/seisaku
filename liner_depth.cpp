#include "liner_depth.h"
#include "frame_buffer.h"
#include "camera.h"
#include "texture.h"
#include "shader.h"
#include "shader_ex.h"

liner_depth::liner_depth()
{
	liner_z_tex = std::make_unique<Texture>();
	liner_depth_shader = std::make_unique<shader>();
	liner_depth_shader->Create(L"Shader/liner_depth");
	liner_depth_shader->CreateConstantBufferOriginal(sizeof(ConstantBufferForLinerDepth));
	liner_depth_shader_ex = std::make_unique<shader_ex>();
	liner_depth_shader_ex->create("Shader/liner_depth_ex_vs.cso", "Shader/liner_depth_ex_ps.cso");
}

void liner_depth::begine(frame_buffer *f_buffer,main_camera *m_camera)
{
	//改造したプロジェクション行列を渡す
	ConstantBufferForLinerDepth cb;
	{
		customProjection._11 = m_camera->getProjection()._11;
		customProjection._21 = m_camera->getProjection()._21;
		customProjection._31 = m_camera->getProjection()._31;
		customProjection._41 = m_camera->getProjection()._41;
		customProjection._21 = m_camera->getProjection()._21;
		customProjection._22 = m_camera->getProjection()._22;
		customProjection._23 = m_camera->getProjection()._23;
		customProjection._24 = m_camera->getProjection()._24;
		customProjection._31 = m_camera->getProjection()._31;
		customProjection._32 = m_camera->getProjection()._32;
		customProjection._33 = 1;
		customProjection._34 = m_camera->getProjection()._34;
		customProjection._41 = 0;
		customProjection._42 = 0;
		customProjection._43 = 0;
		customProjection._44 = 0;
	}
	cb.customProjection = customProjection;
	cb.farZ = m_camera->getFarZ();
	cb.nearZ = m_camera->getNearZ();
	cb.dummy = DirectX::XMFLOAT2(0, 0);
	UpdateConstantBuffer(liner_depth_shader.get(), cb);
	f_buffer->swapRenderTargetViewBegin(liner_z_tex.get());
}

void liner_depth::end(frame_buffer *f_buffer)
{
	f_buffer->swapRenderTargetViewEnd();
}