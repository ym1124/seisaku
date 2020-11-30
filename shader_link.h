#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>
#include "light.h"

class BASE_SHADING
{
private:
	// ���I�V�F�[�_�[�����P�[�W( �T���v���ł̓s�N�Z���V�F�[�_�[�̂� )�p�̒萔�o�b�t�@��`
	// ���̕��я��̓V�F�[�_�[�\�[�X���Ő錾���Ă���萔�o�b�t�@�̕��я��Ɠ������Ȃ�悤�ɂ��邱��
	typedef struct _CBUFFER0
	{
		// �n�[�t�����o�[�g�p
		DirectX::XMFLOAT4 vecLight0;

		// �t�H���V�F�[�f�B���O�p
		DirectX::XMFLOAT4 vecLight1;
		DirectX::XMFLOAT4 EyePos;

		// �Z���V�F�[�f�B���O�p
		DirectX::XMFLOAT4 vecLight2;
	}CBUFFER0;

	// ���_�V�F�[�_�[�p�̒萔�o�b�t�@��`
	typedef struct _CBUFFER1
	{
		// ���[���h�r���[�v���W�F�N�V�����s��
		DirectX::XMMATRIX  matWVP;
	}CBUFFER1;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		VS = nullptr; // ���_�V�F�[�_
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		PS = nullptr; // �s�N�Z���V�F�[�_

	// ���̓��C�A�E�g
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		VertexLayout;

	// �萔�o�b�t�@
	ID3D11Buffer* m_pConstantBuffers[2];

	// ���I�V�F�[�_�[�����N
	// ID3D11ClassLinkage
	ID3D11ClassLinkage* m_pPSClassLinkage;

	// �s�N�Z���V�F�[�_�[�ɐݒ莞�Ɏg�p����N���X�C���X�^���X
	// ID3D11ClassInstance
	ID3D11ClassInstance* m_pClassInstance;

public:

	BASE_SHADING();
	~BASE_SHADING();

	HRESULT Compile(WCHAR* filename, LPCSTR method, LPCSTR shaderModel, ID3DBlob** ppBlobOut);

	// ������
	HRESULT Init(ID3D11Device* pD3DDevice, WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc);
	/*HRESULT Init(ID3D11Device* pD3DDevice, WCHAR* filename, LPCSTR VSFunc, LPCSTR PSFunc);*/

	// ���_�V�F�[�_�[�p�̒萔�o�b�t�@��ݒ肷��
	HRESULT SetCBVertexShader(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMMATRIX* p_matWVP);
	// �n�[�t�����o�[�g�p�̒萔�o�b�t�@��ݒ�
	HRESULT SetCBHalfLambert(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight, ID3D11ShaderResourceView* pDecalMap);
	// �t�H���V�F�[�f�B���O�p�̒萔�o�b�t�@��ݒ�
	HRESULT SetCBPhongShading(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight, DirectX::XMFLOAT4* p_EyePos, ID3D11ShaderResourceView* pDecalMap);
	// �Z���V�F�[�f�B���O�p�̒萔�o�b�t�@��ݒ�
	HRESULT SetCBCelShading(ID3D11DeviceContext* pD3DDeviceContext, DirectX::XMFLOAT4* p_vecLight, ID3D11ShaderResourceView* pDecalMap, ID3D11ShaderResourceView* pCelMap);
	// �`��J�n����
	HRESULT Begin(ID3D11DeviceContext* pD3DDeviceContext, LPCSTR pClassInstanceName);
	// �`��I������
	void End();
};