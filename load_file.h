#pragma once
#include <d3d11.h>

void vs_Load(ID3D11Device *pDevice, const char *cso_name, ID3D11VertexShader **vertex_shader, ID3D11InputLayout **input_layout, D3D11_INPUT_ELEMENT_DESC *input_element_desc, UINT num_elements);
void ps_Load(ID3D11Device *pDevice, const char *cso_name, ID3D11PixelShader **pixel_shader);
void hs_Load(ID3D11Device *pDevice, const char *cso_name, ID3D11HullShader **hull_shader);
void ds_Load(ID3D11Device *pDevice, const char *cso_name, ID3D11DomainShader **domain_shader);
HRESULT load_texture_from_file(ID3D11Device *device, const wchar_t *file_name, ID3D11ShaderResourceView **shader_resource_view, D3D11_TEXTURE2D_DESC *texture2d_desc);
HRESULT load_dummy_texture(ID3D11Device *device, const wchar_t *file_name, ID3D11ShaderResourceView **shader_resource_view, D3D11_TEXTURE2D_DESC *texture2d_desc);
HRESULT make_dummy_texture(ID3D11Device *device, ID3D11ShaderResourceView **shader_resource_view);
void combine_resource_path(wchar_t(&combined_resource_path)[256], const wchar_t *referrer_filename, const wchar_t *referent_filename);
