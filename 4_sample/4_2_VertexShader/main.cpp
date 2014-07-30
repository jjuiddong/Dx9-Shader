//-------------------------------------------------------------
// File: main.cpp
//
// Desc: �ؽ�ó ������
//-------------------------------------------------------------
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
#include "main.h"



//-------------------------------------------------------------
// ��������
//-------------------------------------------------------------
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ)

typedef struct {
    FLOAT x, y, z;    // ��ũ����ǥ���� ��ġ
} CUSTOMVERTEX;



//-------------------------------------------------------------
// ��������
//-------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;


//-------------------------------------------------------------
// Name: WinMain()
// Desc: �����Լ�
//-------------------------------------------------------------
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




//-------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: ���ø����̼� ������
//-------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    // �� ���̴� Ŭ����
    m_pVertexShader    = NULL;
    m_pVertexDeclaration = NULL;

    m_pVB                       = NULL;

    m_dwCreationWidth           = 300;
    m_dwCreationHeight          = 300;
    m_strWindowTitle            = TEXT( "main" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
    m_bStartFullscreen          = false;
    m_bShowCursorWhenFullscreen = false;

    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
}




//-------------------------------------------------------------
// Name: ~CMyD3DApplication()
// Desc: �Ҹ���
//-------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
}




//-------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: �� �ѹ��� �ʱ�ȭ
//       ������ �ʱ�ȭ�� IDirect3D9�ʱ�ȭ�� ������
//       �׷��� LPDIRECT3DDEVICE9�ʱ�ȭ�� ������ ���� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    // �ε� �޽��� ���
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    m_bLoadingApp = FALSE;

    return S_OK;
}




//-------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: �ʱ�ȭ�� ȣ���. �ʿ��� �ɷ�(caps)üũ
//-------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps,
                     DWORD dwBehavior,    D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    
    BOOL bCapsAcceptable;

    // �׷��Ⱥ��尡 ���α׷��� ������ �ɷ��� �ִ°� Ȯ��
    bCapsAcceptable = TRUE;

    // �� �������̴��� ���� Ȯ��
    if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
        (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
    {
        if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
            bCapsAcceptable = FALSE;
    }

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
}




//-------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: ����̽��� ���������� �ʱ�ȭ
//       �����ӹ��� ���˰� ����̽� ������ ���ѵڿ� ȣ��
//       ���⼭ Ȯ���� �޸𸮴� DeleteDeviceObjects()���� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    HRESULT hr;
    LPD3DXBUFFER pCode;
    
    // ---------------------------------------------------------
    // �� �������̴� ����
    // ---------------------------------------------------------

    // �������� ������Ʈ ����
    D3DVERTEXELEMENT9 decl[] = {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,
                 D3DDECLUSAGE_POSITION, 0 },
        D3DDECL_END()
    };
    
    if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( decl, &m_pVertexDeclaration ) ) ) {
        return hr;
    }

    // ���̴� �б�
    if( FAILED( hr = D3DXAssembleShaderFromFile(
                        _T("VertexShader.vsh"), NULL, NULL, 0, &pCode, NULL ))) {
        return hr;
    }

    // ���� ���̴� ����
    hr = m_pd3dDevice->CreateVertexShader(
                          (DWORD*)pCode->GetBufferPointer(),
                          &m_pVertexShader );
    pCode->Release();
    if( FAILED(hr) ) return hr;

	// ��Ʈ
    m_pFont->InitDeviceObjects( m_pd3dDevice );

    return S_OK;
}

//-------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: ȭ��ũ�Ⱑ �������� ȣ���
//       Ȯ���� �޸𸮴� InvalidateDeviceObjects()���� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
    // ������ ������
    CUSTOMVERTEX vertices[] = {
        //   x,     y,      z,  
        {-0.5f, +0.5f, 0},
        {+0.5f, +0.5f, 0},
        {-0.5f, -0.5f, 0},
        {+0.5f, -0.5f, 0},
    };
    
    // �������� ����
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( 
                4*sizeof(CUSTOMVERTEX),        // �������� ũ��
                0, D3DFVF_CUSTOMVERTEX,        // ����, ��������
                D3DPOOL_DEFAULT,            // �޸� Ŭ����
                &m_pVB, NULL )))            // �������� ���ҽ�
        return E_FAIL;

    // �������ۿ� ���� ����
    VOID* pVertices;
    if(FAILED( m_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
        return E_FAIL;
    memcpy( pVertices, vertices, sizeof(vertices) );
    m_pVB->Unlock();

    // �����ũ��
    #define RS   m_pd3dDevice->SetRenderState
    #define TSS  m_pd3dDevice->SetTextureStageState
    #define SAMP m_pd3dDevice->SetSamplerState

    // ������ ���¼���
    RS( D3DRS_DITHERENABLE,   FALSE );
    RS( D3DRS_SPECULARENABLE, FALSE );
    RS( D3DRS_ZENABLE,        TRUE );
    RS( D3DRS_AMBIENT,        0x000F0F0F );
    
    TSS( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    TSS( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

    // ��Ʈ
    m_pFont->RestoreDeviceObjects();

    // ����� ����
    D3DXVECTOR3 vEye = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    D3DXVECTOR3 vAt  = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp  = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtRH( &m_matView, &vEye, &vAt, &vUp );

    // ������� ����
    FLOAT fAspectRatio = (FLOAT)m_d3dsdBackBuffer.Width
                       / (FLOAT)m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovRH( &m_matProj, D3DXToRadian(60.0f),
                                fAspectRatio, 0.1f, 100.0f );

	return S_OK;
}




//-------------------------------------------------------------
// Name: FrameMove()
// Desc: �� �����Ӹ��� ȣ���. �ִϸ��̼� ó���� ���
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
    D3DXMATRIXA16 mat;
    
    // �� ��ǥ��ȯ�� ��� �������� ����
    D3DXMatrixMultiply( &mat, &m_matView, &m_matProj );
    D3DXMatrixTranspose( &mat, &mat );// ��ġ��ķ� ��ȯ
    m_pd3dDevice->SetVertexShaderConstantF(0, (float*)&mat, 4);

    // �� ������ ��� �������� ����
    float color[] = {1.0f, 0.0f, 0.0f, 1.0f};// ����
    m_pd3dDevice->SetVertexShaderConstantF(4, (float*)&color, 1);

    return S_OK;
}




//-------------------------------------------------------------
// Name: Render()
// Desc: ȭ�� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    // ȭ�� Ŭ����
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x000000ff, 1.0f, 0L );

    //---------------------------------------------------------
    // ����
    //---------------------------------------------------------
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        // ������ ���̴� ����
        m_pd3dDevice->SetVertexDeclaration(m_pVertexDeclaration);
        m_pd3dDevice->SetVertexShader( m_pVertexShader );
		
        // ������ ����
        m_pd3dDevice->SetStreamSource( 0, m_pVB, 0,
                                       sizeof(CUSTOMVERTEX) );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

        RenderText();    // ȭ����¿� ���� ���

        // ��������
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}




//-------------------------------------------------------------
// Name: RenderText()
// Desc: ���¿� ������ ȭ�鿡 ���
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    // ȭ�����
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    // ���۹� & �Ķ���� ���
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    return S_OK;
}




//-------------------------------------------------------------
// Name: MsgProc()
// Desc: WndProc �������̵�
//-------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT msg,
                                 WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_PAINT:
        {
            if( m_bLoadingApp )
            {
                // �ε���
                HDC hDC = GetDC( hWnd );
                TCHAR strMsg[MAX_PATH];
                wsprintf(strMsg, TEXT("Loading... Please wait"));
                RECT rct;
                GetClientRect( hWnd, &rct );
                DrawText( hDC, strMsg, -1, &rct
                        , DT_CENTER|DT_VCENTER|DT_SINGLELINE );
                ReleaseDC( hWnd, hDC );
            }
            break;
        }

    }

    return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}




//-------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: RestoreDeviceObjects() ���� ������ ������Ʈ ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pVB );        // ��������

    m_pFont->InvalidateDeviceObjects();    // ��Ʈ

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() ���� ������ ������Ʈ ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    // �� �������̴� ����
	SAFE_RELEASE( m_pVertexShader );
    SAFE_RELEASE( m_pVertexDeclaration );

	// ��Ʈ
    m_pFont->DeleteDeviceObjects();

    return S_OK;
}




//-------------------------------------------------------------
// Name: FinalCleanup()
// Desc: ���������� ȣ���
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    SAFE_DELETE( m_pFont );    // ��Ʈ

    return S_OK;
}




