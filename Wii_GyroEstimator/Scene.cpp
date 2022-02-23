/*
 * @file    Scene.cpp
 * @brief	DirectX�̃V�[���Ǘ����s�����W���[��
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
// �V�[���𐶐�
// ����
//		pD3DDevice : IDirect3DDevice9 �C���^�[�t�F�C�X�ւ̃|�C���^
// �߂�l
//		����������S_OK
//=======================================================
HRESULT Scene::Create(LPDIRECT3DDEVICE9 pD3DDevice)
{
	// �e��ϐ��̏�����
	Destroy();

	if( pD3DDevice == 0 ) {
		return E_FAIL;
	}

	// X�t�@�C���̓ǂݍ���
	if( FAILED( D3DXLoadMeshFromX( 
							TEXT( "wiiRemote_only.x" ),	// X�t�@�C���̃t�@�C����
							D3DXMESH_SYSTEMMEM,	// ���b�V�����V�X�e���������ɓW�J����
							pD3DDevice,			// D3D�f�o�C�X�I�u�W�F�N�g
							NULL,				// 
							&m_pD3DXMtrlBuffer,	// �����o�b�t�@�̐ݒ�
							NULL,
							&m_numMtrl,			// �����̐�
							&m_pMesh			// �i�[��̃��b�V���I�u�W�F�N�g
							) ) ) {
		MessageBox( NULL, L"X�t�@�C���̓ǂݍ��݂Ɏ��s���܂���", L"�G���[", MB_OK );
		return E_FAIL;
	}

	//----------------------------------------------------
	// �����_�����O�X�e�[�g�p�����[�^�̐ݒ�
	// �����_�����O���@�̐ݒ�A�e��}�g���b�N�X�̓V�[���̌������Ɋւ���ݒ�
	//----------------------------------------------------
	// ���ʕ`�惂�[�h�̎w��
	pD3DDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	// Z��r���s��
	pD3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	// �f�B�U�����O���s���i���i���`��j
	pD3DDevice->SetRenderState( D3DRS_DITHERENABLE, TRUE );
	// ���C�e�B���O���[�h
	pD3DDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	// �X�؃L�����[
	pD3DDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );


	//----------------------------------------------------
	// ���C�g�̐ݒ�
	//----------------------------------------------------
	// D3DLIGHT9�\���̂�0�ŃN���A����
	ZeroMemory( &m_light, sizeof(D3DLIGHT9) );

	// ���C�g�̃^�C�v�̐ݒ�i���s���C�g�j
	m_light.Type      = D3DLIGHT_DIRECTIONAL;

	// ���C�g�̐F�w��
	m_light.Diffuse.r = 1.0f;
	m_light.Diffuse.g = 1.0f;
	m_light.Diffuse.b = 1.0f;

	// ����
	m_light.Ambient.r = 0.5f;
	m_light.Ambient.g = 0.5f;
	m_light.Ambient.b = 0.5f;

	// ���ʔ��ˌ�
	m_light.Specular.r = 1.0f;
	m_light.Specular.g = 1.0f;
	m_light.Specular.b = 1.0f;

	// ���C�g�̕����̐ݒ�
	D3DXVECTOR3 mVecDir;
	mVecDir = D3DXVECTOR3( -5.0f, -5.0f, 10.0f );
	D3DXVec3Normalize( (D3DXVECTOR3*)&m_light.Direction, &mVecDir );

	// ���C�g�������_�����O�p�C�v���C���ɐݒ�
	// �����̃��C�g���Z�b�g���邱�Ƃ��ł���̂ŁA�����@�̓��C�g�̔ԍ����w�肵�Ă���
	pD3DDevice->SetLight( 0, &m_light );

	// ���C�g��L���ɂ���
	// �����̃��C�g���Z�b�g���邱�Ƃ��ł���̂ŁA�����@�̓��C�g�̔ԍ����w�肵�Ă���
	pD3DDevice->LightEnable( 0 , TRUE );

	return S_OK;
}

//=======================================================
// ���W�ϊ����s��
// ����
//		pD3DDevice : IDirect3DDevice9 �C���^�[�t�F�C�X�ւ̃|�C���^
//=======================================================
void Scene::Transform( LPDIRECT3DDEVICE9 pD3DDevice )
{
	if( pD3DDevice == 0 ) {
		return;
	}

	//----------------------------------------------------
	// �r���[�}�g���b�N�X�̐ݒ�
	//----------------------------------------------------

	// ������
	D3DXMatrixIdentity( &m_view );

	// �J�����̈ʒu�ƕ�����ݒ�
	D3DXMatrixLookAtLH( &m_view,
		&D3DXVECTOR3( 0.0f, 2.0f, -10.0f ),		// �J�����̈ʒu
		&D3DXVECTOR3( 0.0f, 2.0f, 0.0f ),		// �J�����̌����i�J�����̕�����������_�j
		&D3DXVECTOR3( 0.0f, 1.0f, 0.0f )		// �J�����̌X���i�J�����̏�̕����j
		);

	// �}�g���b�N�X�������_�����O�p�C�v���C���ɐݒ�
	pD3DDevice->SetTransform( D3DTS_VIEW, &m_view );


	//----------------------------------------------------
	// �ˉe�}�g���b�N�X�̐ݒ�
	//----------------------------------------------------

	// �r���[�|�[�g�̎擾
	D3DVIEWPORT9 vp;
	if( FAILED( pD3DDevice->GetViewport( &vp ) ) ) {
		return;
	}

	// �A�X�y�N�g��̌v�Z
	float aspect;
	aspect = (float)vp.Width / (float)vp.Height;

	// �ˉe�s��̏�����
	D3DXMatrixIdentity( &m_proj );

	// �ˉe�}�g���b�N�X���쐬
	D3DXMatrixPerspectiveFovLH( &m_proj,
		D3DXToRadian(45.0f),	// �J�����̉�p
		aspect,					// �A�X�y�N�g��
		1.0f,					// near�v���[��
		1000.0f					// far�v���[��
		);

	// �ˉe�s��̐ݒ�
	pD3DDevice->SetTransform( D3DTS_PROJECTION, &m_proj );
}

//=======================================================
// �Z���T���瓾��yaw,pitch,roll���Z�b�g
// ����
//		yaw, pitch, roll
//=======================================================
void Scene::SetYawPitchRoll( float yaw, float pitch, float roll ){
	m_sensor_yaw   = yaw;
	m_sensor_pitch = -pitch;
	m_sensor_roll  = -roll;

	// ���f�������[���h�ϊ��s����v�Z
	D3DXQUATERNION wv_qt;
	D3DXQuaternionRotationYawPitchRoll( &wv_qt, D3DXToRadian(m_sensor_yaw), D3DXToRadian(m_sensor_pitch), D3DXToRadian(m_sensor_roll) );
	D3DXMatrixRotationQuaternion( &m_mat_model_world, &wv_qt );
}

//=======================================================
// �I�u�W�F�N�g���̕`��
// ����
//		pD3DDevice : IDirect3DDevice9 �C���^�[�t�F�C�X�ւ̃|�C���^
//=======================================================
void Scene::Draw( LPDIRECT3DDEVICE9 pD3DDevice )
{
	if( pD3DDevice == 0 ) {
		return;
	}

	if( m_pMesh == 0 ) {
		return;
	}

	// ���W�ϊ��i�J�����̐ݒ�Ȃǁj
	Transform( pD3DDevice );

	// ���[���h�}�g���b�N�X�̐ݒ�(�������Ȃ��}�g���b�N�X�𐶐��j
	D3DXMatrixIdentity( &m_world );

	// �����p��
	D3DXMatrixRotationY( &m_world, D3DXToRadian(90) );

	// �|���Z
	D3DXMatrixMultiply( &m_world, &m_world, &m_mat_model_world );

	// �}�g���b�N�X�������_�����O�p�C�v���C���ɐݒ�
	pD3DDevice->SetTransform( D3DTS_WORLD, &m_world );

	//----------------------------------------------------
	// ���f���̕`��
	//----------------------------------------------------
	D3DMATERIAL9 m_Material;
	// �}�e���A���������o��
	D3DXMATERIAL* m_D3DXMaterials = (D3DXMATERIAL*)m_pD3DXMtrlBuffer->GetBufferPointer();
	for( DWORD i=0; i<m_numMtrl; i++ ) {
		// �}�e���A���̃R�s�[
		m_Material = m_D3DXMaterials[i].MatD3D;
		// �}�e���A���̃Z�b�g
		pD3DDevice->SetMaterial( &m_Material );
		// �������ꂽ���b�V���̕`��
		m_pMesh->DrawSubset( i );
	}
}

//=======================================================
// �V�[���̔j��
//=======================================================
void Scene::Destroy()
{
	// �����o�b�t�@�̉��
	SAFE_RELEASE( m_pD3DXMtrlBuffer );

	// ���b�V���̉��
	SAFE_RELEASE(m_pMesh);
}