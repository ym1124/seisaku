#include "resource_manager.h"

#include <stdio.h>
#include <fstream>

#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>

ShaderResourceViewData	resource_manager::srvData[resource_manager::RESOURCE_MAX];
VertexShadersData		resource_manager::vsData[resource_manager::RESOURCE_MAX];
PixelShadersData		resource_manager::psData[resource_manager::RESOURCE_MAX];
GeometryShadersData		resource_manager::gsData[resource_manager::RESOURCE_MAX];
HullShadersData			resource_manager::hsData[resource_manager::RESOURCE_MAX];
DomainShadersData		resource_manager::dsData[resource_manager::RESOURCE_MAX];
ComputeShadersData		resource_manager::csData[resource_manager::RESOURCE_MAX];


//	CSO�t�@�C���ǂݍ��݊֐�
bool LoadCSOFile(BYTE** data, long* size, const char* filename)
{
	FILE* fp;
	//	�t�@�C���I�[�v��
	if (fopen_s(&fp, filename, "rb"))	return false;

	//	�t�@�C���T�C�Y�擾
	fseek(fp, 0, SEEK_END);		//	�t�@�C���|�C���^���I�[�Ɉړ�
	long sz = ftell(fp);		//	�I�[�ʒu�̕ۑ� = �t�@�C���T�C�Y
	fseek(fp, 0, SEEK_SET);		//	�t�@�C���|�C���^���ŏ��ɖ߂�

								//	�t�@�C���ǂݍ��ݗp�̈�̊m��
	*data = new BYTE[sz];
	//	�t�@�C���ǂݍ���
	size_t s = fread_s(*data, sizeof(BYTE) * sz,
		sizeof(BYTE), sz, fp);

	//	�ǂݍ��ݏI��
	fclose(fp);

	//	�G���[�`�F�b�N
	if (s != sizeof(BYTE) * sz)	return false;

	*size = sz;			//	�t�@�C���T�C�Y�ۑ�
	return	true;
}



bool resource_manager::loadShaderResourceView(ID3D11Device* device, const wchar_t* filename,ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC** texDesc)
{
	//	�G���[�`�F�b�N
	if (!device)		return false;
	if (!filename)		return false;
	if (*srv)			return false;

	HRESULT hr;
	//	ID3D11Resource* resource = nullptr;;
	ShaderResourceViewData* find = nullptr;
	int no = -1;

	//	�Ώۂ̃t�@�C�������ɑ��݂��Ă��邩������
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ShaderResourceViewData* p = &srvData[n];

		//	�f�[�^�̗L���m�F
		if (p->count <= 0)
		{
			if (no == -1)		no = n;
			continue;
		}

		//	�������r
		if (wcscmp(p->filename, filename) != 0)	continue;

		//	��v��������ǂݍ���ł� => ����!
		find = p;

		break;
	}

	//	�t�@�C����������Ȃ�������V�K�ǂݍ���
	if (!find)
	{
		if (no == -1)	return	false;		//	�󂫂�����

		find = &srvData[no];

		std::wstring tex_file_name = filename;

		tex_file_name.erase(tex_file_name.begin(), tex_file_name.begin() + (tex_file_name.find_first_of(L".")));

		ID3D11Resource* resource = nullptr;

		if (tex_file_name == L".dds") {
			hr = DirectX::CreateDDSTextureFromFile(device, filename, &resource,
				&find->shader_resource_view);
		}
		else {
			hr = DirectX::CreateWICTextureFromFile(device, filename, &resource,
				&find->shader_resource_view);
		}
		//		���f�[�^���ǂݍ��߂Ȃ������� return false;
		if (FAILED(hr))	return false;

		//	texture2d_desc�̎擾
		ID3D11Texture2D* texture2d;
		hr = resource->QueryInterface(&texture2d);
		//		���f�[�^���ǂݍ��߂Ȃ������� return false;
		if (FAILED(hr))
		{
			resource->Release();
			find->shader_resource_view->Release();
			return false;
		}
		//	texture2d_desc�ۑ�
		texture2d->GetDesc(&find->texDesc);

		//	�|�C���^�ϐ� ���
		texture2d->Release();
		resource->Release();

		//	�t�@�C�����̃R�s�[
		wcscpy_s(find->filename, 256, filename);
	}

	//////////////////////////
	//	���ʕ�
	//////////////////////////

	//	�f�[�^��Ԃ�
	*srv = find->shader_resource_view;	//	ShaderResourceView�ۑ�
	*texDesc = &find->texDesc;				//	texture2d_desc�ۑ�

											//	�ێ�������
	find->count++;


	return	true;
}

void resource_manager::releaseShaderResourceView(ID3D11ShaderResourceView* srv)
{
	if (!srv)			return;		//	�����Ώ�Unknown�@�I�����܂�

									//	Target�̌����J�n
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ShaderResourceViewData* p = &srvData[n];

		//	���g�p�f�[�^�Ȃ�A���̃f�[�^�ֈڍs���܂�
		if (p->count == 0)	continue;
		//	�����Ώۂƕs��v�Ȃ�A���̃f�[�^�ֈڍs���܂�
		if (p->shader_resource_view != srv)	continue;

		//	Target�𔭌� Release���܂�
		p->Release();
		break;
	}
}


bool resource_manager::loadVertexShaders(ID3D11Device* device, const char* filename,D3D11_INPUT_ELEMENT_DESC* elementDescs, int numElement,ID3D11VertexShader** vs, ID3D11InputLayout** il)
{
	//	�G���[�`�F�b�N
	if (!device)		return false;
	if (!filename)		return false;
	if (*vs)			return false;
	if (*il)			return false;

	HRESULT hr;
	VertexShadersData* find = nullptr;
	int no = -1;

	//	�Ώۂ̃t�@�C�������ɑ��݂��Ă��邩������
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		VertexShadersData* p = &vsData[n];

		//	�f�[�^�̗L���m�F
		if (p->count == 0)
		{
			if (no == -1)	no = n;
			continue;
		}
		//	�������r
		if (strcmp(p->filename, filename) != 0)	continue;

		//	��v��������ǂݍ���ł� => ����!
		find = p;
		//	���łɍ����̂�����΂�����
		//		�� VertexShaders�̏ꍇ�A���ɖ���
		break;
	}

	//	�t�@�C����������Ȃ�������V�K�ǂݍ���
	if (!find)
	{
		if (no == -1)	return	false;		//	�󂫂�����

		find = &vsData[no];
		//	�V�K�ǂݍ���
		BYTE* shader_data;
		long				size;
		LoadCSOFile(&shader_data, &size, filename);
		hr = device->CreateVertexShader(shader_data, size, nullptr, &find->vertex_shader);
		//		���f�[�^���ǂݍ��߂Ȃ������� return false;
		if (FAILED(hr))	return	false;

		hr = device->CreateInputLayout(elementDescs, numElement,shader_data, size, &find->input_layout);
		delete[] shader_data;
		//		���f�[�^���ǂݍ��߂Ȃ������� return false;
		if (FAILED(hr))
		{
			find->vertex_shader->Release();
			return false;
		}

		//	�t�@�C�����̃R�s�[
		strcpy_s(find->filename, 256, filename);
	}

	//////////////////////////
	//	���ʕ�
	//////////////////////////

	//	���ʂ��č����̂�����΂����ō��
	//		��VertexShaders�œ��ɂ�鎖�͖���

	//	�f�[�^��Ԃ�
	*vs = find->vertex_shader;
	*il = find->input_layout;

	//	�ێ�������
	find->count++;

	return	true;
}

void resource_manager::releaseVertexShaders(ID3D11VertexShader* vs, ID3D11InputLayout* il)
{
	if (!vs)			return;		//	�����Ώ�Unknown�@�I�����܂�
	if (!il)			return;		//	�����Ώ�Unknown�@�I�����܂�

									//	Target�̌����J�n
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		VertexShadersData* p = &vsData[n];

		//	���g�p�f�[�^�Ȃ�A���̃f�[�^�ֈڍs���܂�
		if (p->count == 0)	continue;
		//	�����Ώۂƕs��v�Ȃ�A���̃f�[�^�ֈڍs���܂�
		if (p->vertex_shader != vs)	continue;
		if (p->input_layout != il)	continue;

		//	Target�𔭌� Release���܂�
		p->Release();
		break;
	}
}



bool resource_manager::loadPixelShaders(ID3D11Device* device, const char* filename, ID3D11PixelShader** ps)
{
	//	�G���[�`�F�b�N
	if (!device)		return false;
	if (!filename)		return false;
	if (*ps)			return false;

	HRESULT hr;
	PixelShadersData* find = nullptr;
	int no = -1;

	//	�Ώۂ̃t�@�C�������ɑ��݂��Ă��邩������
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		PixelShadersData* p = &psData[n];

		//	�f�[�^�̗L���m�F
		if (p->count == 0)
		{
			if (no == -1)	no = n;
			continue;
		}
		//	�������r
		if (strcmp(p->filename, filename) != 0)	continue;

		//	��v��������ǂݍ���ł� => ����!
		find = p;
		//	���łɍ����̂�����΂�����
		//		�� VertexShaders�̏ꍇ�A���ɖ���
		break;
	}

	//	�t�@�C����������Ȃ�������V�K�ǂݍ���
	if (!find)
	{
		if (no == -1)	return	false;		//	�󂫂�����

		find = &psData[no];
		//	�V�K�ǂݍ���
		BYTE* shader_data;
		long				size;
		LoadCSOFile(&shader_data, &size, filename);
		hr = device->CreatePixelShader(shader_data, size, nullptr, &find->pixel_shader);
		//		���f�[�^���ǂݍ��߂Ȃ������� return false;
		if (FAILED(hr))	return	false;

		//	�t�@�C�����̃R�s�[
		strcpy_s(find->filename, 256, filename);

		delete[]	shader_data;
	}

	//////////////////////////
	//	���ʕ�
	//////////////////////////

	//	���ʂ��č����̂�����΂����ō��
	//		��VertexShaders�œ��ɂ�鎖�͖���

	//	�f�[�^��Ԃ�
	*ps = find->pixel_shader;

	//	�ێ�������
	find->count++;

	return	true;
}

void resource_manager::releasePixelShaders(ID3D11PixelShader* ps)
{
	if (!ps)			return;		//	�����Ώ�Unknown�@�I�����܂�

									//	Target�̌����J�n
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		PixelShadersData* p = &psData[n];

		//	���g�p�f�[�^�Ȃ�A���̃f�[�^�ֈڍs���܂�
		if (p->count == 0)	continue;
		//	�����Ώۂƕs��v�Ȃ�A���̃f�[�^�ֈڍs���܂�
		if (p->pixel_shader != ps)	continue;

		//	Target�𔭌� Release���܂�
		p->Release();
		break;
	}
}




bool resource_manager::loadGeometryShaders(ID3D11Device* device, const char* filename, ID3D11GeometryShader** gs)
{
	//	�G���[�`�F�b�N
	if (!device)		return false;
	if (!filename)		return false;
	if (*gs)			return false;

	HRESULT hr;
	GeometryShadersData* find = nullptr;
	int no = -1;

	//	�Ώۂ̃t�@�C�������ɑ��݂��Ă��邩������
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		GeometryShadersData* p = &gsData[n];

		//	�f�[�^�̗L���m�F
		if (p->count == 0)
		{
			if (no == -1)	no = n;
			continue;
		}
		//	�������r
		if (strcmp(p->filename, filename) != 0)	continue;

		//	��v��������ǂݍ���ł� => ����!
		find = p;
		//	���łɍ����̂�����΂�����
		//		�� VertexShaders�̏ꍇ�A���ɖ���
		break;
	}

	//	�t�@�C����������Ȃ�������V�K�ǂݍ���
	if (!find)
	{
		if (no == -1)	return	false;		//	�󂫂�����

		find = &gsData[no];
		//	�V�K�ǂݍ���
		BYTE* shader_data;
		long				size;
		LoadCSOFile(&shader_data, &size, filename);
		hr = device->CreateGeometryShader(shader_data, size, nullptr, &find->geometry_shader);
		//		���f�[�^���ǂݍ��߂Ȃ������� return false;
		if (FAILED(hr))	return	false;

		//	�t�@�C�����̃R�s�[
		strcpy_s(find->filename, 256, filename);

		delete[]	shader_data;
	}

	//////////////////////////
	//	���ʕ�
	//////////////////////////

	//	���ʂ��č����̂�����΂����ō��
	//		��VertexShaders�œ��ɂ�鎖�͖���

	//	�f�[�^��Ԃ�
	*gs = find->geometry_shader;

	//	�ێ�������
	find->count++;

	return	true;
}

void resource_manager::releaseGeometryShaders(ID3D11GeometryShader* gs)
{
	if (!gs)			return;		//	�����Ώ�Unknown�@�I�����܂�

									//	Target�̌����J�n
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		GeometryShadersData* p = &gsData[n];

		//	���g�p�f�[�^�Ȃ�A���̃f�[�^�ֈڍs���܂�
		if (p->count == 0)	continue;
		//	�����Ώۂƕs��v�Ȃ�A���̃f�[�^�ֈڍs���܂�
		if (p->geometry_shader != gs)	continue;

		//	Target�𔭌� Release���܂�
		p->Release();
		break;
	}
}




bool resource_manager::loadHullShaders(ID3D11Device* device, const char* filename, ID3D11HullShader** hs)
{
	//	�G���[�`�F�b�N
	if (!device)		return false;
	if (!filename)		return false;
	if (*hs)			return false;

	HRESULT hr;
	HullShadersData* find = nullptr;
	int no = -1;

	//	�Ώۂ̃t�@�C�������ɑ��݂��Ă��邩������
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		HullShadersData* p = &hsData[n];

		//	�f�[�^�̗L���m�F
		if (p->count == 0)
		{
			if (no == -1)	no = n;
			continue;
		}
		//	�������r
		if (strcmp(p->filename, filename) != 0)	continue;

		//	��v��������ǂݍ���ł� => ����!
		find = p;
		//	���łɍ����̂�����΂�����
		//		�� VertexShaders�̏ꍇ�A���ɖ���
		break;
	}

	//	�t�@�C����������Ȃ�������V�K�ǂݍ���
	if (!find)
	{
		if (no == -1)	return	false;		//	�󂫂�����

		find = &hsData[no];
		//	�V�K�ǂݍ���
		BYTE* shader_data;
		long				size;
		LoadCSOFile(&shader_data, &size, filename);
		hr = device->CreateHullShader(shader_data, size, nullptr, &find->hull_shader);
		//		���f�[�^���ǂݍ��߂Ȃ������� return false;
		if (FAILED(hr))	return	false;

		//	�t�@�C�����̃R�s�[
		strcpy_s(find->filename, 256, filename);

		delete[]	shader_data;
	}

	//////////////////////////
	//	���ʕ�
	//////////////////////////

	//	���ʂ��č����̂�����΂����ō��
	//		��VertexShaders�œ��ɂ�鎖�͖���

	//	�f�[�^��Ԃ�
	*hs = find->hull_shader;

	//	�ێ�������
	find->count++;

	return	true;
}

void resource_manager::releaseHullShaders(ID3D11HullShader* hs)
{
	if (!hs)			return;		//	�����Ώ�Unknown�@�I�����܂�

									//	Target�̌����J�n
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		HullShadersData* p = &hsData[n];

		//	���g�p�f�[�^�Ȃ�A���̃f�[�^�ֈڍs���܂�
		if (p->count == 0)	continue;
		//	�����Ώۂƕs��v�Ȃ�A���̃f�[�^�ֈڍs���܂�
		if (p->hull_shader != hs)	continue;

		//	Target�𔭌� Release���܂�
		p->Release();
		break;
	}
}




bool resource_manager::loadDomainShaders(ID3D11Device* device, const char* filename, ID3D11DomainShader** ds)
{
	//	�G���[�`�F�b�N
	if (!device)		return false;
	if (!filename)		return false;
	if (*ds)			return false;

	HRESULT hr;
	DomainShadersData* find = nullptr;
	int no = -1;

	//	�Ώۂ̃t�@�C�������ɑ��݂��Ă��邩������
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		DomainShadersData* p = &dsData[n];

		//	�f�[�^�̗L���m�F
		if (p->count == 0)
		{
			if (no == -1)	no = n;
			continue;
		}
		//	�������r
		if (strcmp(p->filename, filename) != 0)	continue;

		//	��v��������ǂݍ���ł� => ����!
		find = p;
		//	���łɍ����̂�����΂�����
		//		�� VertexShaders�̏ꍇ�A���ɖ���
		break;
	}

	//	�t�@�C����������Ȃ�������V�K�ǂݍ���
	if (!find)
	{
		if (no == -1)	return	false;		//	�󂫂�����

		find = &dsData[no];
		//	�V�K�ǂݍ���
		BYTE* shader_data;
		long				size;
		LoadCSOFile(&shader_data, &size, filename);
		hr = device->CreateDomainShader(shader_data, size, nullptr, &find->domain_shader);
		//		���f�[�^���ǂݍ��߂Ȃ������� return false;
		if (FAILED(hr))	return	false;

		//	�t�@�C�����̃R�s�[
		strcpy_s(find->filename, 256, filename);

		delete[]	shader_data;
	}

	//////////////////////////
	//	���ʕ�
	//////////////////////////

	//	���ʂ��č����̂�����΂����ō��
	//		��VertexShaders�œ��ɂ�鎖�͖���

	//	�f�[�^��Ԃ�
	*ds = find->domain_shader;

	//	�ێ�������
	find->count++;

	return	true;
}

void resource_manager::releaseDomainShaders(ID3D11DomainShader* ds)
{
	if (!ds)			return;		//	�����Ώ�Unknown�@�I�����܂�

									//	Target�̌����J�n
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		DomainShadersData* p = &dsData[n];

		//	���g�p�f�[�^�Ȃ�A���̃f�[�^�ֈڍs���܂�
		if (p->count == 0)	continue;
		//	�����Ώۂƕs��v�Ȃ�A���̃f�[�^�ֈڍs���܂�
		if (p->domain_shader != ds)	continue;

		//	Target�𔭌� Release���܂�
		p->Release();
		break;
	}
}




bool resource_manager::loadComputeShaders(ID3D11Device* device, const char* filename, ID3D11ComputeShader** cs)
{
	//	�G���[�`�F�b�N
	if (!device)		return false;
	if (!filename)		return false;
	if (*cs)			return false;

	HRESULT hr;
	ComputeShadersData* find = nullptr;
	int no = -1;

	//	�Ώۂ̃t�@�C�������ɑ��݂��Ă��邩������
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ComputeShadersData* p = &csData[n];

		//	�f�[�^�̗L���m�F
		if (p->count == 0)
		{
			if (no == -1)	no = n;
			continue;
		}
		//	�������r
		if (strcmp(p->filename, filename) != 0)	continue;

		//	��v��������ǂݍ���ł� => ����!
		find = p;
		//	���łɍ����̂�����΂�����
		//		�� VertexShaders�̏ꍇ�A���ɖ���
		break;
	}

	//	�t�@�C����������Ȃ�������V�K�ǂݍ���
	if (!find)
	{
		if (no == -1)	return	false;		//	�󂫂�����

		find = &csData[no];
		//	�V�K�ǂݍ���
		BYTE* shader_data;
		long				size;
		LoadCSOFile(&shader_data, &size, filename);
		hr = device->CreateComputeShader(shader_data, size, nullptr, &find->compute_shader);
		//		���f�[�^���ǂݍ��߂Ȃ������� return false;
		if (FAILED(hr))	return	false;

		//	�t�@�C�����̃R�s�[
		strcpy_s(find->filename, 256, filename);

		delete[]	shader_data;
	}

	//////////////////////////
	//	���ʕ�
	//////////////////////////

	//	���ʂ��č����̂�����΂����ō��
	//		��VertexShaders�œ��ɂ�鎖�͖���

	//	�f�[�^��Ԃ�
	*cs = find->compute_shader;

	//	�ێ�������
	find->count++;

	return	true;
}

void resource_manager::releaseComputeShaders(ID3D11ComputeShader* cs)
{
	if (!cs)			return;		//	�����Ώ�Unknown�@�I�����܂�

									//	Target�̌����J�n
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ComputeShadersData* p = &csData[n];

		//	���g�p�f�[�^�Ȃ�A���̃f�[�^�ֈڍs���܂�
		if (p->count == 0)	continue;
		//	�����Ώۂƕs��v�Ȃ�A���̃f�[�^�ֈڍs���܂�
		if (p->compute_shader != cs)	continue;

		//	Target�𔭌� Release���܂�
		p->Release();
		break;
	}
}

