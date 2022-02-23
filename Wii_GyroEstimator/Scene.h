/*
 * @file    Scene.h
 * @brief	DirectX�̃V�[���Ǘ����s�����W���[��
 * @date	2011-2
 * @autor   Brays
 */

#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "Common.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// �ˑ�����N���X
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Scene
// �V�[���N���X
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
class Scene
{
public:
	Scene(void);
	~Scene(void);

	//=======================================================
	// �V�[���𐶐�
	// ����
	//		pD3DDevice : IDirect3DDevice9 �C���^�[�t�F�C�X�ւ̃|�C���^
	// �߂�l
	//		����������S_OK
	//=======================================================
	HRESULT Create(LPDIRECT3DDEVICE9 pD3DDevice);

	//=======================================================
	// ���W�ϊ����s��
	// ����
	//		pD3DDevice : IDirect3DDevice9 �C���^�[�t�F�C�X�ւ̃|�C���^
	//=======================================================
	void Transform(LPDIRECT3DDEVICE9 pD3DDevice);

	//=======================================================
	// �Z���T���瓾��yaw,pitch,roll���Z�b�g
	// ����
	//		yaw, pitch, roll
	//=======================================================
	void SetYawPitchRoll( float yaw, float pitch, float roll );

	//=======================================================
	// �`��
	// ����
	//		pD3DDevice : IDirect3DDevice9 �C���^�[�t�F�C�X�ւ̃|�C���^
	//=======================================================
	void Draw(LPDIRECT3DDEVICE9 pD3DDevice);

	//=======================================================
	// �V�[���̔j��
	//=======================================================
	void Destroy();

private:
	D3DXMATRIX m_proj;		// �J�����̉�p�Ȃ�
	D3DXMATRIX m_view;		// �J�����̔z�u
	D3DXMATRIX m_world;		// ���f���̔z�u

	D3DXMATRIX m_mat_model_world;	// ���f�������[���h�ϊ��s��

	LPD3DXMESH   m_pMesh;			// ID3DXMesh�C���^�[�t�F�C�X�ւ̃|�C���^
	LPD3DXBUFFER m_pD3DXMtrlBuffer;	// ���b�V���̃}�e���A�������i�[
	DWORD        m_numMtrl;			// �������̑���

	D3DLIGHT9    m_light;		// ���C�g

	float m_sensor_yaw;
	float m_sensor_pitch;
	float m_sensor_roll;
};

