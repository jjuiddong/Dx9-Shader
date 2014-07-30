//-----------------------------------------------------------------------------
// File: DirectX9Application1.cpp
//
// Desc: DirectX AppWizard에의해 생성된 윈도우 어플리케이션
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
// 전역변수
//-----------------------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;




//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: 메인함수. 모든 초기화처리후 메시지루프에 들어간다.
//       빈둥거리는 동안 화면 렌더링처리
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
// Desc: 어플리케이션 생성자. ~CMyD3DApplication()에 대응한다.
//       멤버변수는 여기서 초기화. 어플리케이션 윈도우는 아직
//       생성되어있지 않다. DirectX디바이스도 아직 없음. 
//       윈도우와 Direct3D에 의존하는 부분의 초기화는 이곳에서 하면 안됨.
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

    // d3dfont.cpp 를 사용한 폰트
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
    m_pD3DXMesh                 = NULL;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = 0.0f;
    m_fWorldRotY                = 0.0f;
}




//-----------------------------------------------------------------------------
// Name: ~CMyD3DApplication()
// Desc: 소멸자.   CMyD3DApplication()에 대응
//-----------------------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
}




//-----------------------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: FinalCleanup()에 대응
//       윈도우도 생성되어있으며, IDirect3D9도 생성되어있지만
//       디바이스는 아직 생성되어 있지 않다.
//       여기서는 디바이스에 의존하지 않는 어플리케이션
//       초기화를 처리한다
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    // TODO: 한번만 초기화됨

    // 로딩 메시지 출력
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    m_bLoadingApp = FALSE;

    return S_OK;
}









//-----------------------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: 디바이스 초기화중에 불림. 어플리케이션이 정상적으로 작동하기 위한
//       최저한의 기능을 조사하는데 사용됨
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
                                          D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
    BOOL bCapsAcceptable;

    // TODO: 호출된 디바이스의 능력(CAPS)이 동작하는가 조사한다
    bCapsAcceptable = TRUE;

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: DeleteDeviceObjects()에 대응
//       디바이스 생성되어 있음
//       D3DPOOL_MANAGED, D3DPOOL_SCRATCH, D3DPOOL_SYSTEMMEM 플래그가
//       지정된 오브젝트처럼 Reset()호출에 의해서도 소실되지 않는 오브젝트는
//       이곳에서 생성한다. CreateImageSurface()를 사용해서 생성한 
//       표면은 절대로 소실되지 않으므로, 여기서 생성하는 것이 가능하다.
//       정점셰이더와 픽셀셰이더도 소실되지 않으므로 생성가능하다.
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    // TODO: 디바이스에 의존하는 오브젝트 생성

    HRESULT hr;

    // 폰트 초기화
    m_pFont->InitDeviceObjects( m_pd3dDevice );

    // D3DX를 사용해서 주전자 폴리곤모델 생성
    if( FAILED( hr = D3DXCreateTeapot( m_pd3dDevice, &m_pD3DXMesh, NULL ) ) )
        return DXTRACE_ERR( "D3DXCreateTeapot", hr );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: InvalidateDeviceObjects()에 대응
//       디바이스는 존재하지만 소실상태임. D3DPOOL_DEFAULT 리소스나
//       렌더링중에 존재하는 행렬,텍스처 등의 오브젝트는 여기서 생성.
//       렌더링중 변하지 않는 것들은 여기서 설정해두면 좋음
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
    // TODO: 렌더링 환경 설정

    // 질감 설정
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // 텍스처 설정
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    m_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    // 렌더링 환경 & 잡다한 설정
    m_pd3dDevice->SetRenderState( D3DRS_DITHERENABLE,   FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, FALSE );
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_AMBIENT,        0x000F0F0F );

    // 월드행렬 설정
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

    // 뷰행열 설정
    D3DXMATRIX matView;
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

    // 투영행렬 설정
    D3DXMATRIX matProj;
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    // 광원 설정
    D3DLIGHT9 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -1.0f, -1.0f, 2.0f );
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // 폰트 재구축
    m_pFont->RestoreDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FrameMove()
// Desc: 매 프레임마다 한번씩 호출된다.
//       애니메이션 계산을 처리하는 곳
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
    // TODO: 세계를 변경한다

    // 사용자 입력 갱신
    UpdateInput( &m_UserInput );

    // 사용자 입력에 따른 월드정보 갱신
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
// Desc: 사용자 입력 갱신. 매 프레임마다 한번씩 호출
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
// Desc: 매 프레임마다 한번씩 호출. 렌더링이 시작되는 곳
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    // 뷰포트 초기화
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x000000ff, 1.0f, 0L );

    // 렌더링 시작
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        // TODO: 세계 렌더링
        
        // 주전자 출력
        m_pD3DXMesh->DrawSubset(0);

        // 상태 & 도움말 출력
        RenderText();

        // 렌더링 종료
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderText()
// Desc: 상태 & 도움말 출력
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    // 디스플레이 상태 출력
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    // 도움말 출력
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
// Desc: WndProc 오버라이딩 한것. 독자적인 이벤트 처리
//       (즉, 마우스,키보드,메뉴처리 등)
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
                // 로드중임을 사용자에게 전한다
                // TODO: 필요시에 변경
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
// Desc: RestoreDeviceObjects()에 대응. 디바이스의존 오브젝트를 무효화
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    // TODO: RestoreDeviceObjects()에서 생성한 모든 오브젝트 해제
    m_pFont->InvalidateDeviceObjects();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects()에 대응
//       어플리케이션이 종료되거나 디바이스가 변경될경우 호출
//       디바이스에 의존하는 모든 오브젝트 해제
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    // TODO: InitDeviceObjects()에서 생성한 모든 오브젝트 해제
    m_pFont->DeleteDeviceObjects();
    SAFE_RELEASE( m_pD3DXMesh );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: FinalCleanup()
// Desc: OneTimeSceneInit()에 대응
//       어플리케이션 종료직전에 호출
//       종료전에 직접 해제할 것들에 대한 처리
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    // 폰트 해제
    SAFE_DELETE( m_pFont );

    return S_OK;
}




