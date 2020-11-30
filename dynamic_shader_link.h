#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <wrl.h>

class BASE_SHADING
{
private:
	// ���I�V�F�[�_�[�����P�[�W( �T���v���ł̓s�N�Z���V�F�[�_�[�̂� )�p�̒萔�o�b�t�@��`
	// ���̕��я��̓V�F�[�_�[�\�[�X���Ő錾���Ă���萔�o�b�t�@�̕��я��Ɠ������Ȃ�悤�ɂ��邱��
	typedef struct _CBUFFER0
	{
		//�����o�[�g
		DirectX::XMFLOAT4 vecLight0;

		// �n�[�t�����o�[�g�p
		DirectX::XMFLOAT4 vecLight1;

		// �t�H���V�F�[�f�B���O�p
		DirectX::XMFLOAT4 vecLight2;
		DirectX::XMFLOAT4 EyePos;

		// �V�F�[�f�B���O�p
		//DirectX::XMFLOAT4 vecLight3;
	}CBUFFER0;

	// ���_�V�F�[�_�[�p�̒萔�o�b�t�@��`
	typedef struct _CBUFFER1
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX  matWVP;
	}CBUFFER1;

	// ���̓��C�A�E�g
	ID3D11InputLayout* m_pLayout;

	// �萔�o�b�t�@// [0]�Ƀ��C�e�B���O�p[1]��VS�p�萔
	ID3D11Buffer* m_pConstantBuffers[2];


	// ���_�V�F�[�_�[
	ID3D11VertexShader* m_pVertexShader;

	// �s�N�Z���V�F�[�_�[
	ID3D11PixelShader* m_pPixelShader;

	// ���I�V�F�[�_�[�����N
	// ID3D11ClassLinkage
	ID3D11ClassLinkage* m_pPSClassLinkage;

	// �s�N�Z���V�F�[�_�[�ɐݒ莞�Ɏg�p����N���X�C���X�^���X
	// ID3D11ClassInstance
	ID3D11ClassInstance* m_pClassInstance;

public:

	BASE_SHADING();
	~BASE_SHADING();//�������

	// ������
	//HRESULT Init(ID3D11Device* pD3DDevice, const BYTE* pVS, size_t VSSize, const BYTE* pPS, size_t PSSize);
	HRESULT Init(ID3D11Device* pD3DDevice, WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc);

	// ���_�V�F�[�_�[�p�̒萔�o�b�t�@��ݒ肷��
	HRESULT SetCBVertexShader(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMMATRIX* World, DirectX::XMMATRIX* p_matWVP);
	//�����o�[�g�p
	HRESULT SetCBLambert(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight);
	// �n�[�t�����o�[�g�p�̒萔�o�b�t�@��ݒ�
	HRESULT SetCBHalfLambert(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight);
	// �t�H���V�F�[�f�B���O�p�̒萔�o�b�t�@��ݒ�
	HRESULT SetCBPhongShading(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLigh, DirectX::XMFLOAT4* p_EyePos);
	// �m�[�V�F�[�f�B���O�p�̒萔�o�b�t�@��ݒ�
	HRESULT SetCBNoShading(ID3D11DeviceContext* pD3DDeviceContext/*, DirectX::XMFLOAT4* p_vecLight*/);
	// �`��J�n����
	HRESULT Begin(ID3D11DeviceContext* pD3DDeviceContext, LPCSTR pClassInstanceName);
	HRESULT BeginNM(ID3D11DeviceContext* pD3DDeviceContext, LPCSTR pClassTypeName);
	//getter
	ID3D11Buffer** getConstantBufferAddress(int num) { return &m_pConstantBuffers[num]; }
	// �`��I������
	void End();
};
