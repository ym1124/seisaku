#include "mouse_input.h"
#include "framework.h"

#include <math.h>

//マウス関連
Mouse mouse;
UINT Mouse::GetState(UINT msg)
{
	switch (msg) {
	case WM_LBUTTONDOWN:
		mouse.SetBottonDownFlg(Mouse::Botton::LEFT);
		break;

	case WM_LBUTTONUP:
		mouse.SetBottonUpFlg(Mouse::Botton::LEFT);
		break;

	case WM_RBUTTONDOWN:
		mouse.SetBottonDownFlg(Mouse::Botton::RIGHT);
		break;

	case WM_RBUTTONUP:
		mouse.SetBottonUpFlg(Mouse::Botton::RIGHT);
		break;

	case WM_MBUTTONDOWN:
		mouse.SetBottonDownFlg(Mouse::Botton::MIDDLE);

		break;

	case WM_MBUTTONUP:
		mouse.SetBottonUpFlg(Mouse::Botton::MIDDLE);
		break;
	}

	return NULL;
}

DirectX::XMFLOAT2 Mouse::GetPos()
{
	POINT p;
	GetCursorPos(&p);
	ScreenToClient(FRAMEWORK.getHWND(), &p);

	return DirectX::XMFLOAT2((float)p.x, (float)p.y);
}

void Mouse::SetBottonDownFlg(const Botton botton)
{
	mouse.SetIsPush(botton, true);
	mouse.SetIsRelease(botton, false);

	mouse.SetIsPushTrigger(botton, true);
	mouse.SetIsReleaseTrigger(botton, false);
}

void Mouse::SetBottonUpFlg(const Botton botton)
{
	mouse.SetIsPush(botton, false);
	mouse.SetIsRelease(botton, true);

	mouse.SetIsPushTrigger(botton, false);
	mouse.SetIsReleaseTrigger(botton, true);
}

bool Mouse::Push(const Botton botton)
{
	if (is_push[botton]) return true;

	return false;
}

bool Mouse::PushTrigger(const Botton botton)
{
	if (is_push_trigger[botton]) {
		is_push_trigger[botton] = false;
		return true;
	}

	return false;
}

bool Mouse::ReleaseTrigger(const Botton botton)
{
	if (is_release_trigger[botton]) {
		is_release_trigger[botton] = false;
		return true;
	}

	return false;
}

bool Mouse::None(const Botton botton)
{
	if (!is_push[botton]) return true;

	return false;
}



//キーボード関連
KeyBoard kb;

void KeyBoard::GetKeyState()
{
	GetKeyboardState(key);
}

void KeyBoard::GetState()
{
	for (int i = 0; i < keyMax; i++) {
		kb.key_prev[i] = kb.key[i];
	}

	kb.GetKeyState();
}

bool KeyBoard::Push(const int _key)
{
	if (key[_key] & 0x80) return true;

	return false;
}

bool KeyBoard::PushTrigger(const int _key)
{
	if ((key[_key] & 0x80) && !(kb.key_prev[_key] & 0x80)) return true;

	return false;
}

bool KeyBoard::ReleaseTrigger(const int _key)
{
	if (!(key[_key] & 0x80) && (kb.key_prev[_key] & 0x80)) return true;

	return false;
}

bool KeyBoard::None(const int _key)
{
	if (!(key[_key] & 0x80)) return true;

	return false;
}





//ゲームパッド関連
GamePad pad;

SHORT GamePad::thres_hold = static_cast<SHORT>(65535.0 * 0.1);

HRESULT GamePad::UpdateControllerState()
{
	DWORD dw_result = 0;
	for (DWORD i = 0; i < controllerMax; i++) {
		dw_result = XInputGetState(i, &gamePad[i].state);

		if (dw_result == ERROR_SUCCESS) gamePad[i].is_connected = true;
		else							gamePad[i].is_connected = false;
	}

	return S_OK;
}

void GamePad::GetState()
{
	pad.UpdateControllerState();

	for (int i = 0; i < controllerMax; i++) {
		if (!pad.gamePad[i].is_connected) continue;

		pad.buttons_prev[i] = pad.buttons[i];
		pad.buttons[i] = pad.gamePad[i].state.Gamepad.wButtons;

		pad.trg_prev[i].left = pad.trg[i].left;
		pad.trg_prev[i].right = pad.trg[i].right;
		pad.trg[i].left = pad.gamePad[i].state.Gamepad.bLeftTrigger;
		pad.trg[i].right = pad.gamePad[i].state.Gamepad.bRightTrigger;
	}
}

bool GamePad::CheckRange(const SHORT x, const SHORT y)
{
	if (x > GamePad::thres_hold)	return true;
	if (x < -GamePad::thres_hold)	return true;
	if (y > GamePad::thres_hold)	return true;
	if (y < -GamePad::thres_hold)	return true;

	return false;
}

bool GamePad::CheckLStickRange(const PlayerID player)
{
	return CheckRange(gamePad[player].state.Gamepad.sThumbLX, gamePad[player].state.Gamepad.sThumbLY);
}

bool GamePad::CheckRStickRange(const PlayerID player)
{
	return CheckRange(gamePad[player].state.Gamepad.sThumbRX, gamePad[player].state.Gamepad.sThumbRY);
}

float GamePad::GetDegreeAngle(const float x, const float y)
{
	return atan2f(y, x) * TO_DEGREE;
}

float GamePad::GetRadianAngle(const float x, const float y)
{
	return atan2f(y, x);
}

float GamePad::GetLStickDegreeAngle(const PlayerID player)
{
	float x = static_cast<float>(gamePad[player].state.Gamepad.sThumbLX);
	float y = static_cast<float>(gamePad[player].state.Gamepad.sThumbLY);

	return GetDegreeAngle(y, x);
}

float GamePad::GetLStickRadianAngle(const PlayerID player)
{
	float x = static_cast<float>(gamePad[player].state.Gamepad.sThumbLX);
	float y = static_cast<float>(gamePad[player].state.Gamepad.sThumbLY);

	return GetRadianAngle(y, x);
}

float GamePad::GetRStickDegreeAngle(const PlayerID player)
{
	float x = static_cast<float>(gamePad[player].state.Gamepad.sThumbRX);
	float y = static_cast<float>(gamePad[player].state.Gamepad.sThumbRY);

	return GetDegreeAngle(y, x);
}

float GamePad::GetRStickRadianAngle(const PlayerID player)
{
	float x = static_cast<float>(gamePad[player].state.Gamepad.sThumbRX);
	float y = static_cast<float>(gamePad[player].state.Gamepad.sThumbRY);

	return GetRadianAngle(y, x);
}

DirectX::XMFLOAT2 GamePad::GetLStickVector(const PlayerID player)
{
	float x = static_cast< float >(gamePad[player].state.Gamepad.sThumbLX);
	float y = static_cast< float >(gamePad[player].state.Gamepad.sThumbLY);

	return DirectX::XMFLOAT2(x, y);
}

void GamePad::SetVibration(const float motor1_speed, const float motor2_speed, const int player)
{
	if (motor1_speed < 0.0f || motor1_speed > 100.0f) return;
	if (motor2_speed < 0.0f || motor2_speed > 100.0f) return;

	if (!gamePad[player].is_connected) return;

	float ajust = 65535.0f*0.01f;	//調整値

	gamePad[player].vibration.wLeftMotorSpeed = static_cast<WORD>(motor2_speed * ajust);
	gamePad[player].vibration.wRightMotorSpeed = static_cast<WORD>(motor1_speed * ajust);

	XInputSetState(player, &gamePad[player].vibration);
}

void GamePad::ResetVibration(const PlayerID player)
{
	SetVibration(0.0f, 0.0f, player);
}

bool GamePad::Push(const int botton, const PlayerID player)
{
	if (pad.buttons[player] & botton) return true;

	return false;
}

bool GamePad::PushTrigger(const int botton, const PlayerID player)
{
	if ((pad.buttons[player] & botton) && !(pad.buttons_prev[player] & botton)) return true;

	return false;
}

bool GamePad::ReleaseTrigger(const int botton, const PlayerID player)
{
	if (!(pad.buttons[player] & botton) && (pad.buttons_prev[player] & botton)) return true;

	return false;
}

bool GamePad::None(const int botton, const PlayerID player)
{
	if (!(pad.buttons[player] & botton)) return true;

	return false;
}

bool GamePad::LTrigger(const TriggerType type, const PlayerID player)
{
	switch (type) {
	case TriggerType::PUSH:
		if (pad.trg[player].left) return true;

		return false;

	case TriggerType::PUSH_TRIGGER:
		if (pad.trg[player].left && !pad.trg_prev[player].left) return true;

		return false;

	case TriggerType::RELEASE_TRIGGER:
		if (!pad.trg[player].left && pad.trg_prev[player].left) return true;

		return false;

	case TriggerType::NONE:
		if (!pad.trg[player].left) return true;

		return false;

	default:
		return false;
	}
}

bool GamePad::RTrigger(const TriggerType type, const PlayerID player)
{
	switch (type) {
	case TriggerType::PUSH:
		if (pad.trg[player].right) return true;

		return false;

	case TriggerType::PUSH_TRIGGER:
		if (pad.trg[player].right && !pad.trg_prev[player].right) return true;

		return false;

	case TriggerType::RELEASE_TRIGGER:
		if (!pad.trg[player].right && pad.trg_prev[player].right) return true;

		return false;

	case TriggerType::NONE:
		if (!pad.trg[player].right) return true;

		return false;

	default:
		return false;
	}
}

