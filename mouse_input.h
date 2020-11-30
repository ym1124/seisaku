#pragma once

#include <Windows.h>
#include <Xinput.h>
#include "math.h"

#pragma comment (lib, "xinput.lib")

#define TO_RADIAN	0.017453292519f
#define TO_DEGREE	52.29577951308f

//マウス関連
class Mouse
{
public:
	enum Botton
	{
		LEFT,
		RIGHT,
		MIDDLE,
		BOTTON_MAX,
	};

private:
	bool is_push[BOTTON_MAX];
	bool is_release[BOTTON_MAX];

	bool is_push_trigger[BOTTON_MAX];
	bool is_release_trigger[BOTTON_MAX];

	LPPOINT p;

public:
	static UINT GetState(UINT msg);

	DirectX::XMFLOAT2 GetPos();

	void SetIsPush(const Botton botton, const bool flg) { is_push[botton] = flg; }
	void SetIsRelease(const Botton botton, const bool flg) { is_release[botton] = flg; }

	void SetIsPushTrigger(const Botton botton, const bool flg) { is_push_trigger[botton] = flg; }
	void SetIsReleaseTrigger(const Botton botton, const bool flg) { is_release_trigger[botton] = flg; }

	void SetBottonDownFlg(const Botton botton);
	void SetBottonUpFlg(const Botton botton);

	bool Push(const Botton botton);				//マウスが押されていたら		true
	bool PushTrigger(const Botton botton);		//マウスが押された瞬間			true
	bool ReleaseTrigger(const Botton botton);	//マウスが離された瞬間			true
	bool None(const Botton botton);				//マウスがおされてなかったら	true
};

extern Mouse mouse;


//キーボード関連
class KeyBoard
{
private:
	static const int keyMax = 256;

	BYTE key[keyMax];
	BYTE key_prev[keyMax];

	void GetKeyState();

public:
	KeyBoard() :key(), key_prev() {};
	~KeyBoard() {};

	KeyBoard& operator=(const KeyBoard &org)
	{
		for (int i = 0; i < keyMax; i++) {
			key[i] = org.key[i];
		}

		return *this;
	}

	static void GetState();	//全部のキーの状態を取得している

	bool Push(const int _key);				//キーが押されていたら		true
	bool PushTrigger(const int _key);		//キーが押された瞬間		true
	bool ReleaseTrigger(const int _key);	//キーが離された瞬間		true
	bool None(const int _key);				//キーがおされてなかったら	true
};

extern KeyBoard kb;




//ゲームパッド関連
class GamePad
{
private:
	static const int controllerMax = 4;

	struct ControllerState
	{
		XINPUT_STATE state;			//パッドのid
		XINPUT_VIBRATION vibration;	//パッドの振動速度
		bool is_connected;			//パッドが接続されていたら	true
	};

	ControllerState gamePad[controllerMax];

	WORD buttons[controllerMax];
	WORD buttons_prev[controllerMax];

	struct bTrigger
	{
		BYTE left;
		BYTE right;
	};

	bTrigger trg[controllerMax];
	bTrigger trg_prev[controllerMax];

	HRESULT UpdateControllerState();	//パッドが接続されているか調べる

	float GetDegreeAngle(const float x, const float y);
	float GetRadianAngle(const float x, const float y);

	bool CheckRange(const SHORT x, const SHORT y);

public:
	GamePad() :buttons(), buttons_prev(), gamePad(), trg(), trg_prev() {};
	~GamePad() {};

	GamePad& operator=(const GamePad &org)
	{
		for (int i = 0; i < controllerMax; i++) {
			gamePad[i] = org.gamePad[i];
		}

		return *this;
	}

	//どのコントローラーか決めるためのid
	enum PlayerID
	{
		_1P,
		_2P,
		_3P,
		_4P,
		PLAYER_MAX
	};

	//コントローラーの トリガー(TRIGGER) の状態を調べるため 
	enum TriggerType
	{
		PUSH,
		PUSH_TRIGGER,
		RELEASE_TRIGGER,
		NONE,
	};

	static SHORT thres_hold;	//パッドの反応する範囲の設定するための値

	static void GetState();		//パッドの状態を取得している


	bool CheckLStickRange(const PlayerID player);		//左のパッドの反応する範囲か調べる
	bool CheckRStickRange(const PlayerID player);		//右のパッドの反応する範囲か調べる

	float GetLStickDegreeAngle(const PlayerID player);	//度(degree) で左のパッドの角度を調べる
	float GetLStickRadianAngle(const PlayerID player);	//ラジアン で左のパッドの角度を調べる

	float GetRStickDegreeAngle(const PlayerID player);	//度(degree) で右のパッドの角度を調べる
	float GetRStickRadianAngle(const PlayerID player);	//ラジアン で右のパッドの角度を調べる

	DirectX::XMFLOAT2 GetLStickVector(const PlayerID player);

	void SetVibration(const float motor1_speed, const float motor2_speed, const int player);	//パッドの振動の速度の設定	最小 0.0f, 最大 100.0f 
	void ResetVibration(const PlayerID player);													//パッドの振動の速度を 0.0f にする


	bool LTrigger(const TriggerType type, const PlayerID player);	//パッドの左のトリガー(TRIGGER) の状態を調べる
	bool RTrigger(const TriggerType type, const PlayerID player);	//パッドの右のトリガー(TRIGGER) の状態を調べる

	bool Push(const int botton, const PlayerID player);				//ボタンが押されていたら		true
	bool PushTrigger(const int botton, const PlayerID player);		//ボタンが押された瞬間			true
	bool ReleaseTrigger(const int botton, const PlayerID player);	//ボタンが離された瞬間			true
	bool None(const int botton, const PlayerID player);				//ボタンがおされてなかったら	true
};

extern GamePad pad;
