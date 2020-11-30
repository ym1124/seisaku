#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "shader.h"

class my_mesh;
class static_object;

static const UINT NUM_ELEMENTS = 20000;

struct particle_data
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT2 size;
	float timer;
};

class gpu_particle
{
public:
//	static const UINT NUM_ELEMENTS = 10240;

	//struct vertex
	//{
	//	DirectX::XMFLOAT3 pos;	//位置
	//	DirectX::XMFLOAT3 normal;//法線
	//	DirectX::XMFLOAT2 tex;	//UV座標
	//	DirectX::XMFLOAT4 color;	//頂点色
	//};

	ID3D11ShaderResourceView *srv = nullptr;//構造化バッファをSRVとしてバインド//計算用データはテクスチャとしてシェーダーに渡されるのでSRVにする
	//ID3D11ShaderResourceView* srv = nullptr;
	std::shared_ptr<Texture> tex;

	gpu_particle();
	~gpu_particle();
	void create(wchar_t* srcFile, char* functionName, wchar_t* texture_filename);
	void create(wchar_t* srcFile, char* functionName);
	void update();
	void render(DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(1.0f,1.0f,1.0f));
	void release();
	HRESULT createStructuredBuffer(ID3D11Device* pDevice, UINT elementSize, UINT count, void* pInitData, ID3D11Buffer**  ppBufferOut);
	HRESULT createSRVBuffer(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut);
	HRESULT createRawBuffer(ID3D11Device* pDevice, UINT size, void* pInitData, ID3D11Buffer** ppBufferOut);
	HRESULT createUAVBuffer(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut);
	ID3D11Buffer* createAndCopyToBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer);

	void createStructureBuffers();
	void createSRVBuffers();
	void createUAVBuffers();
	void runComputeShader(/*ID3D11DeviceContext* pContext,*//*ID3D11ComputeShader* pComputeShader,*/UINT numViews, ID3D11ShaderResourceView**  pSRVs, ID3D11Buffer* pCBCS, void* pCSData, DWORD numDataBytes, ID3D11UnorderedAccessView*  pUAV, UINT x, UINT y, UINT z);

	enum PATTERN { RADIAL, SNOW, TORNADO, FIREWORK, MODEL,STATIC_MODEL }; 
	void setInitData(int pattern = PATTERN::RADIAL, DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0, 0, 0), DirectX::XMFLOAT2 size = DirectX::XMFLOAT2(0.1f, 0.1f),float range = 10.0f, float timer = 100.0f);
	void setInitData(my_mesh *model, DirectX::XMFLOAT2 size = DirectX::XMFLOAT2(0.1f, 0.1f), float range = 10.0f);
	void setInitData(static_object *model, DirectX::XMFLOAT2 size = DirectX::XMFLOAT2(0.1f, 0.1f), float range = 10.0f);
	void resetInitPosition(DirectX::XMFLOAT3 init_pos);
	DirectX::XMFLOAT3 resetInitPosition(my_mesh *model);
	void setModel(static_object *model) { s_obj = model; }

	particle_data* pd = nullptr;
	particle_data* temp = nullptr;
	//particle_data pd[NUM_ELEMENTS];//計算用バッファ

	DirectX::XMFLOAT3 getInitPos() { return init_pos; }
	ID3D11UnorderedAccessView* getUAVBuffer() { return uav_buffer; };//順不同アクセスビュー//書きだし先として割り込むことが出来る
	ID3D11Buffer* getResultBuffer() { return result_buffer; };


private:
	DirectX::XMFLOAT3 init_pos;
	int type;
	float range;
	float timer;
	my_mesh *mdl;
	static_object *s_obj;

	std::unique_ptr<shader> _shader;
	ID3D11RasterizerState* rasterizer_states;
	ID3D11DepthStencilState* depth_stencil_state;

	ID3D11Buffer* vertex_buffer = nullptr;
	ID3D11Buffer* constant_buffer = nullptr;

	ID3D11Buffer* buffer = nullptr;//構造化バッファ//Texture2D的な感じでここからSRV作る
	ID3D11Buffer* result_buffer = nullptr;//結果格納用

	ID3D11UnorderedAccessView* uav_buffer = nullptr;

	ID3D11Buffer* cs_buffer;

	struct cbuffer
	{
		DirectX::XMFLOAT4 init_pos;
		DirectX::XMFLOAT4 color;
	};

};

class firework_emitter
{

	std::unique_ptr<gpu_particle> gp[10];
public:
	firework_emitter();
	~firework_emitter() {};

	void init(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 size, int range);
	void update();
	void render();
};