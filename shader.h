#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include <memory>
#include "light.h"
//#include "frame_buffer.h"
//#include "texture.h"
//#include "sprite.h"

class Texture;
class sprite;
class frame_buffer;

//CalcBlurParam���g���ăp�����[�^�����
struct ConstantBufferForBlur
{
	DirectX::XMFLOAT4 Offset[16];
	DirectX::XMFLOAT4 SampleCount;
};

class shader
{
	//d3dcompiler.lib��ǉ��I�I
public:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vs = nullptr; // ���_�V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11PixelShader> ps = nullptr; // �s�N�Z���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> gs = nullptr; // �W�I���g���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11HullShader> hs = nullptr; //�@�n���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11DomainShader> ds = nullptr;//�@�h���C���l�[���V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> cs = nullptr;//�@�R���s���[�g�V�F�[�_

	Microsoft::WRL::ComPtr<ID3D11InputLayout> vertex_layout;

	//static�ɕύX5/25
	static HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);
	//HRESULT CompileCS(const wchar_t*  szFileName, const char*     szEntryPoint, const char*     szShaderModel, ID3DBlob**      ppBlobOut);

public:
	//shader() { /*ZeroMemory(this, sizeof(Shader));*/ }
	shader();
	virtual ~shader();


	//ComPtr������ƕ�����`�悵�悤�Ƃ���Ƃ΂���
	//ComPtr�������CB�������Ƀ��[�N���Ȃ�9/6
	//�R���X�^���g�o�b�t�@
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer;//phong�p
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBuffer_w;//World��matWVP�p
	Microsoft::WRL::ComPtr<ID3D11Buffer> ConstantBufferOriginal;//�e�X���g�����

	//�����֐�
	bool Create(const wchar_t* shader_name);//cso�ǂݍ���
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName);
	bool CreateForSkin(WCHAR* filename, LPCSTR VSName, LPCSTR PSName);
	bool CreateForTangent(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc);
	//bool Create(WCHAR* filename, LPCSTR Fucn, shader::SHADER_TYPE type);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR HSName, LPCSTR DSName);
	bool CreateForSkin(WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc, LPCSTR GSFunc);
	bool Create(WCHAR* filename, LPCSTR VSName, LPCSTR PSName, LPCSTR GSName, LPCSTR HSName, LPCSTR DSName);
	bool CreateConstantBuffer(u_int size);
	bool CreateConstantBufferOriginal(u_int size);
	//HRESULT CreateVS_POINT(wchar_t* srcFile, char* functionName);
	HRESULT CreatePS_POINT(wchar_t* srcFile, char* functionName);
	HRESULT CreateCS(wchar_t* srcFile, char* functionName);
	HRESULT CreateGS(wchar_t* srcFile, char* functionName);
	HRESULT CreateHS(wchar_t* srcFile, char* functionName);

	//���s�֐�
	void Activate();//Set�n
	void Deactivate();
	void RunComputeShader(UINT numViews, ID3D11ShaderResourceView** pSRVs, ID3D11Buffer* pCBCS, void* pCSData, DWORD numDataBytes, ID3D11UnorderedAccessView* pUAV, UINT x, UINT y, UINT z);

	//void CreateGPUParticleShader();
	//void UpdateGPUParticleShader();
	//void RenderGPUParticleShader(DirectX::XMFLOAT4X4 world, Camera* m_camera);

	//�V�F�[�_�[�Z�b�g
	void setVertexShader(ID3D11VertexShader* vs) { this->vs = vs; }
	void setPixelShader(ID3D11PixelShader* ps) { this->ps = ps; }

	//�V�F�[�_�[�Q�b�g
	ID3D11VertexShader* getVertexShader() { return vs.Get(); }
	ID3D11PixelShader* getPixelShader() { return ps.Get(); }

	//���
	void release();
};


//----------------------------------------------
//
//	���_�f�[�^�\���̒�`
//
//----------------------------------------------

struct VERTEX
{
	DirectX::XMFLOAT3 Pos;	//�ʒu
	DirectX::XMFLOAT3 Normal;//�@��
	DirectX::XMFLOAT2 Tex;	//UV���W
	DirectX::XMFLOAT4 Color;	//���_�F
};

//�R���X�^���g�o�b�t�@�\����

//OBJ�p��world�s��Ȃǂ̒萔�o�b�t�@
struct ConstantBufferWorld
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX matWVP;
	DirectX::XMFLOAT4 material_color;
};

//FBX�p��world�s��Ȃǂ̒萔�o�b�t�@
#define MAX_BONES 128
struct ConstantBufferWorld_s
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX matWVP;
	DirectX::XMFLOAT4X4 bone_transforms[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
};


//�R���X�^���g�o�b�t�@�\����
struct ConstantBufferForPerFrame
{
	DirectX::XMFLOAT4	LightColor;		//���C�g�̐F
	DirectX::XMFLOAT4	LightDir;		//���C�g�̕���
	DirectX::XMFLOAT4	AmbientColor;	//����
	DirectX::XMFLOAT4	EyePos;			//�J����
	DirectX::XMFLOAT4X4 LightViewProjection;
};

struct ConstantBufferForPhong
{
	DIRLIGHT dir_light;
	DirectX::XMFLOAT4	ambient_color;	//����
};

struct ConstantBufferForTessellation
{
	float tessFactor;       // �|���S���̃G�b�W�̃e�b�Z���[�V�����W��
	float insideTessFactor; // �|���S�������̃e�b�Z���[�V�����W��
	float depth;
	float dummy2;
};

//UV�X�N���[�����Ȃ��ꍇ��Timer=0.0f
struct ConstantBufferForNormal
{
	DirectX::XMFLOAT4 Timer;
	DirectX::XMFLOAT4 Color_par;
};

//struct ConstantBufferForToon
//{
//	DirectX::XMFLOAT4	LightColor;		//���C�g�̐F
//	DirectX::XMFLOAT4	LightDir;		//���C�g�̕���
//	DirectX::XMFLOAT4	AmbientColor;	//����
//	DirectX::XMFLOAT4	EyePos;			//�J����
//};

struct ConstantBufferForVoroNoise
{
	DirectX::XMFLOAT4 TimeX_SizeYZ;
};

struct ConstantBufferForFog
{
	DirectX::XMFLOAT4 fog_color;
};

struct ConstantBufferForConstantFog
{
	float	scale;
	float	attenuation;
	float	dummy1;
	float	dummy2;
};

struct ConstantBufferForHeightFog
{
	float	scale;
	float	attenuation;
	float	offset;
	float	height_weight_rate;
};

struct ConstantBufferForDepth
{
	DirectX::XMFLOAT4	LightColor;		//���C�g�̐F
	DirectX::XMFLOAT4	LightDir;		//���C�g�̕���
	DirectX::XMFLOAT4	AmbientColor;	//����
	DirectX::XMFLOAT4	EyePos;			//�J����
};

struct ConstantBufferForLinerDepth
{
	DirectX::XMFLOAT4X4 customProjection;
	float farZ;
	float nearZ;
	DirectX::XMFLOAT2 dummy;
};

struct ConstantBufferForShadowMap
{
	DirectX::XMFLOAT4X4 matCameraVP;
	DirectX::XMFLOAT4X4 matLightVP;
	DirectX::XMFLOAT4 color;
	float variance_pow;
	float d1;
	float d2;
	float d3;
};

//timer�Ɖ摜�̃T�C�Y����
struct ConstantBufferForWaver
{
	DirectX::XMFLOAT4 TimeX_SizeYZ;
	DirectX::XMFLOAT4 waverColor;
};

//�摜�̃T�C�Y���R���X�^���g�o�b�t�@�ő���
struct ConstantBufferForHightToNormal
{
	DirectX::XMFLOAT2 TexSize;
	DirectX::XMFLOAT2 dummy;
};

//�g
struct ConstantBufferForWave
{
	DirectX::XMFLOAT2 WaveCenter;
	DirectX::XMFLOAT2 TimeX_PowY;
};

//Light�n��shader�p�萔�o�b�t�@
struct ConstantBufferForLight
{
	POINTLIGHT  PointLight[Light::POINTMAX];//�|�C���g���C�g
	SPOTLIGHT  SpotLight[Light::SPOTMAX];	//�X�|�b�g���C�g
};

struct ConstantBufferForMultiTexture
{
	DirectX::XMFLOAT4	LightColor;		//���C�g�̐F
	DirectX::XMFLOAT4	LightDir;		//���C�g�̕���
	DirectX::XMFLOAT4	AmbientColor;	//����
	DirectX::XMFLOAT4	EyePos;			//�J����
	FLOAT  Mode;
	DirectX::XMFLOAT3 Dummy;
	enum MODE { OVER, MULTI };
};

struct ConstantBufferForSplatter
{
	DirectX::XMFLOAT4	LightColor;		//���C�g�̐F
	DirectX::XMFLOAT4	LightDir;		//���C�g�̕���
	DirectX::XMFLOAT4	AmbientColor;	//����
	DirectX::XMFLOAT4	EyePos;			//�J����
};

struct ConstantBufferForDissolve
{
	float DissolveThreshold; //����臒l
	float DissolveEmissiveWidth; //����臒l(�f�B�]���u�E�G�~�b�V�u)
	float Dummy00;
	float Dummy01;
};

struct ConstantBufferForVelocityMap
{
	DirectX::XMFLOAT4X4 NewMatrix;
	DirectX::XMFLOAT4X4 OldMatrix;
	DirectX::XMFLOAT4X4 ROnlyMatrix;
	DirectX::XMFLOAT4X4 OldBoneTransforms[MAX_BONES];
};

struct ConstantBufferForMotionBlur
{
	FLOAT Power;
	DirectX::XMFLOAT3 Color;
};

struct ConstantBufferForGaussianBlur
{
	float Weights[8];
	DirectX::XMFLOAT2 Offset;
	DirectX::XMFLOAT2 Size;
};

struct ConstantBufferForIrradiation
{
	DirectX::XMFLOAT4 Offset[16];
};

struct ConstantBufferForBrightnessExtraction
{
	float threshold;
	DirectX::XMFLOAT3 dummy;
};

struct ConstantBufferForFur
{
	float	density;//�т̖��x
	float	distance;//�т̒���
	float	dummy1;
	float	dummy2;
};

//���̃p�����[�^�[�͂R��̃e�N�X�`���t�F�b�`�̃R�X�g�y���ׂ̈ɐ��ݏo���ꂽ�A�g�[���}�b�v�Ȑ��̋ߎ��l���o�����߂̐��l
struct ConstantBufferForToneMap
{
	float	paramA;
	float	paramB;
	float	paramCB;
	float	paramDE;
	float	paramDF;
	float	paramEperF;
	float	paramF_White;
	float	Exposure;
	int		toneType;
	int		isGamma;
	int		dummy[2];
};

struct dLights
{
	DirectX::XMFLOAT4 direction;
	DirectX::XMFLOAT4 color;
	int is_visible;
	DirectX::XMFLOAT3 dummy;
};

struct pLights
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 color;
	float distance;
	float decay;
	int is_visible;
	float dummy;
};

struct ConstantBufferForPBR
{
	DirectX::XMFLOAT4 albedo;
	DirectX::XMFLOAT4 reflected_diffuse;
	DirectX::XMFLOAT4 reflected_specular;
	dLights d_lights[4];
	pLights p_lights[4];
	//DirectX::XMFLOAT4X4 view_matrix;
	//DirectX::XMFLOAT4X4 projection_matrix;
	float metallic;
	float roughness;
	int tex_flg;
	int diffuse_brdf_type;
};

struct ConstantBufferForPBRv2
{
	DirectX::XMFLOAT4X4 vpm;//view_projection
	DirectX::XMFLOAT4X4 spm;//sky_projection
	DirectX::XMFLOAT4X4 srm;//scene_rotation
};


struct ConstantBufferForPBREX
{
	float metalness;
	float roughness;
	DirectX::XMFLOAT2 dummy;
};

struct ConstantBufferForEnvironmentMap
{
	DirectX::XMFLOAT4 albedo;
	DirectX::XMFLOAT4 reflected_diffuse;
	DirectX::XMFLOAT4 reflected_specular;
	//DirectX::XMFLOAT4 eyepos;
	dLights d_lights[4];
	pLights p_lights[4];
	//DirectX::XMFLOAT4X4 view_matrix;
	//DirectX::XMFLOAT4X4 projection_matrix;
	float metallic;
	float roughness;
	int tex_flg;
	int diffuse_brdf_type;
};

struct ConstantBufferForSSAO
{
	//hemiradius 0.2f, Zfar(100), Aopower 10.0f
	//float4x4 wvp;
	float hemi_radius;
	float bias;
	int kernel_size;
	float dummy;
	DirectX::XMFLOAT4 kernel[64];
	DirectX::XMFLOAT4 kernel_noise[16];
};

struct ConstantBufferForSSR
{
	float raytrace_max_length;
	float raytrace_max_thickness;
	float reflection_enhancer;
	int timer;
};

struct cb_ray
{
	DirectX::XMFLOAT4 origin;
	DirectX::XMFLOAT4 direction;
};
struct cb_sphere
{
	DirectX::XMFLOAT4 center;
	float radius;
	DirectX::XMFLOAT3 light_color;
	DirectX::XMFLOAT3 rho;
	float dummy;
	int material_type;
	DirectX::XMINT3 dummy2;
};

const int SPHERE_MAX = 7;
struct ConstantBufferForRayTracing
{
	enum MATERIAL_TYPE { diffuse, mirror };
	cb_ray _ray;
	cb_sphere _sphere[SPHERE_MAX];
	DIRLIGHT dir_light;
};

struct ConstantBufferForFieldRaymarching
{
	DirectX::XMFLOAT4 plane;
	DirectX::XMFLOAT4 camera_pos;
	DirectX::XMFLOAT4 light_dir;
	DirectX::XMFLOAT4 sky;
	float height_factor;
	float sampling;
	float dummy2;
	float dummy3;
};

struct ConstantBufferForSeaRaymarching
{
	DirectX::XMFLOAT3 sea_base;
	int timer;
	DirectX::XMFLOAT3 ray_origin;
	float sea_height;
	DirectX::XMFLOAT3 sea_water_color;
	float sea_freq;
	float choppy;
	float speed;
	float dummy;
	float dummy2;
};

struct ConstantBufferForCloudRaymarching
{
	int timer;
	float camera_distance;
	float target_depth;
	float transmittance;
};

struct ConstantBufferForSkyRaymarching
{
	int timer;
	DirectX::XMFLOAT2 sun_pos;
	float dummy;
};

struct ConstantBufferForBoxVolumeLight
{
	DirectX::XMFLOAT3 light_color;
	int timer;
};

struct ConstantBufferForBoxVolumeLightSphere
{
	DirectX::XMFLOAT3 sphere_pos;
	int timer;
	DirectX::XMFLOAT3 cam_pos;
	float obj_dist;
	DirectX::XMFLOAT3 light_color;
	int dummy;
};

struct ConstantBufferForPathTracing1
{
	DirectX::XMFLOAT3 cam_pos;
	float random_modulator;
	DirectX::XMFLOAT3 cam_look;
	float width;
	float height;
	int iteration;
	int surf_type;
	float dummy;
	DirectX::XMFLOAT3 color;
	float ior;//����
	DirectX::XMFLOAT3 emit_color;//�����F
	float gloss;//����
};

struct ConstantBufferForDoF
{
	float focus;
	float dummy;
	float dummy2;
	float dummy3;
};

struct ConstantBufferForLensGhost
{
	int   timer;
	DirectX::XMFLOAT2 sun_pos;
	float dummy;
};

bool CreateConstantBuffer(ID3D11Buffer**ppCB, u_int size);
HRESULT CreateStructuredBuffer(ID3D11Device* pDevice, UINT elementSize, UINT count, void* pInitData, ID3D11Buffer**  ppBufferOut);
HRESULT CreateSRVBuffer(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11ShaderResourceView** ppSRVOut);
HRESULT CreateRawBuffer(ID3D11Device* pDevice, UINT size, void* pInitData, ID3D11Buffer** ppBufferOut);
HRESULT CreateUAVBuffer(ID3D11Device* pDevice, ID3D11Buffer* pBuffer, ID3D11UnorderedAccessView** ppUAVOut);
ID3D11Buffer* CreateAndCopyToBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pd3dImmediateContext, ID3D11Buffer* pBuffer);

//�萔�o�b�t�@�X�V
template <typename T>
void UpdateConstantBuffer(shader* shader, T cb, UINT slot_num = 2);

void UpdateConstantBuffer(shader* shader, ConstantBufferForPhong cb, UINT slot_num = 2);
void UpdateConstantBuffer(shader* shader, ConstantBufferForDepth cb, UINT slot_num = 2);

//�u���[�̃p�����[�^�[�v�Z
float GaussianDistribution(DirectX::XMFLOAT2 &pos, float rho);
ConstantBufferForBlur CalcBlurParam(int width, int height, DirectX::XMFLOAT2 dir, float deviation);
void CalcGaussWeights(float dispersion, float *weights, int weight_num);
void CreateDownSampleTextures(std::unique_ptr<Texture> texes[], std::unique_ptr<Texture> temp[], int size);
void DownSamplingAndBlur(std::unique_ptr<Texture> texes[], std::unique_ptr<Texture> temp[], Texture* src, sprite *spr, std::unique_ptr<shader> *blur,frame_buffer* f_buffer, ConstantBufferForGaussianBlur cbgb, int size);
void ReleaseDownSampleTextures(Texture* texes[]);

//���[�V�����u���[�p�s��ۑ�
struct MotionBlurMatrix
{
	DirectX::XMFLOAT4X4 NewMatWorld;
	DirectX::XMFLOAT4X4 NewMatView;
	DirectX::XMFLOAT4X4 NewMatProjection;
	DirectX::XMFLOAT4X4 OldMatWorld;
	DirectX::XMFLOAT4X4 OldMatView;
	DirectX::XMFLOAT4X4 OldMatProjection;
	DirectX::XMFLOAT4X4 ROnlyMat;
	DirectX::XMFLOAT4X4 NewBoneTransforms[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
	DirectX::XMFLOAT4X4 OldBoneTransforms[MAX_BONES] = { { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 } };
};

void setOldMBMatrix(MotionBlurMatrix &mbm);