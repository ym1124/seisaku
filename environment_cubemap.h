#pragma once

#include <DirectXTex.h>
#include "framework.h"

#include <wrl.h>
#include <d3d11.h>
#include <direct.h>

#include "camera.h"
#include "misc.h"
#include "cube_mesh.h"

class camera;
class main_camera;

//Comptr�������烊�[�N�������������ǃq�[�v������Ȃ��Ȃ�/9/7

class environment_cubemap
{
public:
	static const UINT number_of_faces = 6;

private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;//RTV��SRV������ł����Ȃ�8/26
	//Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_views[number_of_faces] = { NULL };//����������
	//Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_views[number_of_faces] = { NULL };//����������
	ID3D11RenderTargetView* render_target_views[number_of_faces] = { nullptr };//����������
	ID3D11DepthStencilView* depth_stencil_views[number_of_faces] = { nullptr };//����������

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view = nullptr;
	D3D11_VIEWPORT viewport;
	DirectX::TexMetadata metadata;//�摜�̏��

	std::unique_ptr<main_camera> cameras[number_of_faces];//�e�ʎB�e�p�̃J����

	UINT number_of_cached_viewports = 0;
	D3D11_VIEWPORT cached_viewports[D3D11_VIEWPORT_AND_SCISSORRECT_OBJECT_COUNT_PER_PIPELINE] = {};

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> cached_render_target_view = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> cached_depth_stencil_view = nullptr;
public:

	//texture�̑傫��
	environment_cubemap(ID3D11Device* device, UINT dimension, DXGI_FORMAT format = DXGI_FORMAT_R16G16B16A16_FLOAT, bool generate_mips = true);
	~environment_cubemap() { release(); }

	bool generated_mips;
	size_t miplevels;

	void clear(ID3D11DeviceContext* immediate_context, size_t face);//RTV��DSV���N���A
	void activate(ID3D11DeviceContext* immediate_context, size_t face);//vp��rtv���Z�b�g

	void save_to_dds(ID3D11DeviceContext* immediate_context, const wchar_t* filename);//filename�ɏ����o���������O�����
	void load_dds(ID3D11DeviceContext* immediate_context, const wchar_t* filename);

	//void capture(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT4 eye);//IBL�p�̊��}�b�v�B�e
	void captureBegine(ID3D11DeviceContext* immediate_context, DirectX::XMFLOAT4 eye, int loop_num);//IBL�p�̊��}�b�v�B�e
	void captureEnd(ID3D11DeviceContext* immediate_context);
	void calculate_matrix(DirectX::XMFLOAT4 eye, bool perspective_projection = true, float fovy_or_view_width = 90, float near_z = 0.1f, float far_z = 100.0f);

	//getter
	ID3D11Texture2D* getTexture2D() { return texture2d.Get(); };
	ID3D11ShaderResourceView* getShaderResourceView() { return  shader_resource_view.Get(); };
	ID3D11ShaderResourceView** getShaderResourceViewAddress() { return shader_resource_view.GetAddressOf(); };
	main_camera* getCamera(int number) { return cameras[number].get(); };

	//setter
	void setShaderResourceView(ID3D11ShaderResourceView* srv) { shader_resource_view = srv; };

	void update();
	void render();
	void release();
};