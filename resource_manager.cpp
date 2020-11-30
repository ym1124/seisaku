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


//	CSOファイル読み込み関数
bool LoadCSOFile(BYTE** data, long* size, const char* filename)
{
	FILE* fp;
	//	ファイルオープン
	if (fopen_s(&fp, filename, "rb"))	return false;

	//	ファイルサイズ取得
	fseek(fp, 0, SEEK_END);		//	ファイルポインタを終端に移動
	long sz = ftell(fp);		//	終端位置の保存 = ファイルサイズ
	fseek(fp, 0, SEEK_SET);		//	ファイルポインタを最初に戻す

								//	ファイル読み込み用領域の確保
	*data = new BYTE[sz];
	//	ファイル読み込み
	size_t s = fread_s(*data, sizeof(BYTE) * sz,
		sizeof(BYTE), sz, fp);

	//	読み込み終了
	fclose(fp);

	//	エラーチェック
	if (s != sizeof(BYTE) * sz)	return false;

	*size = sz;			//	ファイルサイズ保存
	return	true;
}



bool resource_manager::loadShaderResourceView(ID3D11Device* device, const wchar_t* filename,ID3D11ShaderResourceView** srv, D3D11_TEXTURE2D_DESC** texDesc)
{
	//	エラーチェック
	if (!device)		return false;
	if (!filename)		return false;
	if (*srv)			return false;

	HRESULT hr;
	//	ID3D11Resource* resource = nullptr;;
	ShaderResourceViewData* find = nullptr;
	int no = -1;

	//	対象のファイルが既に存在しているかを検索
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ShaderResourceViewData* p = &srvData[n];

		//	データの有無確認
		if (p->count <= 0)
		{
			if (no == -1)		no = n;
			continue;
		}

		//	文字列比較
		if (wcscmp(p->filename, filename) != 0)	continue;

		//	一致したから読み込んでる => 発見!
		find = p;

		break;
	}

	//	ファイルが見つからなかったら新規読み込み
	if (!find)
	{
		if (no == -1)	return	false;		//	空きが無い

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
		//		→データが読み込めなかったら return false;
		if (FAILED(hr))	return false;

		//	texture2d_descの取得
		ID3D11Texture2D* texture2d;
		hr = resource->QueryInterface(&texture2d);
		//		→データが読み込めなかったら return false;
		if (FAILED(hr))
		{
			resource->Release();
			find->shader_resource_view->Release();
			return false;
		}
		//	texture2d_desc保存
		texture2d->GetDesc(&find->texDesc);

		//	ポインタ変数 解放
		texture2d->Release();
		resource->Release();

		//	ファイル名のコピー
		wcscpy_s(find->filename, 256, filename);
	}

	//////////////////////////
	//	共通部
	//////////////////////////

	//	データを返す
	*srv = find->shader_resource_view;	//	ShaderResourceView保存
	*texDesc = &find->texDesc;				//	texture2d_desc保存

											//	保持数増加
	find->count++;


	return	true;
}

void resource_manager::releaseShaderResourceView(ID3D11ShaderResourceView* srv)
{
	if (!srv)			return;		//	検索対象Unknown　終了します

									//	Targetの検索開始
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ShaderResourceViewData* p = &srvData[n];

		//	未使用データなら、次のデータへ移行します
		if (p->count == 0)	continue;
		//	検索対象と不一致なら、次のデータへ移行します
		if (p->shader_resource_view != srv)	continue;

		//	Targetを発見 Releaseします
		p->Release();
		break;
	}
}


bool resource_manager::loadVertexShaders(ID3D11Device* device, const char* filename,D3D11_INPUT_ELEMENT_DESC* elementDescs, int numElement,ID3D11VertexShader** vs, ID3D11InputLayout** il)
{
	//	エラーチェック
	if (!device)		return false;
	if (!filename)		return false;
	if (*vs)			return false;
	if (*il)			return false;

	HRESULT hr;
	VertexShadersData* find = nullptr;
	int no = -1;

	//	対象のファイルが既に存在しているかを検索
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		VertexShadersData* p = &vsData[n];

		//	データの有無確認
		if (p->count == 0)
		{
			if (no == -1)	no = n;
			continue;
		}
		//	文字列比較
		if (strcmp(p->filename, filename) != 0)	continue;

		//	一致したから読み込んでる => 発見!
		find = p;
		//	ついでに作るものがあればここで
		//		→ VertexShadersの場合、特に無し
		break;
	}

	//	ファイルが見つからなかったら新規読み込み
	if (!find)
	{
		if (no == -1)	return	false;		//	空きが無い

		find = &vsData[no];
		//	新規読み込み
		BYTE* shader_data;
		long				size;
		LoadCSOFile(&shader_data, &size, filename);
		hr = device->CreateVertexShader(shader_data, size, nullptr, &find->vertex_shader);
		//		→データが読み込めなかったら return false;
		if (FAILED(hr))	return	false;

		hr = device->CreateInputLayout(elementDescs, numElement,shader_data, size, &find->input_layout);
		delete[] shader_data;
		//		→データが読み込めなかったら return false;
		if (FAILED(hr))
		{
			find->vertex_shader->Release();
			return false;
		}

		//	ファイル名のコピー
		strcpy_s(find->filename, 256, filename);
	}

	//////////////////////////
	//	共通部
	//////////////////////////

	//	共通して作るものがあればここで作る
	//		→VertexShadersで特にやる事は無し

	//	データを返す
	*vs = find->vertex_shader;
	*il = find->input_layout;

	//	保持数増加
	find->count++;

	return	true;
}

void resource_manager::releaseVertexShaders(ID3D11VertexShader* vs, ID3D11InputLayout* il)
{
	if (!vs)			return;		//	検索対象Unknown　終了します
	if (!il)			return;		//	検索対象Unknown　終了します

									//	Targetの検索開始
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		VertexShadersData* p = &vsData[n];

		//	未使用データなら、次のデータへ移行します
		if (p->count == 0)	continue;
		//	検索対象と不一致なら、次のデータへ移行します
		if (p->vertex_shader != vs)	continue;
		if (p->input_layout != il)	continue;

		//	Targetを発見 Releaseします
		p->Release();
		break;
	}
}



bool resource_manager::loadPixelShaders(ID3D11Device* device, const char* filename, ID3D11PixelShader** ps)
{
	//	エラーチェック
	if (!device)		return false;
	if (!filename)		return false;
	if (*ps)			return false;

	HRESULT hr;
	PixelShadersData* find = nullptr;
	int no = -1;

	//	対象のファイルが既に存在しているかを検索
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		PixelShadersData* p = &psData[n];

		//	データの有無確認
		if (p->count == 0)
		{
			if (no == -1)	no = n;
			continue;
		}
		//	文字列比較
		if (strcmp(p->filename, filename) != 0)	continue;

		//	一致したから読み込んでる => 発見!
		find = p;
		//	ついでに作るものがあればここで
		//		→ VertexShadersの場合、特に無し
		break;
	}

	//	ファイルが見つからなかったら新規読み込み
	if (!find)
	{
		if (no == -1)	return	false;		//	空きが無い

		find = &psData[no];
		//	新規読み込み
		BYTE* shader_data;
		long				size;
		LoadCSOFile(&shader_data, &size, filename);
		hr = device->CreatePixelShader(shader_data, size, nullptr, &find->pixel_shader);
		//		→データが読み込めなかったら return false;
		if (FAILED(hr))	return	false;

		//	ファイル名のコピー
		strcpy_s(find->filename, 256, filename);

		delete[]	shader_data;
	}

	//////////////////////////
	//	共通部
	//////////////////////////

	//	共通して作るものがあればここで作る
	//		→VertexShadersで特にやる事は無し

	//	データを返す
	*ps = find->pixel_shader;

	//	保持数増加
	find->count++;

	return	true;
}

void resource_manager::releasePixelShaders(ID3D11PixelShader* ps)
{
	if (!ps)			return;		//	検索対象Unknown　終了します

									//	Targetの検索開始
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		PixelShadersData* p = &psData[n];

		//	未使用データなら、次のデータへ移行します
		if (p->count == 0)	continue;
		//	検索対象と不一致なら、次のデータへ移行します
		if (p->pixel_shader != ps)	continue;

		//	Targetを発見 Releaseします
		p->Release();
		break;
	}
}




bool resource_manager::loadGeometryShaders(ID3D11Device* device, const char* filename, ID3D11GeometryShader** gs)
{
	//	エラーチェック
	if (!device)		return false;
	if (!filename)		return false;
	if (*gs)			return false;

	HRESULT hr;
	GeometryShadersData* find = nullptr;
	int no = -1;

	//	対象のファイルが既に存在しているかを検索
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		GeometryShadersData* p = &gsData[n];

		//	データの有無確認
		if (p->count == 0)
		{
			if (no == -1)	no = n;
			continue;
		}
		//	文字列比較
		if (strcmp(p->filename, filename) != 0)	continue;

		//	一致したから読み込んでる => 発見!
		find = p;
		//	ついでに作るものがあればここで
		//		→ VertexShadersの場合、特に無し
		break;
	}

	//	ファイルが見つからなかったら新規読み込み
	if (!find)
	{
		if (no == -1)	return	false;		//	空きが無い

		find = &gsData[no];
		//	新規読み込み
		BYTE* shader_data;
		long				size;
		LoadCSOFile(&shader_data, &size, filename);
		hr = device->CreateGeometryShader(shader_data, size, nullptr, &find->geometry_shader);
		//		→データが読み込めなかったら return false;
		if (FAILED(hr))	return	false;

		//	ファイル名のコピー
		strcpy_s(find->filename, 256, filename);

		delete[]	shader_data;
	}

	//////////////////////////
	//	共通部
	//////////////////////////

	//	共通して作るものがあればここで作る
	//		→VertexShadersで特にやる事は無し

	//	データを返す
	*gs = find->geometry_shader;

	//	保持数増加
	find->count++;

	return	true;
}

void resource_manager::releaseGeometryShaders(ID3D11GeometryShader* gs)
{
	if (!gs)			return;		//	検索対象Unknown　終了します

									//	Targetの検索開始
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		GeometryShadersData* p = &gsData[n];

		//	未使用データなら、次のデータへ移行します
		if (p->count == 0)	continue;
		//	検索対象と不一致なら、次のデータへ移行します
		if (p->geometry_shader != gs)	continue;

		//	Targetを発見 Releaseします
		p->Release();
		break;
	}
}




bool resource_manager::loadHullShaders(ID3D11Device* device, const char* filename, ID3D11HullShader** hs)
{
	//	エラーチェック
	if (!device)		return false;
	if (!filename)		return false;
	if (*hs)			return false;

	HRESULT hr;
	HullShadersData* find = nullptr;
	int no = -1;

	//	対象のファイルが既に存在しているかを検索
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		HullShadersData* p = &hsData[n];

		//	データの有無確認
		if (p->count == 0)
		{
			if (no == -1)	no = n;
			continue;
		}
		//	文字列比較
		if (strcmp(p->filename, filename) != 0)	continue;

		//	一致したから読み込んでる => 発見!
		find = p;
		//	ついでに作るものがあればここで
		//		→ VertexShadersの場合、特に無し
		break;
	}

	//	ファイルが見つからなかったら新規読み込み
	if (!find)
	{
		if (no == -1)	return	false;		//	空きが無い

		find = &hsData[no];
		//	新規読み込み
		BYTE* shader_data;
		long				size;
		LoadCSOFile(&shader_data, &size, filename);
		hr = device->CreateHullShader(shader_data, size, nullptr, &find->hull_shader);
		//		→データが読み込めなかったら return false;
		if (FAILED(hr))	return	false;

		//	ファイル名のコピー
		strcpy_s(find->filename, 256, filename);

		delete[]	shader_data;
	}

	//////////////////////////
	//	共通部
	//////////////////////////

	//	共通して作るものがあればここで作る
	//		→VertexShadersで特にやる事は無し

	//	データを返す
	*hs = find->hull_shader;

	//	保持数増加
	find->count++;

	return	true;
}

void resource_manager::releaseHullShaders(ID3D11HullShader* hs)
{
	if (!hs)			return;		//	検索対象Unknown　終了します

									//	Targetの検索開始
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		HullShadersData* p = &hsData[n];

		//	未使用データなら、次のデータへ移行します
		if (p->count == 0)	continue;
		//	検索対象と不一致なら、次のデータへ移行します
		if (p->hull_shader != hs)	continue;

		//	Targetを発見 Releaseします
		p->Release();
		break;
	}
}




bool resource_manager::loadDomainShaders(ID3D11Device* device, const char* filename, ID3D11DomainShader** ds)
{
	//	エラーチェック
	if (!device)		return false;
	if (!filename)		return false;
	if (*ds)			return false;

	HRESULT hr;
	DomainShadersData* find = nullptr;
	int no = -1;

	//	対象のファイルが既に存在しているかを検索
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		DomainShadersData* p = &dsData[n];

		//	データの有無確認
		if (p->count == 0)
		{
			if (no == -1)	no = n;
			continue;
		}
		//	文字列比較
		if (strcmp(p->filename, filename) != 0)	continue;

		//	一致したから読み込んでる => 発見!
		find = p;
		//	ついでに作るものがあればここで
		//		→ VertexShadersの場合、特に無し
		break;
	}

	//	ファイルが見つからなかったら新規読み込み
	if (!find)
	{
		if (no == -1)	return	false;		//	空きが無い

		find = &dsData[no];
		//	新規読み込み
		BYTE* shader_data;
		long				size;
		LoadCSOFile(&shader_data, &size, filename);
		hr = device->CreateDomainShader(shader_data, size, nullptr, &find->domain_shader);
		//		→データが読み込めなかったら return false;
		if (FAILED(hr))	return	false;

		//	ファイル名のコピー
		strcpy_s(find->filename, 256, filename);

		delete[]	shader_data;
	}

	//////////////////////////
	//	共通部
	//////////////////////////

	//	共通して作るものがあればここで作る
	//		→VertexShadersで特にやる事は無し

	//	データを返す
	*ds = find->domain_shader;

	//	保持数増加
	find->count++;

	return	true;
}

void resource_manager::releaseDomainShaders(ID3D11DomainShader* ds)
{
	if (!ds)			return;		//	検索対象Unknown　終了します

									//	Targetの検索開始
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		DomainShadersData* p = &dsData[n];

		//	未使用データなら、次のデータへ移行します
		if (p->count == 0)	continue;
		//	検索対象と不一致なら、次のデータへ移行します
		if (p->domain_shader != ds)	continue;

		//	Targetを発見 Releaseします
		p->Release();
		break;
	}
}




bool resource_manager::loadComputeShaders(ID3D11Device* device, const char* filename, ID3D11ComputeShader** cs)
{
	//	エラーチェック
	if (!device)		return false;
	if (!filename)		return false;
	if (*cs)			return false;

	HRESULT hr;
	ComputeShadersData* find = nullptr;
	int no = -1;

	//	対象のファイルが既に存在しているかを検索
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ComputeShadersData* p = &csData[n];

		//	データの有無確認
		if (p->count == 0)
		{
			if (no == -1)	no = n;
			continue;
		}
		//	文字列比較
		if (strcmp(p->filename, filename) != 0)	continue;

		//	一致したから読み込んでる => 発見!
		find = p;
		//	ついでに作るものがあればここで
		//		→ VertexShadersの場合、特に無し
		break;
	}

	//	ファイルが見つからなかったら新規読み込み
	if (!find)
	{
		if (no == -1)	return	false;		//	空きが無い

		find = &csData[no];
		//	新規読み込み
		BYTE* shader_data;
		long				size;
		LoadCSOFile(&shader_data, &size, filename);
		hr = device->CreateComputeShader(shader_data, size, nullptr, &find->compute_shader);
		//		→データが読み込めなかったら return false;
		if (FAILED(hr))	return	false;

		//	ファイル名のコピー
		strcpy_s(find->filename, 256, filename);

		delete[]	shader_data;
	}

	//////////////////////////
	//	共通部
	//////////////////////////

	//	共通して作るものがあればここで作る
	//		→VertexShadersで特にやる事は無し

	//	データを返す
	*cs = find->compute_shader;

	//	保持数増加
	find->count++;

	return	true;
}

void resource_manager::releaseComputeShaders(ID3D11ComputeShader* cs)
{
	if (!cs)			return;		//	検索対象Unknown　終了します

									//	Targetの検索開始
	for (int n = 0; n < RESOURCE_MAX; n++)
	{
		ComputeShadersData* p = &csData[n];

		//	未使用データなら、次のデータへ移行します
		if (p->count == 0)	continue;
		//	検索対象と不一致なら、次のデータへ移行します
		if (p->compute_shader != cs)	continue;

		//	Targetを発見 Releaseします
		p->Release();
		break;
	}
}


