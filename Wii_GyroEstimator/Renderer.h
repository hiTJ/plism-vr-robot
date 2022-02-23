/*
 * @file    Renderer.h
 * @brief	DirectXのレンダリングを行うモジュール
 * @date	2011-2
 * @autor   Brays
 */

#pragma once

#include <windows.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "Common.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// 依存するクラス
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "Scene.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Renderer
// レンダラークラス
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
class Renderer
{
public:
	Renderer(void);
	~Renderer(void);

	//=======================================================
	// レンダラーの初期化
	// 引数
	//		hWnd         : ウィンドウハンドル
	//		isFullScreen : フルスクリーンの場合TRUE
	//		clientWidth  : クライアント領域の幅
	//		clientHeight : クライアント領域の高さ
	// 戻り値
	//		成功したらS_OK 
	//=======================================================
	HRESULT Initialize( HWND hWnd, int clientWidth, int clientHeight );

	//=======================================================
	// デバイスの取得
	// 戻り値
	//		成功したらIDirect3DDevice9インターフェイスへのポインタ
	//		失敗したら0
	//=======================================================
	LPDIRECT3DDEVICE9 GetDevice();

	//=======================================================
	// シーンの描画
	// 引数
	//		pScene : シーンへのポインタ
	// 戻り値
	//		成功したらS_OK 
	//=======================================================
	void RenderScene(Scene* pScene);

	//=======================================================
	// テキストの描画
	//=======================================================
	void RenderText1(TCHAR str[]);
	void RenderText2(TCHAR str[]);
	void RenderText3(TCHAR str[]);

	//=======================================================
	// 終了処理
	//=======================================================
	void Finalize();

private:
	LPDIRECT3D9				m_pD3D;			// IDirect3D9インターフェイスへのポインタ
	LPDIRECT3DDEVICE9		m_pD3DDevice;	// IDirect3DDevice9インターフェイスへのポインタ
	D3DPRESENT_PARAMETERS	m_D3DPP;		// デバイスのプレゼンテーションパラメータ

	RECT                    m_rect;			// 文字を表示したい範囲
	LPD3DXFONT              m_lpFont;		// フォントオブジェクト
	double                  m_frameTime;	// 1フレームにかかった時間

public:
	// テキスト表示用
	TCHAR					m_text1[80];
	TCHAR					m_text2[80];
	TCHAR					m_text3[80];
};

