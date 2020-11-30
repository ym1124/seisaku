#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

#define DIRECTINPUT_VERSION		0x0800		// DirectInput�̃o�[�W�������
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

class mouse
{
public:
	LPDIRECTINPUT8       g_pDInput = NULL;	// DirectInput�I�u�W�F�N�g
	LPDIRECTINPUTDEVICE8 g_pDIMouse = NULL;	// �}�E�X�f�o�C�X
	DIMOUSESTATE g_zdiMouseState;			// �}�E�X���

	enum STATE{NO,TRIGGER,HOLD,RELEASE};
	bool old_right = false;
	int r_state = STATE::NO;

	DirectX::XMFLOAT3 mouse_movement = { 0,0,0 };

	bool InitDInput(HINSTANCE hInstApp, HWND hWnd);
	bool ReleaseDInput(void);
	bool InitDInputMouse(HWND hWnd);
	bool ReleaseDInputMouse();
	void GetMouseState(HWND hWnd);
	int isRightButton();
	DirectX::XMFLOAT3 getMousePosition();
	DirectX::XMFLOAT3 getMouseMovement();
	void resetMouseButtonState();
};