#include "mouse.h"

bool mouse::InitDInput(HINSTANCE hInstApp, HWND hWnd)
{
	HRESULT ret = DirectInput8Create(hInstApp, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&g_pDInput, NULL);
	if (FAILED(ret)) {
		return false;	// 作成に失敗
	}
	return true;
}

bool mouse::ReleaseDInput(void)
{
	// DirectInputのデバイスを開放
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

	// マウス用にデバイスオブジェクトを作成
	ret = g_pDInput->CreateDevice(GUID_SysMouse, &g_pDIMouse, NULL);
	if (FAILED(ret)) {
		// デバイスの作成に失敗
		return false;
	}

	// データフォーマットを設定
	ret = g_pDIMouse->SetDataFormat(&c_dfDIMouse);	// マウス用のデータ・フォーマットを設定
	if (FAILED(ret)) {
		// データフォーマットに失敗
		return false;
	}

	// モードを設定（フォアグラウンド＆非排他モード）
	ret = g_pDIMouse->SetCooperativeLevel(hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);
	if (FAILED(ret)) {
		// モードの設定に失敗
		return false;
	}

	// デバイスの設定
	DIPROPDWORD diprop;
	diprop.diph.dwSize = sizeof(diprop);
	diprop.diph.dwHeaderSize = sizeof(diprop.diph);
	diprop.diph.dwObj = 0;
	diprop.diph.dwHow = DIPH_DEVICE;
	diprop.dwData = DIPROPAXISMODE_REL;	// 相対値モードで設定（絶対値はDIPROPAXISMODE_ABS）

	ret = g_pDIMouse->SetProperty(DIPROP_AXISMODE, &diprop.diph);
	if (FAILED(ret)) {
		// デバイスの設定に失敗
		return false;
	}

	// 入力制御開始
	g_pDIMouse->Acquire();

	return true;
}
//-----------------------------------------------------------------------------
// DirectInputのマウスデバイス用の解放処理
//-----------------------------------------------------------------------------
bool mouse::ReleaseDInputMouse()
{
	// DirectInputのデバイスを開放
	if (g_pDIMouse) {
		g_pDIMouse->Release();
		g_pDIMouse = NULL;
	}

	return true;
}

void mouse::GetMouseState(HWND hWnd)
{
	if (g_pDIMouse == NULL) {
		// オブジェクト生成前に呼ばれたときはここで生成させる
		InitDInputMouse(hWnd);
	}

	// 読取前の値を保持します
	//DIMOUSESTATE g_zdiMouseState_bak;	// マウス情報(変化検知用)
	//memcpy(&g_zdiMouseState_bak, &g_zdiMouseState, sizeof(g_zdiMouseState_bak));

	// マウスの状態を取得します
	HRESULT	hr = g_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &g_zdiMouseState);
	if (hr == DIERR_INPUTLOST) {
		g_pDIMouse->Acquire();
		hr = g_pDIMouse->GetDeviceState(sizeof(DIMOUSESTATE), &g_zdiMouseState);
	}

	//if (memcmp(&g_zdiMouseState_bak, &g_zdiMouseState, sizeof(g_zdiMouseState_bak)) != 0) {
	//	// 確認用の処理、ここから -->
	//	// 値が変わったら表示します
	//	char buf[128];
	//	wsprintf(buf, "(%5d, %5d, %5d) %s %s %s\n",
	//		g_zdiMouseState.lX, g_zdiMouseState.lY, g_zdiMouseState.lZ,
	//		(g_zdiMouseState.rgbButtons[0] & 0x80) ? "Left" : "--",
	//		(g_zdiMouseState.rgbButtons[1] & 0x80) ? "Right" : "--",
	//		(g_zdiMouseState.rgbButtons[2] & 0x80) ? "Center" : "--");
	//	OutputDebugString(buf);
	//	// --> ここまで、確認用の処理
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