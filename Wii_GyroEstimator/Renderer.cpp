/*
 * @file    Renderer.cpp
 * @brief	DirectXのレンダリングを行うモジュール
 * @date	2011-2
 * @autor   Brays
 */

#include "Renderer.h"
#include "Scene.h"
#include "tchar.h"

Renderer::Renderer(void) : m_pD3D(0), m_pD3DDevice(0), m_lpFont(0), m_frameTime(0)
{
}


Renderer::~Renderer(void)
{
	Finalize();
}

//=======================================================
// レンダラーの初期化
// 引数
//		hWnd         : ウィンドウハンドル
//		clientWidth  : クライアント領域の幅
//		clientHeight : クライアント領域の高さ
// 戻り値
//		成功したらS_OK 
//=======================================================
HRESULT Renderer::Initialize(HWND hWnd, int clientWidth, int clientHeight)
{
	D3DDISPLAYMODE d3ddm;

	// Direct3D9オブジェクトの作成
	if( ( m_pD3D = ::Direct3DCreate9(D3D_SDK_VERSION) ) == 0 ) {
		return E_FAIL;	// 取得失敗
	}

	// 現在のディスプレイモードを取得
	if( FAILED( m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm) ) ) {
		return E_FAIL;
	}

	// デバイスのプレゼンテーションパラメータを初期化
	ZeroMemory( &m_D3DPP, sizeof(D3DPRESENT_PARAMETERS) );
	m_D3DPP.BackBufferCount = 1;
	m_D3DPP.Windowed              = TRUE;			// ウィンドウ内表示の指定
	m_D3DPP.BackBufferFormat          = d3ddm.Format;	// カラーモードの指定

	// フリッピングの方法（生成した画像をどのような方法でモニタ画面に転送するか）
	m_D3DPP.SwapEffect                = D3DSWAPEFFECT_DISCARD;
	
	m_D3DPP.EnableAutoDepthStencil    = TRUE;			// エラー対策
	m_D3DPP.AutoDepthStencilFormat    = D3DFMT_D16;		// エラー対策

	// デバイスオブジェクトの生成
	// できるだけハードウェア的に処理を行うようにする
	// 描画と頂点処理をハードウェアで行う
	if( FAILED( m_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,						// ビデオカードの指定
		D3DDEVTYPE_HAL,							// *デバイスのタイプ
		hWnd,									// ウィンドウハンドル
		D3DCREATE_HARDWARE_VERTEXPROCESSING,	// *デバイスの動作モード
		&m_D3DPP,								// D3DPRESENT_PARAMETERS構造体のアドレス
		&m_pD3DDevice							// 「取得したデバイスへのポインタ」へのポインタ
		)) ) {
			// 上記の設定が失敗したら
			// 描画をハードウェアで行い、頂点処理をCPUで行う
			if( FAILED( m_pD3D->CreateDevice(
				D3DADAPTER_DEFAULT,						// ビデオカードの指定
				D3DDEVTYPE_HAL,							// *デバイスのタイプ
				hWnd,									// ウィンドウハンドル
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,	// *デバイスの動作モード
				&m_D3DPP,								// D3DPRESENT_PARAMETERS構造体のアドレス
				&m_pD3DDevice							// 「取得したデバイスへのポインタ」へのポインタ
				)) ) {
					// 上記の設定が失敗したら
					// 描画と頂点処理をCPUで行う
					if( FAILED( m_pD3D->CreateDevice(
						D3DADAPTER_DEFAULT,						// ビデオカードの指定
						D3DDEVTYPE_REF,							// *デバイスのタイプ
						hWnd,									// ウィンドウハンドル
						D3DCREATE_SOFTWARE_VERTEXPROCESSING,	// *デバイスの動作モード
						&m_D3DPP,								// D3DPRESENT_PARAMETERS構造体のアドレス
						&m_pD3DDevice							// 「取得したデバイスへのポインタ」へのポインタ
						)) ) {
							// 初期化失敗
							return E_FAIL;
					}
			}
	}

	// フォントオブジェクトの作成
	if( FAILED( D3DXCreateFont(
		m_pD3DDevice,				// IDrect3DDevice9インタフェース(デバイス作成してから)
		16,							// 高さ
		0,							// 幅
		FW_HEAVY,					// 太さ
		1,							// ミップマップレベルの数
		FALSE,						// 斜体かどうか
		DEFAULT_CHARSET,			// 文字セット
		OUT_CHARACTER_PRECIS,		// 出力精度
		DEFAULT_QUALITY,			// 出力品質
		DEFAULT_PITCH | FF_SWISS,	// ピッチとファミリ
		L"ＭＳ Ｐゴシック",			// フォント名
		&m_lpFont					// ID3DXFontインタフェースへのポインタ
		) ) ){
			
			return E_FAIL;
	}
	
	return S_OK;
}

//=======================================================
// デバイスの取得
// 戻り値
//		成功したらIDirect3DDevice9インターフェイスへのポインタ
//		失敗したら0
//=======================================================
LPDIRECT3DDEVICE9 Renderer::GetDevice()
{
	return m_pD3DDevice;
}

//=======================================================
// シーンの描画
// 引数
//		pScene : シーンへのポインタ
// 戻り値
//		成功したらS_OK 
//=======================================================
void Renderer::RenderScene( Scene* pScene )
{
	// フレームレート計測
	static DWORD time = timeGetTime();		// 単位は[ms]
	// 1フレームあたりの時間計測
	m_frameTime = timeGetTime() - time;
	time = timeGetTime();

	if( pScene == 0 ) {
		return;
	}

	// ビューポートと深度バッファのクリアとステンシルバッファの削除
	if( FAILED( m_pD3DDevice->Clear( 
							0,							// 次の引数であるpRectsの配列にある矩形の数。pRectsをNULLに設定する場合は、このパラメータを 0 に設定する。 
							NULL,						// クリアする領域は全面
							D3DCLEAR_TARGET |			// バックバッファをを指定
							D3DCLEAR_ZBUFFER,			// 深度バッファ（Zバッファ）を指定
							D3DCOLOR_XRGB( 0, 0, 255 ),	// クリアする色
							1.0f,						// クリアする深度バッファ（Zバッファの値）
							0							// クリアするステンシルバッファの値
							)) ){
		return;
	}

	// 描画開始宣言
	if( SUCCEEDED( m_pD3DDevice->BeginScene() ) ) {
		// モデルなどの描画
		pScene->Draw( m_pD3DDevice );

		// テキストの描画
		_stprintf_s( m_text1, TEXT("FPS:%f"), 1000.0 / m_frameTime );
		RenderText1( m_text1 );
		RenderText2( m_text2 );
		RenderText3( m_text3 );

		// 描画終了宣言
		m_pD3DDevice->EndScene();
	}

	//----- この時点でシーンはバックバッファに書きこまれている -----//

	// 描画結果をディスプレイに転送
	if( FAILED( m_pD3DDevice->Present( 0, 0, 0, 0 ) ) ) {	// バックバッファの表示
		// デバイス消失から復帰
		m_pD3DDevice->Reset( &m_D3DPP );
	}



}

//=======================================================
// テキストの描画
//=======================================================
void Renderer::RenderText1(TCHAR str[])
{
	// フォントの描画範囲の指定
	m_rect.left   = 5;
	m_rect.top    = 5;
	m_rect.right  = 250;
	m_rect.bottom = 70;

	m_lpFont->DrawText( NULL, str, -1, &m_rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB( 255, 255, 255, 0 ) );
}

void Renderer::RenderText2(TCHAR str[])
{
	// フォントの描画範囲の指定
	m_rect.left   = 5;
	m_rect.top    = 21;
	m_rect.right  = 250;
	m_rect.bottom = 70;

	m_lpFont->DrawText( NULL, str, -1, &m_rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB( 255, 255, 255, 0 ) );
}

void Renderer::RenderText3(TCHAR str[])
{
	// フォントの描画範囲の指定
	m_rect.left   = 5;
	m_rect.top    = 37;
	m_rect.right  = 250;
	m_rect.bottom = 70;

	m_lpFont->DrawText( NULL, str, -1, &m_rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB( 255, 255, 255, 0 ) );
}

//=======================================================
// 終了処理
//=======================================================
void Renderer::Finalize()
{
	// デバイスオブジェクトの解放
	SAFE_RELEASE( m_pD3DDevice );

	// DirectXGraphicsの解放
	SAFE_RELEASE( m_pD3D );

	// フォントオブジェクトの解放
	SAFE_RELEASE( m_lpFont );
}