/*
 * @file    Scene.h
 * @brief	DirectXのシーン管理を行うモジュール
 * @date	2011-2
 * @autor   Brays
 */

#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "Common.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// 依存するクラス
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Scene
// シーンクラス
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
class Scene
{
public:
	Scene(void);
	~Scene(void);

	//=======================================================
	// シーンを生成
	// 引数
	//		pD3DDevice : IDirect3DDevice9 インターフェイスへのポインタ
	// 戻り値
	//		成功したらS_OK
	//=======================================================
	HRESULT Create(LPDIRECT3DDEVICE9 pD3DDevice);

	//=======================================================
	// 座標変換を行う
	// 引数
	//		pD3DDevice : IDirect3DDevice9 インターフェイスへのポインタ
	//=======================================================
	void Transform(LPDIRECT3DDEVICE9 pD3DDevice);

	//=======================================================
	// センサから得たyaw,pitch,rollをセット
	// 引数
	//		yaw, pitch, roll
	//=======================================================
	void SetYawPitchRoll( float yaw, float pitch, float roll );

	//=======================================================
	// 描画
	// 引数
	//		pD3DDevice : IDirect3DDevice9 インターフェイスへのポインタ
	//=======================================================
	void Draw(LPDIRECT3DDEVICE9 pD3DDevice);

	//=======================================================
	// シーンの破棄
	//=======================================================
	void Destroy();

private:
	D3DXMATRIX m_proj;		// カメラの画角など
	D3DXMATRIX m_view;		// カメラの配置
	D3DXMATRIX m_world;		// モデルの配置

	D3DXMATRIX m_mat_model_world;	// モデル→ワールド変換行列

	LPD3DXMESH   m_pMesh;			// ID3DXMeshインターフェイスへのポインタ
	LPD3DXBUFFER m_pD3DXMtrlBuffer;	// メッシュのマテリアル情報を格納
	DWORD        m_numMtrl;			// 属性情報の総数

	D3DLIGHT9    m_light;		// ライト

	float m_sensor_yaw;
	float m_sensor_pitch;
	float m_sensor_roll;
};

