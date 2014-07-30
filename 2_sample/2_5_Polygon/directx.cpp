#include <d3d9.h>							// DirectX ���g����悤�ɂ���
#include "stdafx.h"
#include "draw.h"							 // ������ �����Լ� ȣ��

// �O���[�o���ϐ� :
LPDIRECT3D9				g_pD3D    = NULL;	// Direct 3D�� ����
LPDIRECT3DDEVICE9		g_pD3DDev = NULL;	// ����ī�忡 ����

//-----------------------------------------------------------------------------
// Direct X Graphics �ʱ�ȭ
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Direct 3D ���
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) ) return E_FAIL;

    // ���÷��� ��� ����
    D3DDISPLAYMODE d3ddm;
    if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
		return E_FAIL;

    // Direct 3D ����̽� ����
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );        // ���� 0����
    d3dpp.Windowed = TRUE;                      // ��������
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;   // ȭ����ȯ ���
    d3dpp.BackBufferFormat = d3ddm.Format;      // ���� ȭ�� ���� ���

    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pD3DDev ) ) ) return E_FAIL;
	
	if(FAILED(Initialize(g_pD3DDev))) return E_FAIL; // �A�v���P�[�V�����̏�����

	return S_OK;
}
//-----------------------------------------------------------------------------
// �����Լ�
//-----------------------------------------------------------------------------
VOID Render()
{
    // ����� �˰�ĥ�Ѵ�
    g_pD3DDev->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
    
    g_pD3DDev->BeginScene(); // ���� ����
    
	Update(g_pD3DDev);			// ����
    
    g_pD3DDev->EndScene(); // ���� ����
    
    // ����� ������ ���� �����쿡 ��Ÿ���� �Ѵ�
    g_pD3DDev->Present( NULL, NULL, NULL, NULL );
}
//-----------------------------------------------------------------------------
// ��Еt��
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    Close(g_pD3DDev);			// ���ø����̼� ����

	RELEASE(g_pD3DDev);
    RELEASE(g_pD3D);
}