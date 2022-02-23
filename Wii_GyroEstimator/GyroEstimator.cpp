/*
 * @file    GyroEstimator.cpp
 * @brief	WiiRemoteのモーションプラスによる姿勢推定サンプル
			WiiYourSelf!を使っています。
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

#define CLIENT_WIDTH  640 // クライアント領域のデフォルトの幅
#define CLIENT_HEIGHT 480 // クライアント領域のデフォルトの高さ
TCHAR className[] = TEXT("GyroEstimator");	// クラス名
TCHAR winName[] = TEXT("GyroEstimator");		// ウィンドウ名

#define CONNECT_TRY_NUM 2		// 繋がらなかったときに試す回数
#define STATIC_CALIB_NUM 100	// キャリブレーションの際のサンプリング数

LRESULT CALLBACK WinProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

//----------------------------------------------------
// グローバル変数
//----------------------------------------------------
// モーションプラスから取得した角速度
float g_speed_yaw = 0.0f;
float g_speed_pitch = 0.0f;
float g_speed_roll = 0.0f;

// ポーズリセット用
bool g_isResetPose = false;
bool g_isStaticCalibration = false;

//=======================================================
// state-change callback
//=======================================================
void on_state_change( wiimote &remote,
					  state_change_flags  changed,
					  const wiimote_state &new_state ) {
	// 接続されたら
	if( changed & CONNECTED )
	{
		if( new_state.ExtensionType != wiimote::BALANCE_BOARD ) {
			if( new_state.bExtension )
				remote.SetReportType( wiimote::IN_BUTTONS_ACCEL_IR_EXT );	// no IR dots
			else
				remote.SetReportType( wiimote::IN_BUTTONS_ACCEL_IR );	// IR dots
		}
	}

	// モーションプラスの検出したら
	if( changed & MOTIONPLUS_DETECTED ) {
		if( remote.ExtensionType == wiimote_state::NONE ) {
			bool res = remote.EnableMotionPlus();
			_ASSERT(res);
		}
	// 拡張コネクタにモーションプラスが接続されてたら
	} else if( changed & MOTIONPLUS_EXTENSION_CONNECTED ) {
		if( remote.MotionPlusEnabled() )
			remote.EnableMotionPlus();
	// モーションプラスが拡張コネクタから切断されたら
	} else if( changed & MOTIONPLUS_EXTENSION_DISCONNECTED ) {
		// 再びモーションプラスのデータを有効にする
		if( remote.MotionPlusConnected() )
			remote.EnableMotionPlus();
	// その他の拡張機器が接続されたら
	} else if( changed & EXTENSION_CONNECTED ) {
		if( !remote.IsBalanceBoard() )
			remote.SetReportType( wiimote::IN_BUTTONS_ACCEL_EXT );
	// その他の拡張機器が切断されたら
	} else if( changed & EXTENSION_DISCONNECTED ) {
		remote.SetReportType( wiimote::IN_BUTTONS_ACCEL_IR );
	}

	// 何らかの変化が起こったら
	if( changed & CHANGED_ALL ) {
		// リフレッシュ
		remote.RefreshState();

		// モーションプラスの状態変化が起こったら
		if( changed & MOTIONPLUS_SPEED_CHANGED ) {
			g_speed_yaw = remote.MotionPlus.Speed.Yaw;
			g_speed_pitch = remote.MotionPlus.Speed.Pitch;
			g_speed_roll = remote.MotionPlus.Speed.Roll;
		}
	}
}



//=======================================================
// アプリケーションのエントリーポイント
// 引数
//		hInstance     : 現在のインスタンスのハンドル
//		hPrevInstance : 以前のインスタンスのハンドル
//		lpCmdLine	  : コマンドラインパラメータ
//		nCmdShow	  : ウィンドウの表示状態
// 戻り値
//		成功したら0以外の値
//=======================================================
int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
	HWND    hWnd;	// ウィンドウハンドル
	MSG     msg;	// メッセージ

	// ウィンドウクラスの初期化
	WNDCLASSEX wcex = {
		sizeof(WNDCLASSEX),				// この構造体のサイズ
		NULL,							// ウィンドウのスタイル
		WinProc,						// メッセージ処理関数(WinMsgProc)を登録(プロシージャ)
		0,								// 補足メモリブロック（通常は0）
		0,								// 補足メモリブロックのサイズ（通常は0）
		hInstance,						// インスタンスへのハンドル
		NULL,							// アイコン
		LoadCursor(NULL, IDC_ARROW),	// カーソルの形
		NULL,							// 背景色
		NULL,							// メニュー
		className,						// クラス名の指定
		NULL							// 小アイコン
	};

	// ウィンドウクラスの登録
	if( RegisterClassEx(&wcex) == 0 ) {
		return 0;	// 登録失敗
	}

	// ウィンドウの作成
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

	// ウィンドウサイズを再設定する
	RECT rect;		// 長方形の4点を保持する構造体
	int ww, wh;		// ウィンドウ全体の横幅、縦幅
	int cw, ch;		// クライアント領域の横幅、縦幅
	// クライアント領域の外の幅を計算
	GetClientRect( hWnd, &rect );		// クライアント部分のサイズを取得
	cw = rect.right - rect.left;		// クライアント領域の横幅を計算
	ch = rect.bottom - rect.top;		// クライアント領域外の横幅を計算
	
	// ウィンドう全体の横幅を計算
	GetWindowRect( hWnd, &rect );		// ウィンドウ全体のサイズ取得
	ww = rect.right - rect.left;		// ウィンドウ全体の横幅を計算
	wh = rect.bottom - rect.top;		// ウィンドウ全体の縦幅を計算
	ww = ww - cw;						// クライアント領域以外に必要な幅
	wh = wh - ch;						// クライアント領域以外に必要な高さ

	// ウィンドウサイズの再計算
	ww = CLIENT_WIDTH + ww;				// 必要なウィンドウの幅
	wh = CLIENT_HEIGHT + wh;			// 必要なウィンドウの高さ

	// ウィンドウサイズの再設定
	SetWindowPos(
		hWnd,		// ウィンドウハンドル
		HWND_TOP,	// 配置順序のハンドル
		0,			// 横方向の位置
		0,			// 縦方向の位置
		ww,			// 横幅
		wh,			// 縦幅
		SWP_NOMOVE	// ウィンドウ位置のオプション 
		);
		
	// ウィンドウの表示
	ShowWindow( hWnd, nCmdShow );

	// WM_PAINTが呼ばれないようにする
	// 更新リージョン（再描画）の阻止
	ValidateRect( hWnd, 0 );

	// レンダラーオブジェクト
	Renderer renderer;

	// レンダラーの初期化
	HRESULT hr;
	hr = renderer.Initialize( hWnd, CLIENT_WIDTH, CLIENT_HEIGHT );

	if( FAILED(hr) ) {
		// 初期化失敗
		MessageBox( NULL, L"レンダラーの初期化ができませんでした", L"エラー", MB_OK | MB_ICONERROR );
		return 0;
	}

	// シーンオブジェクト
	Scene scene;

	// シーンの生成
	if( FAILED( scene.Create( renderer.GetDevice() ) ) ) {
		// 生成失敗
		MessageBox( NULL, L"シーンが生成できませんでした", L"エラー", MB_OK | MB_ICONERROR );
		return 0;
	}

	///------------ ここからwiiremote関連初期処理 ------------///

	// wiimoteオブジェクト
	wiimote wm;

	// コールバック関数を設定
	wm.ChangedCallback = on_state_change;
	// コールバック条件を全フラグに設定
	wm.CallbackTriggerFlags = (state_change_flags)(CONNECTED | CHANGED_ALL);

	int count = 0;
	while( !wm.Connect( wiimote::FIRST_AVAILABLE ) ) {
		Sleep( 1000 );

		if( count >= CONNECT_TRY_NUM ) {		// CONNECT_TRY_NUM回試してもダメだったら
			MessageBox( NULL, L"Wiiリモコンに接続できませんでした", L"エラー", MB_OK | MB_ICONERROR );
			return 0;
		}
		count++;
	}
	count = 0;

	///------------ ここまでwiiremote関連初期処理 ------------///

	// メインループ
	while(TRUE) {
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {
			if( msg.message == WM_QUIT ) {
				break;
			} else {
				// メッセージの翻訳とディスパッチ（送出）
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
		} else {	// 処理するメッセージが無いときは描画を行う
			// ウィンドウが見えているときだけ処理する
			WINDOWPLACEMENT wndpl;
			GetWindowPlacement( hWnd, &wndpl );	// ウィンドウの状態を取得
			if( (wndpl.showCmd != SW_HIDE) &&				// ウィンドウ非表示
				(wndpl.showCmd != SW_MINIMIZE) &&			// 最小化
				(wndpl.showCmd != SW_SHOWMINIMIZED) &&		// アイコンとして表示
				(wndpl.showCmd != SW_SHOWMINNOACTIVE) ) {	// アイコンとして表示

					float yaw, pitch, roll;	// 算出角度
					static float angle_yaw = 0, angle_pitch = 0, angle_roll = 0;	// 姿勢
					static float offset_yaw = -29, offset_pitch = 9, offset_roll = 18;	// オフセット（値は初期値）
					static float temp_yaw = 0, temp_pitch = 0, temp_roll = 0;	// テンポラリ 
					TCHAR text[80];			// 転送用文字配列

					// 静的キャリブレーション
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
					
					// 角速度補正
					g_speed_yaw -= offset_yaw;
					g_speed_pitch -= offset_pitch;
					g_speed_roll -= offset_roll;

					// オフセットの表示
					_stprintf_s( text, TEXT("%4.3f %4.3f %4.3f"), offset_yaw, offset_pitch, offset_roll);
					for( int i=0; i<80; i++ ) {
						renderer.m_text3[i] = text[i];
					}

					// フレームレート計測
					static DWORD time = timeGetTime();		// 単位は[ms]
					// 1フレームあたりの時間計測
					DWORD frametime = timeGetTime() - time;
					time = timeGetTime();

					// 角度算出
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
					
					// 姿勢のリセット
					if( g_isResetPose ) {
						angle_yaw = angle_pitch = angle_roll = 0.0f;
						g_isResetPose = false;
					}

					// 角度のオーバーフロー対策
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

					// シーンに値を送る
					scene.SetYawPitchRoll( angle_yaw, angle_pitch, angle_roll );

					// 各姿勢の表示
					_stprintf_s( text, TEXT("%4.3f %4.3f %4.3f"), angle_yaw, angle_pitch, angle_roll);
					for( int i=0; i<80; i++ ) {
						renderer.m_text2[i] = text[i];
					}
					
					// 描画処理の実行
					renderer.RenderScene( &scene );
			}
		}
	}

	// wiiremote終了処理
	wm.SetLEDs(0);
	wm.SetRumble(0);
	wm.Disconnect();

	return (int) msg.wParam;
}


//=======================================================
// メッセージ処理用コールバック関数
// 引数
//		hWnd	: ウィンドウハンドル
//		msg		: メッセージ
//		wParam	: メッセージの最初のパラメータ
//		lParam	: メッセージの2番目のパラメータ
// 戻り値
//		メッセージ処理結果
//=======================================================
LRESULT CALLBACK WinProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg ) {
	case WM_CLOSE:		// ウィンドウが閉じられた
		PostQuitMessage(0);				// アプリケーションの終了
		break;
	case WM_KEYDOWN:	// キーが押された
		if( wParam == VK_ESCAPE ) {		// ESCキーが押された
			PostQuitMessage(0);			// アプリケーションの終了
			break;
		}
		if( wParam == 'R' ) {			// Rキーが押された
			g_isResetPose = true;		// ポーズのリセット
			break;
		}
		if( wParam == 'S' ) {			// Sキーが押された
			g_isStaticCalibration = true;	// 静的キャリブレーション
			break;
		}
		
		break;
	default:
		return DefWindowProc( hWnd, msg, wParam, lParam );
	}

	return 0;
}