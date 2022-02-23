/*
 * @file    Scene.cpp
 * @brief	DirectXのシーン管理を行うモジュール
 * @date	2011-2
 * @autor   Brays
 */

#include "Scene.h"

#include <windows.h>
#include <mmsystem.h> // for timeGetTime()

#include <d3d9.h>
#include <d3dx9.h>

Scene::Scene(void) : m_pMesh(0), m_numMtrl(0L), m_pD3DXMtrlBuffer(0)
{
}


Scene::~Scene(void)
{
	Destroy();
}

//=======================================================
// シーンを生成
// 引数
//		pD3DDevice : IDirect3DDevice9 インターフェイスへのポインタ
// 戻り値
//		成功したらS_OK
//=======================================================
HRESULT Scene::Create(LPDIRECT3DDEVICE9 pD3DDevice)
{
	// 各種変数の初期化
	Destroy();

	if( pD3DDevice == 0 ) {
		return E_FAIL;
	}

	// Xファイルの読み込み
	if( FAILED( D3DXLoadMeshFromX( 
							TEXT( "wiiRemote_only.x" ),	// Xファイルのファイル名
							D3DXMESH_SYSTEMMEM,	// メッシュをシステムメモリに展開する
							pD3DDevice,			// D3Dデバイスオブジェクト
							NULL,				// 
							&m_pD3DXMtrlBuffer,	// 属性バッファの設定
							NULL,
							&m_numMtrl,			// 属性の数
							&m_pMesh			// 格納先のメッシュオブジェクト
							) ) ) {
		MessageBox( NULL, L"Xファイルの読み込みに失敗しました", L"エラー", MB_OK );
		return E_FAIL;
	}

	//----------------------------------------------------
	// レンダリングステートパラメータの設定
	// レンダリング方法の設定、各種マトリックスはシーンの見え方に関する設定
	//----------------------------------------------------
	// 両面描画モードの指定
	pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	// Z比較を行う
	pD3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	// ディザリングを行う（高品質描画）
	pD3DDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
	// ライティングモード
	pD3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	// スぺキュラー
	pD3DDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );


	//----------------------------------------------------
	// ライトの設定
	//----------------------------------------------------
	// D3DLIGHT9構造体を0でクリアする
	ZeroMemory( &m_light, sizeof(D3DLIGHT9) );

	// ライトのタイプの設定（平行ライト）
	m_light.Type      = D3DLIGHT_DIRECTIONAL;

	// ライトの色指定
	m_light.Diffuse.r = 1.0f;
	m_light.Diffuse.g = 1.0f;
	m_light.Diffuse.b = 1.0f;

	// 環境光
	m_light.Ambient.r = 0.5f;
	m_light.Ambient.g = 0.5f;
	m_light.Ambient.b = 0.5f;

	// 鏡面反射光
	m_light.Specular.r = 1.0f;
	m_light.Specular.g = 1.0f;
	m_light.Specular.b = 1.0f;

	// ライトの方向の設定
	D3DXVECTOR3 mVecDir;
	mVecDir = D3DXVECTOR3( -5.0f, -5.0f, 10.0f );
	D3DXVec3Normalize( (D3DXVECTOR3*)&m_light.Direction, &mVecDir );

	// ライトをレンダリングパイプラインに設定
	// 複数のライトをセットすることができるので、引数①はライトの番号を指定している
	pD3DDevice->SetLight( 0, &m_light );

	// ライトを有効にする
	// 複数のライトをセットすることができるので、引数①はライトの番号を指定している
	pD3DDevice->LightEnable( 0 , TRUE );

	return S_OK;
}

//=======================================================
// 座標変換を行う
// 引数
//		pD3DDevice : IDirect3DDevice9 インターフェイスへのポインタ
//=======================================================
void Scene::Transform( LPDIRECT3DDEVICE9 pD3DDevice )
{
	if( pD3DDevice == 0 ) {
		return;
	}

	//----------------------------------------------------
	// ビューマトリックスの設定
	//----------------------------------------------------

	// 初期化
	D3DXMatrixIdentity( &m_view );

	// カメラの位置と方向を設定
	D3DXMatrixLookAtLH( &m_view,
		&D3DXVECTOR3( 0.0f, 2.0f, -10.0f ),		// カメラの位置
		&D3DXVECTOR3( 0.0f, 2.0f, 0.0f ),		// カメラの向き（カメラの方向を向ける点）
		&D3DXVECTOR3( 0.0f, 1.0f, 0.0f )		// カメラの傾き（カメラの上の方向）
		);

	// マトリックスをレンダリングパイプラインに設定
	pD3DDevice->SetTransform( D3DTS_VIEW, &m_view );


	//----------------------------------------------------
	// 射影マトリックスの設定
	//----------------------------------------------------

	// ビューポートの取得
	D3DVIEWPORT9 vp;
	if( FAILED( pD3DDevice->GetViewport( &vp ) ) ) {
		return;
	}

	// アスペクト比の計算
	float aspect;
	aspect = (float)vp.Width / (float)vp.Height;

	// 射影行列の初期化
	D3DXMatrixIdentity( &m_proj );

	// 射影マトリックスを作成
	D3DXMatrixPerspectiveFovLH( &m_proj,
		D3DXToRadian(45.0f),	// カメラの画角
		aspect,					// アスペクト比
		1.0f,					// nearプレーン
		1000.0f					// farプレーン
		);

	// 射影行列の設定
	pD3DDevice->SetTransform( D3DTS_PROJECTION, &m_proj );
}

//=======================================================
// センサから得たyaw,pitch,rollをセット
// 引数
//		yaw, pitch, roll
//=======================================================
void Scene::SetYawPitchRoll( float yaw, float pitch, float roll ){
	m_sensor_yaw   = yaw;
	m_sensor_pitch = -pitch;
	m_sensor_roll  = -roll;

	// モデル→ワールド変換行列を計算
	D3DXQUATERNION wv_qt;
	D3DXQuaternionRotationYawPitchRoll( &wv_qt, D3DXToRadian(m_sensor_yaw), D3DXToRadian(m_sensor_pitch), D3DXToRadian(m_sensor_roll) );
	D3DXMatrixRotationQuaternion( &m_mat_model_world, &wv_qt );
}

//=======================================================
// オブジェクト等の描画
// 引数
//		pD3DDevice : IDirect3DDevice9 インターフェイスへのポインタ
//=======================================================
void Scene::Draw( LPDIRECT3DDEVICE9 pD3DDevice )
{
	if( pD3DDevice == 0 ) {
		return;
	}

	if( m_pMesh == 0 ) {
		return;
	}

	// 座標変換（カメラの設定など）
	Transform( pD3DDevice );

	// ワールドマトリックスの設定(何もしないマトリックスを生成）
	D3DXMatrixIdentity( &m_world );

	// 初期姿勢
	D3DXMatrixRotationY( &m_world, D3DXToRadian(90) );

	// 掛け算
	D3DXMatrixMultiply( &m_world, &m_world, &m_mat_model_world );

	// マトリックスをレンダリングパイプラインに設定
	pD3DDevice->SetTransform( D3DTS_WORLD, &m_world );

	//----------------------------------------------------
	// モデルの描画
	//----------------------------------------------------
	D3DMATERIAL9 m_Material;
	// マテリアル情報を取り出す
	D3DXMATERIAL* m_D3DXMaterials = (D3DXMATERIAL*)m_pD3DXMtrlBuffer->GetBufferPointer();
	for( DWORD i=0; i<m_numMtrl; i++ ) {
		// マテリアルのコピー
		m_Material = m_D3DXMaterials[i].MatD3D;
		// マテリアルのセット
		pD3DDevice->SetMaterial( &m_Material );
		// 分割されたメッシュの描画
		m_pMesh->DrawSubset( i );
	}
}

//=======================================================
// シーンの破棄
//=======================================================
void Scene::Destroy()
{
	// 属性バッファの解放
	SAFE_RELEASE( m_pD3DXMtrlBuffer );

	// メッシュの解放
	SAFE_RELEASE(m_pMesh);
}