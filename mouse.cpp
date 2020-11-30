#include "mouse.h"

bool mouse::InitDInput(HINSTANCE hInstApp, HWND hWnd)
{
	HRESULT ret = DirectInput8Create(hInstApp, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pDInput, NULL);
	if (FAILED(ret)) {
		return false;	// �쐬�Ɏ��s
	}
	return true;
}

bool mouse::ReleaseDInput(void)
{
	// DirectInput�̃f�o�C�X���J��
	if (g_pDInput) {
		g_pDInput->Release();
		g_pDInput = NULL;
	}

	return true;
}

bool mouse::InitDInputMouse(HWND hWnd)
{
	HRESULT ret = S_FALSE;
	if (g_pDInput == NULL) {
		return false;
	}

	// �}�E�X�p�Ƀf�o�C�X�I�u�W�F�N�g���쐬
	ret = g_pDInput->CreateDevice(GUID_SysMouse, &g_pDIMouse, NULL);
	if (FAILED(ret)) {
		// �f�o�C�X�̍쐬�Ɏ��s
		return false;
	}

	// �f�[�^�t�H�[�}�b�g��ݒ�
	ret = g_pDIMouse->SetDataFormat(&c_dfDIMouse);	// �}�E�X�p�̃f�[�^�E�t�H�[�}�b�g��ݒ�
	if (FAILED(ret)) {
		// �f�[�^�t�H�[�}�b�g�Ɏ��s
		return false;
	}

	// ���[�h��ݒ�i�t�H�A�O���E���h����r�����[�h�j
	ret = g_pDIMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(ret)) {
		// ���[�h�̐ݒ�Ɏ��s
		return false;
	}

	// �f�o�C�X�̐ݒ�
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;	// ���Βl���[�h�Őݒ�i��Βl��DIPROPAXISMODE_ABS�j

	ret = g_pDIMouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	if (FAILED(ret)) {
		// �f�o�C�X�̐ݒ�Ɏ��s
		return false;
	}

	// ���͐���J�n
	g_pDIMouse->Acquire();

	return true;
}
//-----------------------------------------------------------------------------
// DirectInput�̃}�E�X�f�o�C�X�p�̉������
//-----------------------------------------------------------------------------
bool mouse::ReleaseDInputMouse()
{
	// DirectInput�̃f�o�C�X���J��
	if (g_pDIMouse) {
		g_pDIMouse->Release();
		g_pDIMouse = NULL;
	}

	return true;
}

void mouse::GetMouseState(HWND hWnd)
{
	if (g_pDIMouse == NULL) {
		// �I�u�W�F�N�g�����O�ɌĂ΂ꂽ�Ƃ��͂����Ő���������
		InitDInputMouse(hWnd);
	}

	// �ǎ�O�̒l��ێ����܂�
	//DIMOUSESTATE g_zdiMouseState_bak;	// �}�E�X���(�ω����m�p)
	//memcpy(&g_zdiMouseState_bak, &g_zdiMouseState, sizeof(g_zdiMouseState_bak));

	// �}�E�X�̏�Ԃ��擾���܂�
	HRESULT	hr = g_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &g_zdiMouseState);
	if (hr == DIERR_INPUTLOST) {
		g_pDIMouse->Acquire();
		hr = g_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &g_zdiMouseState);
	}

	//if (memcmp(&g_zdiMouseState_bak, &g_zdiMouseState, sizeof(g_zdiMouseState_bak)) != 0) {
	//	// �m�F�p�̏����A�������� -->
	//	// �l���ς������\�����܂�
	//	char buf[128];
	//	wsprintf(buf, "(%5d, %5d, %5d) %s %s %s\n",
	//		g_zdiMouseState.lX, g_zdiMouseState.lY, g_zdiMouseState.lZ,
	//		(g_zdiMouseState.rgbButtons[0] & 0x80) ? "Left" : "--",
	//		(g_zdiMouseState.rgbButtons[1] & 0x80) ? "Right" : "--",
	//		(g_zdiMouseState.rgbButtons[2] & 0x80) ? "Center" : "--");
	//	OutputDebugString(buf);
	//	// --> �����܂ŁA�m�F�p�̏���
	//}
}
int mouse::isRightButton()
{
	//if (g_zdiMouseState.rgbButtons[1] &0x80)
	//	return true;
	//else
	//	return false;

	//if ((GetAsyncKeyState(VK_RBUTTON) & 1 )&& old_right)
	//{
	//	r_state = STATE::HOLD;
	//	old_right = true;
	//}
	//else if((GetAsyncKeyState(VK_RBUTTON) &1) && !old_right)
	//{
	//	r_state = STATE::TRIGGER;
	//	old_right = true;
	//}
	//else
	//{
	//	r_state = STATE::NO;
	//	old_right = false;
	//}

	//if (GetAsyncKeyState(VK_RBUTTON) & 1)
	//{
	//		r_state = STATE::HOLD;
	//		old_right = true;
	//}

	return r_state;
}

DirectX::XMFLOAT3 mouse::getMousePosition()
{
	DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(g_zdiMouseState.lX, g_zdiMouseState.lY, g_zdiMouseState.lZ);
	return pos;
}

DirectX::XMFLOAT3 mouse::getMouseMovement()
{
	mouse_movement = DirectX::XMFLOAT3(mouse_movement.x+getMousePosition().x, mouse_movement.y + getMousePosition().y, mouse_movement.z + getMousePosition().z);
	return mouse_movement;
}

void mouse::resetMouseButtonState()
{
	for (int i = 0; i < 3; i++)
		g_zdiMouseState.rgbButtons[i] = 0;
}