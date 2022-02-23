/*
 * @file    GyroEstimator.cpp
 * @brief	WiiRemote�̃��[�V�����v���X�ɂ��p������T���v��
			WiiYourSelf!���g���Ă��܂��B
			contains WiiYourself! wiimote code by gl.tter
			http://gl.tter.org
 * @date	2011-2
 * @autor   Brays
 */

#include <Windows.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9d.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxerr.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "comctl32.lib")

#include "Common.h"
#include "Renderer.h"
#include "Scene.h"

#include "wiimote.h"
#include <mmsystem.h>

#define CLIENT_WIDTH  640 // �N���C�A���g�̈�̃f�t�H���g�̕�
#define CLIENT_HEIGHT 480 // �N���C�A���g�̈�̃f�t�H���g�̍���
TCHAR className[] = TEXT("GyroEstimator");	// �N���X��
TCHAR winName[] = TEXT("GyroEstimator");		// �E�B���h�E��

#define CONNECT_TRY_NUM 2		// �q����Ȃ������Ƃ��Ɏ�����
#define STATIC_CALIB_NUM 100	// �L�����u���[�V�����̍ۂ̃T���v�����O��

LRESULT CALLBACK WinProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

//----------------------------------------------------
// �O���[�o���ϐ�
//----------------------------------------------------
// ���[�V�����v���X����擾�����p���x
float g_speed_yaw = 0.0f;
float g_speed_pitch = 0.0f;
float g_speed_roll = 0.0f;

// �|�[�Y���Z�b�g�p
bool g_isResetPose = false;
bool g_isStaticCalibration = false;

//=======================================================
// state-change callback
//=======================================================
void on_state_change( wiimote &remote,
					  state_change_flags  changed,
					  const wiimote_state &new_state ) {
	// �ڑ����ꂽ��
	if( changed & CONNECTED )
	{
		if( new_state.ExtensionType != wiimote::BALANCE_BOARD ) {
			if( new_state.bExtension )
				remote.SetReportType( wiimote::IN_BUTTONS_ACCEL_IR_EXT );	// no IR dots
			else
				remote.SetReportType( wiimote::IN_BUTTONS_ACCEL_IR );	// IR dots
		}
	}

	// ���[�V�����v���X�̌��o������
	if( changed & MOTIONPLUS_DETECTED ) {
		if( remote.ExtensionType == wiimote_state::NONE ) {
			bool res = remote.EnableMotionPlus();
			_ASSERT(res);
		}
	// �g���R�l�N�^�Ƀ��[�V�����v���X���ڑ�����Ă���
	} else if( changed & MOTIONPLUS_EXTENSION_CONNECTED ) {
		if( remote.MotionPlusEnabled() )
			remote.EnableMotionPlus();
	// ���[�V�����v���X���g���R�l�N�^����ؒf���ꂽ��
	} else if( changed & MOTIONPLUS_EXTENSION_DISCONNECTED ) {
		// �Ăу��[�V�����v���X�̃f�[�^��L���ɂ���
		if( remote.MotionPlusConnected() )
			remote.EnableMotionPlus();
	// ���̑��̊g���@�킪�ڑ����ꂽ��
	} else if( changed & EXTENSION_CONNECTED ) {
		if( !remote.IsBalanceBoard() )
			remote.SetReportType( wiimote::IN_BUTTONS_ACCEL_EXT );
	// ���̑��̊g���@�킪�ؒf���ꂽ��
	} else if( changed & EXTENSION_DISCONNECTED ) {
		remote.SetReportType( wiimote::IN_BUTTONS_ACCEL_IR );
	}

	// ���炩�̕ω����N��������
	if( changed & CHANGED_ALL ) {
		// ���t���b�V��
		remote.RefreshState();

		// ���[�V�����v���X�̏�ԕω����N��������
		if( changed & MOTIONPLUS_SPEED_CHANGED ) {
			g_speed_yaw = remote.MotionPlus.Speed.Yaw;
			g_speed_pitch = remote.MotionPlus.Speed.Pitch;
			g_speed_roll = remote.MotionPlus.Speed.Roll;
		}
	}
}



//=======================================================
// �A�v���P�[�V�����̃G���g���[�|�C���g
// ����
//		hInstance     : ���݂̃C���X�^���X�̃n���h��
//		hPrevInstance : �ȑO�̃C���X�^���X�̃n���h��
//		lpCmdLine	  : �R�}���h���C���p�����[�^
//		nCmdShow	  : �E�B���h�E�̕\�����
// �߂�l
//		����������0�ȊO�̒l
//=======================================================
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	HWND    hWnd;	// �E�B���h�E�n���h��
	MSG     msg;	// ���b�Z�[�W

	// �E�B���h�E�N���X�̏�����
	WNDCLASSEX wcex = {
		sizeof(WNDCLASSEX),				// ���̍\���̂̃T�C�Y
		NULL,							// �E�B���h�E�̃X�^�C��
		WinProc,						// ���b�Z�[�W�����֐�(WinMsgProc)��o�^(�v���V�[�W��)
		0,								// �⑫�������u���b�N�i�ʏ��0�j
		0,								// �⑫�������u���b�N�̃T�C�Y�i�ʏ��0�j
		hInstance,						// �C���X�^���X�ւ̃n���h��
		NULL,							// �A�C�R��
		LoadCursor(NULL, IDC_ARROW),	// �J�[�\���̌`
		NULL,							// �w�i�F
		NULL,							// ���j���[
		className,						// �N���X���̎w��
		NULL							// ���A�C�R��
	};

	// �E�B���h�E�N���X�̓o�^
	if( RegisterClassEx(&wcex) == 0 ) {
		return 0;	// �o�^���s
	}

	// �E�B���h�E�̍쐬
	hWnd = CreateWindow(
		className,
		winName,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
		);

	// �E�B���h�E�T�C�Y���Đݒ肷��
	RECT rect;		// �����`��4�_��ێ�����\����
	int ww, wh;		// �E�B���h�E�S�̂̉����A�c��
	int cw, ch;		// �N���C�A���g�̈�̉����A�c��
	// �N���C�A���g�̈�̊O�̕����v�Z
	GetClientRect( hWnd, &rect );		// �N���C�A���g�����̃T�C�Y���擾
	cw = rect.right - rect.left;		// �N���C�A���g�̈�̉������v�Z
	ch = rect.bottom - rect.top;		// �N���C�A���g�̈�O�̉������v�Z
	
	// �E�B���h���S�̂̉������v�Z
	GetWindowRect( hWnd, &rect );		// �E�B���h�E�S�̂̃T�C�Y�擾
	ww = rect.right - rect.left;		// �E�B���h�E�S�̂̉������v�Z
	wh = rect.bottom - rect.top;		// �E�B���h�E�S�̂̏c�����v�Z
	ww = ww - cw;						// �N���C�A���g�̈�ȊO�ɕK�v�ȕ�
	wh = wh - ch;						// �N���C�A���g�̈�ȊO�ɕK�v�ȍ���

	// �E�B���h�E�T�C�Y�̍Čv�Z
	ww = CLIENT_WIDTH + ww;				// �K�v�ȃE�B���h�E�̕�
	wh = CLIENT_HEIGHT + wh;			// �K�v�ȃE�B���h�E�̍���

	// �E�B���h�E�T�C�Y�̍Đݒ�
	SetWindowPos(
		hWnd,		// �E�B���h�E�n���h��
		HWND_TOP,	// �z�u�����̃n���h��
		0,			// �������̈ʒu
		0,			// �c�����̈ʒu
		ww,			// ����
		wh,			// �c��
		SWP_NOMOVE	// �E�B���h�E�ʒu�̃I�v�V���� 
		);
		
	// �E�B���h�E�̕\��
	ShowWindow( hWnd, nCmdShow );

	// WM_PAINT���Ă΂�Ȃ��悤�ɂ���
	// �X�V���[�W�����i�ĕ`��j�̑j�~
	ValidateRect( hWnd, 0 );

	// �����_���[�I�u�W�F�N�g
	Renderer renderer;

	// �����_���[�̏�����
	HRESULT hr;
	hr = renderer.Initialize( hWnd, CLIENT_WIDTH, CLIENT_HEIGHT );

	if( FAILED(hr) ) {
		// ���������s
		MessageBox( NULL, L"�����_���[�̏��������ł��܂���ł���", L"�G���[", MB_OK | MB_ICONERROR );
		return 0;
	}

	// �V�[���I�u�W�F�N�g
	Scene scene;

	// �V�[���̐���
	if( FAILED( scene.Create( renderer.GetDevice() ) ) ) {
		// �������s
		MessageBox( NULL, L"�V�[���������ł��܂���ł���", L"�G���[", MB_OK | MB_ICONERROR );
		return 0;
	}

	///------------ ��������wiiremote�֘A�������� ------------///

	// wiimote�I�u�W�F�N�g
	wiimote wm;

	// �R�[���o�b�N�֐���ݒ�
	wm.ChangedCallback = on_state_change;
	// �R�[���o�b�N������S�t���O�ɐݒ�
	wm.CallbackTriggerFlags = (state_change_flags)(CONNECTED | CHANGED_ALL);

	int count = 0;
	while( !wm.Connect( wiimote::FIRST_AVAILABLE ) ) {
		Sleep( 1000 );

		if( count >= CONNECT_TRY_NUM ) {		// CONNECT_TRY_NUM�񎎂��Ă��_����������
			MessageBox( NULL, L"Wii�����R���ɐڑ��ł��܂���ł���", L"�G���[", MB_OK | MB_ICONERROR );
			return 0;
		}
		count++;
	}
	count = 0;

	///------------ �����܂�wiiremote�֘A�������� ------------///

	// ���C�����[�v
	while(TRUE) {
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
			if( msg.message == WM_QUIT ) {
				break;
			} else {
				// ���b�Z�[�W�̖|��ƃf�B�X�p�b�`�i���o�j
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		} else {	// �������郁�b�Z�[�W�������Ƃ��͕`����s��
			// �E�B���h�E�������Ă���Ƃ�������������
			WINDOWPLACEMENT wndpl;
			GetWindowPlacement( hWnd, &wndpl );	// �E�B���h�E�̏�Ԃ��擾
			if( (wndpl.showCmd != SW_HIDE) &&				// �E�B���h�E��\��
				(wndpl.showCmd != SW_MINIMIZE) &&			// �ŏ���
				(wndpl.showCmd != SW_SHOWMINIMIZED) &&		// �A�C�R���Ƃ��ĕ\��
				(wndpl.showCmd != SW_SHOWMINNOACTIVE) ) {	// �A�C�R���Ƃ��ĕ\��

					float yaw, pitch, roll;	// �Z�o�p�x
					static float angle_yaw = 0, angle_pitch = 0, angle_roll = 0;	// �p��
					static float offset_yaw = -29, offset_pitch = 9, offset_roll = 18;	// �I�t�Z�b�g�i�l�͏����l�j
					static float temp_yaw = 0, temp_pitch = 0, temp_roll = 0;	// �e���|���� 
					TCHAR text[80];			// �]���p�����z��

					// �ÓI�L�����u���[�V����
					if( g_isStaticCalibration ) {
						temp_yaw += g_speed_yaw;
						temp_pitch += g_speed_pitch;
						temp_roll += g_speed_roll;

						count++;
						if( count >= STATIC_CALIB_NUM ) {
							offset_yaw = temp_yaw / STATIC_CALIB_NUM;
							offset_pitch = temp_pitch / STATIC_CALIB_NUM;
							offset_roll = temp_roll / STATIC_CALIB_NUM;

							temp_yaw = 0;
							temp_pitch = 0;
							temp_roll = 0;
							count = 0;
							g_isStaticCalibration = false;
						}
					}
					
					// �p���x�␳
					g_speed_yaw -= offset_yaw;
					g_speed_pitch -= offset_pitch;
					g_speed_roll -= offset_roll;

					// �I�t�Z�b�g�̕\��
					_stprintf_s( text, TEXT("%4.3f %4.3f %4.3f"), offset_yaw, offset_pitch, offset_roll);
					for( int i=0; i<80; i++ ) {
						renderer.m_text3[i] = text[i];
					}

					// �t���[�����[�g�v��
					static DWORD time = timeGetTime();		// �P�ʂ�[ms]
					// 1�t���[��������̎��Ԍv��
					DWORD frametime = timeGetTime() - time;
					time = timeGetTime();

					// �p�x�Z�o
					if( g_speed_yaw > 10 || g_speed_yaw < -10 ) {
						yaw = g_speed_yaw * frametime / 1000;
						angle_yaw += yaw;
					}
					if( g_speed_pitch > 10 || g_speed_pitch < -10 ) {
						pitch = g_speed_pitch * frametime / 1000;
						angle_pitch += pitch;
					}
					if( g_speed_roll > 10 || g_speed_roll < -10 ) {
						roll = g_speed_roll * frametime / 1000;
						angle_roll += roll;
					}
					
					// �p���̃��Z�b�g
					if( g_isResetPose ) {
						angle_yaw = angle_pitch = angle_roll = 0.0f;
						g_isResetPose = false;
					}

					// �p�x�̃I�[�o�[�t���[�΍�
					if( yaw <= -180 ) {
						angle_yaw = 180;
					}
					if( yaw > 180 ) {
						angle_yaw = -180;
					}
					if( pitch <= -180 ) {
						angle_pitch = 180;
					}
					if( pitch > 180 ) {
						angle_pitch = -180;
					}
					if( roll <= -180 ) {
						angle_roll = 180;
					}
					if( roll > 180 ) {
						angle_roll = -180;
					}

					// �V�[���ɒl�𑗂�
					scene.SetYawPitchRoll( angle_yaw, angle_pitch, angle_roll );

					// �e�p���̕\��
					_stprintf_s( text, TEXT("%4.3f %4.3f %4.3f"), angle_yaw, angle_pitch, angle_roll);
					for( int i=0; i<80; i++ ) {
						renderer.m_text2[i] = text[i];
					}
					
					// �`�揈���̎��s
					renderer.RenderScene( &scene );
			}
		}
	}

	// wiiremote�I������
	wm.SetLEDs(0);
	wm.SetRumble(0);
	wm.Disconnect();

	return (int) msg.wParam;
}


//=======================================================
// ���b�Z�[�W�����p�R�[���o�b�N�֐�
// ����
//		hWnd	: �E�B���h�E�n���h��
//		msg		: ���b�Z�[�W
//		wParam	: ���b�Z�[�W�̍ŏ��̃p�����[�^
//		lParam	: ���b�Z�[�W��2�Ԗڂ̃p�����[�^
// �߂�l
//		���b�Z�[�W��������
//=======================================================
LRESULT CALLBACK WinProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg ) {
	case WM_CLOSE:		// �E�B���h�E������ꂽ
		PostQuitMessage(0);				// �A�v���P�[�V�����̏I��
		break;
	case WM_KEYDOWN:	// �L�[�������ꂽ
		if( wParam == VK_ESCAPE ) {		// ESC�L�[�������ꂽ
			PostQuitMessage(0);			// �A�v���P�[�V�����̏I��
			break;
		}
		if( wParam == 'R' ) {			// R�L�[�������ꂽ
			g_isResetPose = true;		// �|�[�Y�̃��Z�b�g
			break;
		}
		if( wParam == 'S' ) {			// S�L�[�������ꂽ
			g_isStaticCalibration = true;	// �ÓI�L�����u���[�V����
			break;
		}
		
		break;
	default:
		return DefWindowProc( hWnd, msg, wParam, lParam );
	}

	return 0;
}