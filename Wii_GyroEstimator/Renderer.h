/*
 * @file    Renderer.h
 * @brief	DirectX�̃����_�����O���s�����W���[��
 * @date	2011-2
 * @autor   Brays
 */

#pragma once

#include <windows.h>

#include <d3d9.h>
#include <d3dx9.h>

#include "Common.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// �ˑ�����N���X
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
#include "Scene.h"

//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
// Renderer
// �����_���[�N���X
//-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
class Renderer
{
public:
	Renderer(void);
	~Renderer(void);

	//=======================================================
	// �����_���[�̏�����
	// ����
	//		hWnd         : �E�B���h�E�n���h��
	//		isFullScreen : �t���X�N���[���̏ꍇTRUE
	//		clientWidth  : �N���C�A���g�̈�̕�
	//		clientHeight : �N���C�A���g�̈�̍���
	// �߂�l
	//		����������S_OK 
	//=======================================================
	HRESULT Initialize( HWND hWnd, int clientWidth, int clientHeight );

	//=======================================================
	// �f�o�C�X�̎擾
	// �߂�l
	//		����������IDirect3DDevice9�C���^�[�t�F�C�X�ւ̃|�C���^
	//		���s������0
	//=======================================================
	LPDIRECT3DDEVICE9 GetDevice();

	//=======================================================
	// �V�[���̕`��
	// ����
	//		pScene : �V�[���ւ̃|�C���^
	// �߂�l
	//		����������S_OK 
	//=======================================================
	void RenderScene(Scene* pScene);

	//=======================================================
	// �e�L�X�g�̕`��
	//=======================================================
	void RenderText1(TCHAR str[]);
	void RenderText2(TCHAR str[]);
	void RenderText3(TCHAR str[]);

	//=======================================================
	// �I������
	//=======================================================
	void Finalize();

private:
	LPDIRECT3D9				m_pD3D;			// IDirect3D9�C���^�[�t�F�C�X�ւ̃|�C���^
	LPDIRECT3DDEVICE9		m_pD3DDevice;	// IDirect3DDevice9�C���^�[�t�F�C�X�ւ̃|�C���^
	D3DPRESENT_PARAMETERS	m_D3DPP;		// �f�o�C�X�̃v���[���e�[�V�����p�����[�^

	RECT                    m_rect;			// ������\���������͈�
	LPD3DXFONT              m_lpFont;		// �t�H���g�I�u�W�F�N�g
	double                  m_frameTime;	// 1�t���[���ɂ�����������

public:
	// �e�L�X�g�\���p
	TCHAR					m_text1[80];
	TCHAR					m_text2[80];
	TCHAR					m_text3[80];
};

