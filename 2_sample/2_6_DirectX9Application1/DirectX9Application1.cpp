//-----------------------------------------------------------------------------
// File: DirectX9Application1.cpp
//
// Desc: DirectX AppWizard������ ������ ������ ���ø����̼�
//-----------------------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "resource.h"
#include "DirectX9Application1.h"



//-----------------------------------------------------------------------------
// ��������
//-----------------------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: �����Լ�. ��� �ʱ�ȭó���� �޽��������� ����.
//       ��հŸ��� ���� ȭ�� ������ó��
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}




//-----------------------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: ���ø����̼� ������. ~CMyD3DApplication()�� �����Ѵ�.
//       ��������� ���⼭ �ʱ�ȭ. ���ø����̼� ������� ����
//       �����Ǿ����� �ʴ�. DirectX����̽��� ���� ����. 
//       ������� Direct3D�� �����ϴ� �κ��� �ʱ�ȭ�� �̰����� �ϸ� �ȵ�.
//
//-----------------------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "DirectX9Application1" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // d3dfont.cpp �� ����� ��Ʈ
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
    m_pD3DXMesh                 = NULL;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = 0.0f;
    m_fWorldRotY                = 0.0f;
}




//-----------------------------------------------------------------------------
// Name: ~CMyD3DApplication()
// Desc: �Ҹ���.   CMyD3DApplication()�� ����
//-----------------------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: FinalCleanup()�� ����
//       �����쵵 �����Ǿ�������, IDirect3D9�� �����Ǿ�������
//       ����̽��� ���� �����Ǿ� ���� �ʴ�.
//       ���⼭�� ����̽��� �������� �ʴ� ���ø����̼�
//       �ʱ�ȭ�� ó���Ѵ�
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    // TODO: �ѹ��� �ʱ�ȭ��

    // �ε� �޽��� ���
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    m_bLoadingApp = FALSE;

    return S_OK;
}









//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: ����̽� �ʱ�ȭ�߿� �Ҹ�. ���ø����̼��� ���������� �۵��ϱ� ����
//       �������� ����� �����ϴµ� ����
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
    BOOL bCapsAcceptable;

    // TODO: ȣ��� ����̽��� �ɷ�(CAPS)�� �����ϴ°� �����Ѵ�
    bCapsAcceptable = TRUE;

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: DeleteDeviceObjects()�� ����
//       ����̽� �����Ǿ� ����
//       D3DPOOL_MANAGED, D3DPOOL_SCRATCH, D3DPOOL_SYSTEMMEM �÷��װ�
//       ������ ������Ʈó�� Reset()ȣ�⿡ ���ؼ��� �ҽǵ��� �ʴ� ������Ʈ��
//       �̰����� �����Ѵ�. CreateImageSurface()�� ����ؼ� ������ 
//       ǥ���� ����� �ҽǵ��� �����Ƿ�, ���⼭ �����ϴ� ���� �����ϴ�.
//       �������̴��� �ȼ����̴��� �ҽǵ��� �����Ƿ� ���������ϴ�.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    // TODO: ����̽��� �����ϴ� ������Ʈ ����

    HRESULT hr;

    // ��Ʈ �ʱ�ȭ
    m_pFont->InitDeviceObjects( m_pd3dDevice );

    // D3DX�� ����ؼ� ������ ������� ����
    if( FAILED( hr = D3DXCreateTeapot( m_pd3dDevice, &m_pD3DXMesh, NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateTeapot", hr );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: InvalidateDeviceObjects()�� ����
//       ����̽��� ���������� �ҽǻ�����. D3DPOOL_DEFAULT ���ҽ���
//       �������߿� �����ϴ� ���,�ؽ�ó ���� ������Ʈ�� ���⼭ ����.
//       �������� ������ �ʴ� �͵��� ���⼭ �����صθ� ����
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
    // TODO: ������ ȯ�� ����

    // ���� ����
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // �ؽ�ó ����
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    // ������ ȯ�� & ����� ����
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x000F0F0F );

    // ������� ����
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

    // ���࿭ ����
    D3DXMATRIX matView;
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // ������� ����
    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // ���� ����
    D3DLIGHT9 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // ��Ʈ �籸��
    m_pFont->RestoreDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: �� �����Ӹ��� �ѹ��� ȣ��ȴ�.
//       �ִϸ��̼� ����� ó���ϴ� ��
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
    // TODO: ���踦 �����Ѵ�

    // ����� �Է� ����
    UpdateInput( &m_UserInput );

    // ����� �Է¿� ���� �������� ����
    D3DXMATRIX matWorld;
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;

    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;

    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );

    D3DXMatrixMultiply( &matWorld, &matRotX, &matRotY );
    m_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: UpdateInput()
// Desc: ����� �Է� ����. �� �����Ӹ��� �ѹ��� ȣ��
//-----------------------------------------------------------------------------
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    pUserInput->bRotateUp    = ( m_bActive && (GetAsyncKeyState( VK_UP )    & 0x8000) == 0x8000 );
    pUserInput->bRotateDown  = ( m_bActive && (GetAsyncKeyState( VK_DOWN )  & 0x8000) == 0x8000 );
    pUserInput->bRotateLeft  = ( m_bActive && (GetAsyncKeyState( VK_LEFT )  & 0x8000) == 0x8000 );
    pUserInput->bRotateRight = ( m_bActive && (GetAsyncKeyState( VK_RIGHT ) & 0x8000) == 0x8000 );
}




//-----------------------------------------------------------------------------
// Name: Render()
// Desc: �� �����Ӹ��� �ѹ��� ȣ��. �������� ���۵Ǵ� ��
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    // ����Ʈ �ʱ�ȭ
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x000000ff, 1.0f, 0L );

    // ������ ����
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        // TODO: ���� ������
        
        // ������ ���
        m_pD3DXMesh->DrawSubset(0);

        // ���� & ���� ���
        RenderText();

        // ������ ����
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: ���� & ���� ���
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    // ���÷��� ���� ���
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    // ���� ���
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    wsprintf( szMsg, TEXT("Arrow keys: Up=%d Down=%d Left=%d Right=%d"), 
              m_UserInput.bRotateUp, m_UserInput.bRotateDown, m_UserInput.bRotateLeft, m_UserInput.bRotateRight );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Use arrow keys to rotate object") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: WndProc �������̵� �Ѱ�. �������� �̺�Ʈ ó��
//       (��, ���콺,Ű����,�޴�ó�� ��)
//-----------------------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
{
    switch( msg )
    {
        case WM_PAINT:
        {
            if( m_bLoadingApp )
            {
                // �ε������� ����ڿ��� ���Ѵ�
                // TODO: �ʿ�ÿ� ����
                HDC hDC = GetDC( hWnd );
                TCHAR strMsg[MAX_PATH];
                wsprintf( strMsg, TEXT("Loading... Please wait") );
                RECT rct;
                GetClientRect( hWnd, &rct );
                DrawText( hDC, strMsg, -1, &rct, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
                ReleaseDC( hWnd, hDC );
            }
            break;
        }

    }

    return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: RestoreDeviceObjects()�� ����. ����̽����� ������Ʈ�� ��ȿȭ
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    // TODO: RestoreDeviceObjects()���� ������ ��� ������Ʈ ����
    m_pFont->InvalidateDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects()�� ����
//       ���ø����̼��� ����ǰų� ����̽��� ����ɰ�� ȣ��
//       ����̽��� �����ϴ� ��� ������Ʈ ����
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    // TODO: InitDeviceObjects()���� ������ ��� ������Ʈ ����
    m_pFont->DeleteDeviceObjects();
    SAFE_RELEASE( m_pD3DXMesh );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: OneTimeSceneInit()�� ����
//       ���ø����̼� ���������� ȣ��
//       �������� ���� ������ �͵鿡 ���� ó��
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    // ��Ʈ ����
    SAFE_DELETE( m_pFont );

    return S_OK;
}




