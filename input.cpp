//#include	<Windows.h>
//#include	"input.h"//�v���R���p�C���ς݃w�b�_�[�ł������g�܂ŏ����Ă��������ۂ�

INT64 KeyInput::keyState = 0;
INT64 KeyInput::keyTrigger = 0;
INT64 KeyInput::keyRelease = 0;


void KeyInput::Update()
{
	INT64		oldState;

	oldState = keyState;	// �O���keyState��ޔ�������
	keyState = 0;				// ���݂̏�Ԃ��擾����܂��Ƀ[���N���A����

	if (GetKeyState(VK_UP) < 0) keyState |= KEY_UP;		// ����KEY_UP�Ɋ��蓖��
	if (GetKeyState(VK_DOWN) < 0) keyState |= KEY_DOWN;		// ����KEY_DOWN
	if (GetKeyState(VK_RIGHT) < 0) keyState |= KEY_RIGHT;		// ����KEY_RIGHT
	if (GetKeyState(VK_LEFT) < 0) keyState |= KEY_LEFT;		// ����KEY_LEFT
	if (GetKeyState(VK_SPACE) < 0) keyState |= KEY_START;		// �X�y�[�X��KEY_START
	if (GetKeyState(VK_RETURN) < 0) keyState |= KEY_ENTER;	// ENTER��KEY_ENTER

	if (GetKeyState('A')      < 0) keyState |= KEY_A;
	if (GetKeyState('Z')      < 0) keyState |= KEY_Z;
	if (GetKeyState('X')      < 0) keyState |= KEY_X;
	if (GetKeyState('C')      < 0) keyState |= KEY_C;
	if (GetKeyState('D')      < 0) keyState |= KEY_D;
	if (GetKeyState('E')      < 0) keyState |= KEY_E;
	if (GetKeyState('F')      < 0) keyState |= KEY_F;
	if (GetKeyState('G')      < 0) keyState |= KEY_G;
	if (GetKeyState('H')      < 0) keyState |= KEY_H;
	if (GetKeyState('I')      < 0) keyState |= KEY_I;
	if (GetKeyState('J')      < 0) keyState |= KEY_J;
	if (GetKeyState('K')      < 0) keyState |= KEY_K;
	if (GetKeyState('L')      < 0) keyState |= KEY_L;
	if (GetKeyState('M')      < 0) keyState |= KEY_M;
	if (GetKeyState('N')      < 0) keyState |= KEY_N;
	if (GetKeyState('O')      < 0) keyState |= KEY_O;
	if (GetKeyState('P')      < 0) keyState |= KEY_P;
	if (GetKeyState('Q')      < 0) keyState |= KEY_Q;
	if (GetKeyState('R')      < 0) keyState |= KEY_R;
	if (GetKeyState('S')      < 0) keyState |= KEY_S;
	if (GetKeyState('T')      < 0) keyState |= KEY_T;
	if (GetKeyState('U')      < 0) keyState |= KEY_U;
	if (GetKeyState('V')      < 0) keyState |= KEY_V;
	if (GetKeyState('W')      < 0) keyState |= KEY_W;
	if (GetKeyState('X')      < 0) keyState |= KEY_X;
	if (GetKeyState('Y')      < 0) keyState |= KEY_Y;
	if (GetKeyState('Z')      < 0) keyState |= KEY_Z;

	if (GetKeyState('1')      < 0) keyState |= KEY_1;
	if (GetKeyState('2')      < 0) keyState |= KEY_2;
	if (GetKeyState('3')      < 0) keyState |= KEY_3;
	if (GetKeyState('4')      < 0) keyState |= KEY_4;
	if (GetKeyState('5')      < 0) keyState |= KEY_5;
	if (GetKeyState('6')      < 0) keyState |= KEY_6;
	if (GetKeyState('7')      < 0) keyState |= KEY_7;
	if (GetKeyState('8')      < 0) keyState |= KEY_8;
	if (GetKeyState('9')      < 0) keyState |= KEY_9;
	if (GetKeyState('0')      < 0) keyState |= KEY_0;

	keyTrigger = (~oldState) & keyState;							// �g���K�[���͂̌v�Z
	INT64 now_state = keyState;
	keyRelease = oldState & (~now_state);						// �����[�X�̌v�Z
}