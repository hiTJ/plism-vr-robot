/*
 * @file    Renderer.cpp
 * @brief	DirectX�̃����_�����O���s�����W���[��
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
// �����_���[�̏�����
// ����
//		hWnd         : �E�B���h�E�n���h��
//		clientWidth  : �N���C�A���g�̈�̕�
//		clientHeight : �N���C�A���g�̈�̍���
// �߂�l
//		����������S_OK 
//=======================================================
HRESULT Renderer::Initialize(HWND hWnd, int clientWidth, int clientHeight)
{
	D3DDISPLAYMODE d3ddm;

	// Direct3D9�I�u�W�F�N�g�̍쐬
	if( ( m_pD3D = ::Direct3DCreate9(D3D_SDK_VERSION) ) == 0 ) {
		return E_FAIL;	// �擾���s
	}

	// ���݂̃f�B�X�v���C���[�h���擾
	if( FAILED( m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm) ) ) {
		return E_FAIL;
	}

	// �f�o�C�X�̃v���[���e�[�V�����p�����[�^��������
	ZeroMemory( &m_D3DPP, sizeof(D3DPRESENT_PARAMETERS) );
	m_D3DPP.BackBufferCount = 1;
	m_D3DPP.Windowed              = TRUE;			// �E�B���h�E���\���̎w��
	m_D3DPP.BackBufferFormat          = d3ddm.Format;	// �J���[���[�h�̎w��

	// �t���b�s���O�̕��@�i���������摜���ǂ̂悤�ȕ��@�Ń��j�^��ʂɓ]�����邩�j
	m_D3DPP.SwapEffect                = D3DSWAPEFFECT_DISCARD;
	
	m_D3DPP.EnableAutoDepthStencil    = TRUE;			// �G���[�΍�
	m_D3DPP.AutoDepthStencilFormat    = D3DFMT_D16;		// �G���[�΍�

	// �f�o�C�X�I�u�W�F�N�g�̐���
	// �ł��邾���n�[�h�E�F�A�I�ɏ������s���悤�ɂ���
	// �`��ƒ��_�������n�[�h�E�F�A�ōs��
	if( FAILED( m_pD3D->CreateDevice(
		D3DADAPTER_DEFAULT,						// �r�f�I�J�[�h�̎w��
		D3DDEVTYPE_HAL,							// *�f�o�C�X�̃^�C�v
		hWnd,									// �E�B���h�E�n���h��
		D3DCREATE_HARDWARE_VERTEXPROCESSING,	// *�f�o�C�X�̓��샂�[�h
		&m_D3DPP,								// D3DPRESENT_PARAMETERS�\���̂̃A�h���X
		&m_pD3DDevice							// �u�擾�����f�o�C�X�ւ̃|�C���^�v�ւ̃|�C���^
		)) ) {
			// ��L�̐ݒ肪���s������
			// �`����n�[�h�E�F�A�ōs���A���_������CPU�ōs��
			if( FAILED( m_pD3D->CreateDevice(
				D3DADAPTER_DEFAULT,						// �r�f�I�J�[�h�̎w��
				D3DDEVTYPE_HAL,							// *�f�o�C�X�̃^�C�v
				hWnd,									// �E�B���h�E�n���h��
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,	// *�f�o�C�X�̓��샂�[�h
				&m_D3DPP,								// D3DPRESENT_PARAMETERS�\���̂̃A�h���X
				&m_pD3DDevice							// �u�擾�����f�o�C�X�ւ̃|�C���^�v�ւ̃|�C���^
				)) ) {
					// ��L�̐ݒ肪���s������
					// �`��ƒ��_������CPU�ōs��
					if( FAILED( m_pD3D->CreateDevice(
						D3DADAPTER_DEFAULT,						// �r�f�I�J�[�h�̎w��
						D3DDEVTYPE_REF,							// *�f�o�C�X�̃^�C�v
						hWnd,									// �E�B���h�E�n���h��
						D3DCREATE_SOFTWARE_VERTEXPROCESSING,	// *�f�o�C�X�̓��샂�[�h
						&m_D3DPP,								// D3DPRESENT_PARAMETERS�\���̂̃A�h���X
						&m_pD3DDevice							// �u�擾�����f�o�C�X�ւ̃|�C���^�v�ւ̃|�C���^
						)) ) {
							// ���������s
							return E_FAIL;
					}
			}
	}

	// �t�H���g�I�u�W�F�N�g�̍쐬
	if( FAILED( D3DXCreateFont(
		m_pD3DDevice,				// IDrect3DDevice9�C���^�t�F�[�X(�f�o�C�X�쐬���Ă���)
		16,							// ����
		0,							// ��
		FW_HEAVY,					// ����
		1,							// �~�b�v�}�b�v���x���̐�
		FALSE,						// �Α̂��ǂ���
		DEFAULT_CHARSET,			// �����Z�b�g
		OUT_CHARACTER_PRECIS,		// �o�͐��x
		DEFAULT_QUALITY,			// �o�͕i��
		DEFAULT_PITCH | FF_SWISS,	// �s�b�`�ƃt�@�~��
		L"�l�r �o�S�V�b�N",			// �t�H���g��
		&m_lpFont					// ID3DXFont�C���^�t�F�[�X�ւ̃|�C���^
		) ) ){
			
			return E_FAIL;
	}
	
	return S_OK;
}

//=======================================================
// �f�o�C�X�̎擾
// �߂�l
//		����������IDirect3DDevice9�C���^�[�t�F�C�X�ւ̃|�C���^
//		���s������0
//=======================================================
LPDIRECT3DDEVICE9 Renderer::GetDevice()
{
	return m_pD3DDevice;
}

//=======================================================
// �V�[���̕`��
// ����
//		pScene : �V�[���ւ̃|�C���^
// �߂�l
//		����������S_OK 
//=======================================================
void Renderer::RenderScene( Scene* pScene )
{
	// �t���[�����[�g�v��
	static DWORD time = timeGetTime();		// �P�ʂ�[ms]
	// 1�t���[��������̎��Ԍv��
	m_frameTime = timeGetTime() - time;
	time = timeGetTime();

	if( pScene == 0 ) {
		return;
	}

	// �r���[�|�[�g�Ɛ[�x�o�b�t�@�̃N���A�ƃX�e���V���o�b�t�@�̍폜
	if( FAILED( m_pD3DDevice->Clear( 
							0,							// ���̈����ł���pRects�̔z��ɂ����`�̐��BpRects��NULL�ɐݒ肷��ꍇ�́A���̃p�����[�^�� 0 �ɐݒ肷��B 
							NULL,						// �N���A����̈�͑S��
							D3DCLEAR_TARGET |			// �o�b�N�o�b�t�@�����w��
							D3DCLEAR_ZBUFFER,			// �[�x�o�b�t�@�iZ�o�b�t�@�j���w��
							D3DCOLOR_XRGB( 0, 0, 255 ),	// �N���A����F
							1.0f,						// �N���A����[�x�o�b�t�@�iZ�o�b�t�@�̒l�j
							0							// �N���A����X�e���V���o�b�t�@�̒l
							)) ){
		return;
	}

	// �`��J�n�錾
	if( SUCCEEDED( m_pD3DDevice->BeginScene() ) ) {
		// ���f���Ȃǂ̕`��
		pScene->Draw( m_pD3DDevice );

		// �e�L�X�g�̕`��
		_stprintf_s( m_text1, TEXT("FPS:%f"), 1000.0 / m_frameTime );
		RenderText1( m_text1 );
		RenderText2( m_text2 );
		RenderText3( m_text3 );

		// �`��I���錾
		m_pD3DDevice->EndScene();
	}

	//----- ���̎��_�ŃV�[���̓o�b�N�o�b�t�@�ɏ������܂�Ă��� -----//

	// �`�挋�ʂ��f�B�X�v���C�ɓ]��
	if( FAILED( m_pD3DDevice->Present( 0, 0, 0, 0 ) ) ) {	// �o�b�N�o�b�t�@�̕\��
		// �f�o�C�X�������畜�A
		m_pD3DDevice->Reset( &m_D3DPP );
	}



}

//=======================================================
// �e�L�X�g�̕`��
//=======================================================
void Renderer::RenderText1(TCHAR str[])
{
	// �t�H���g�̕`��͈͂̎w��
	m_rect.left   = 5;
	m_rect.top    = 5;
	m_rect.right  = 250;
	m_rect.bottom = 70;

	m_lpFont->DrawText( NULL, str, -1, &m_rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB( 255, 255, 255, 0 ) );
}

void Renderer::RenderText2(TCHAR str[])
{
	// �t�H���g�̕`��͈͂̎w��
	m_rect.left   = 5;
	m_rect.top    = 21;
	m_rect.right  = 250;
	m_rect.bottom = 70;

	m_lpFont->DrawText( NULL, str, -1, &m_rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB( 255, 255, 255, 0 ) );
}

void Renderer::RenderText3(TCHAR str[])
{
	// �t�H���g�̕`��͈͂̎w��
	m_rect.left   = 5;
	m_rect.top    = 37;
	m_rect.right  = 250;
	m_rect.bottom = 70;

	m_lpFont->DrawText( NULL, str, -1, &m_rect, DT_LEFT | DT_NOCLIP, D3DCOLOR_ARGB( 255, 255, 255, 0 ) );
}

//=======================================================
// �I������
//=======================================================
void Renderer::Finalize()
{
	// �f�o�C�X�I�u�W�F�N�g�̉��
	SAFE_RELEASE( m_pD3DDevice );

	// DirectXGraphics�̉��
	SAFE_RELEASE( m_pD3D );

	// �t�H���g�I�u�W�F�N�g�̉��
	SAFE_RELEASE( m_lpFont );
}