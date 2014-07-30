//-------------------------------------------------------------
// File: main.cpp
//
// Desc: 텍스처 입히기
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
// 정점포맷
//-------------------------------------------------------------
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ)

typedef struct {
    FLOAT x, y, z;    // 스크린좌표에서 위치
} CUSTOMVERTEX;



//-------------------------------------------------------------
// 전역변수
//-------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;


//-------------------------------------------------------------
// Name: WinMain()
// Desc: 메인함수
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
// Desc: 어플리케이션 생성자
//-------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
    // ★ 셰이더 클리어
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
// Desc: 소멸자
//-------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
}




//-------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: 단 한번만 초기화
//       윈도우 초기화와 IDirect3D9초기화는 끝난뒤
//       그러나 LPDIRECT3DDEVICE9초기화는 끝나지 않은 상태
//-------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    // 로딩 메시지 출력
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    m_bLoadingApp = FALSE;

    return S_OK;
}




//-------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: 초기화시 호출됨. 필요한 능력(caps)체크
//-------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps,
                     DWORD dwBehavior,    D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    
    BOOL bCapsAcceptable;

    // 그래픽보드가 프로그램을 실행할 능력이 있는가 확인
    bCapsAcceptable = TRUE;

    // ★ 정점셰이더의 버전 확인
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
// Desc: 디바이스가 생성된후의 초기화
//       프레임버퍼 포맷과 디바이스 종류가 변한뒤에 호출
//       여기서 확보한 메모리는 DeleteDeviceObjects()에서 해제
//-------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    HRESULT hr;
    LPD3DXBUFFER pCode;
    
    // ---------------------------------------------------------
    // ★ 정점셰이더 생성
    // ---------------------------------------------------------

    // 정점선언 오브젝트 생성
    D3DVERTEXELEMENT9 decl[] = {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,
                 D3DDECLUSAGE_POSITION, 0 },
        D3DDECL_END()
    };
    
    if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( decl, &m_pVertexDeclaration ) ) ) {
        return hr;
    }

    // 셰이더 읽기
    if( FAILED( hr = D3DXAssembleShaderFromFile(
                        _T("VertexShader.vsh"), NULL, NULL, 0, &pCode, NULL ))) {
        return hr;
    }

    // 정점 셰이더 생성
    hr = m_pd3dDevice->CreateVertexShader(
                          (DWORD*)pCode->GetBufferPointer(),
                          &m_pVertexShader );
    pCode->Release();
    if( FAILED(hr) ) return hr;

	// 폰트
    m_pFont->InitDeviceObjects( m_pd3dDevice );

    return S_OK;
}

//-------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: 화면크기가 변했을때 호출됨
//       확보한 메모리는 InvalidateDeviceObjects()에서 해제
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
    // 정점과 색정보
    CUSTOMVERTEX vertices[] = {
        //   x,     y,      z,  
        {-0.5f, +0.5f, 0},
        {+0.5f, +0.5f, 0},
        {-0.5f, -0.5f, 0},
        {+0.5f, -0.5f, 0},
    };
    
    // 정점버퍼 생성
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( 
                4*sizeof(CUSTOMVERTEX),        // 정점버퍼 크기
                0, D3DFVF_CUSTOMVERTEX,        // 사용법, 정점포맷
                D3DPOOL_DEFAULT,            // 메모리 클래스
                &m_pVB, NULL )))            // 정점버퍼 리소스
        return E_FAIL;

    // 정점버퍼에 정보 저장
    VOID* pVertices;
    if(FAILED( m_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
        return E_FAIL;
    memcpy( pVertices, vertices, sizeof(vertices) );
    m_pVB->Unlock();

    // 단축매크로
    #define RS   m_pd3dDevice->SetRenderState
    #define TSS  m_pd3dDevice->SetTextureStageState
    #define SAMP m_pd3dDevice->SetSamplerState

    // 렌더링 상태설정
    RS( D3DRS_DITHERENABLE,   FALSE );
    RS( D3DRS_SPECULARENABLE, FALSE );
    RS( D3DRS_ZENABLE,        TRUE );
    RS( D3DRS_AMBIENT,        0x000F0F0F );
    
    TSS( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    TSS( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

    // 폰트
    m_pFont->RestoreDeviceObjects();

    // 뷰행렬 설정
    D3DXVECTOR3 vEye = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    D3DXVECTOR3 vAt  = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp  = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtRH( &m_matView, &vEye, &vAt, &vUp );

    // 투영행렬 설정
    FLOAT fAspectRatio = (FLOAT)m_d3dsdBackBuffer.Width
                       / (FLOAT)m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovRH( &m_matProj, D3DXToRadian(60.0f),
                                fAspectRatio, 0.1f, 100.0f );

	return S_OK;
}




//-------------------------------------------------------------
// Name: FrameMove()
// Desc: 매 프레임마다 호출됨. 애니메이션 처리등 담당
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
    D3DXMATRIXA16 mat;
    
    // ★ 좌표변환용 상수 레지스터 설정
    D3DXMatrixMultiply( &mat, &m_matView, &m_matProj );
    D3DXMatrixTranspose( &mat, &mat );// 전치행렬로 변환
    m_pd3dDevice->SetVertexShaderConstantF(0, (float*)&mat, 4);

    // ★ 정점색 상수 레지스터 설정
    float color[] = {1.0f, 0.0f, 0.0f, 1.0f};// 적색
    m_pd3dDevice->SetVertexShaderConstantF(4, (float*)&color, 1);

    return S_OK;
}




//-------------------------------------------------------------
// Name: Render()
// Desc: 화면 렌더
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    // 화면 클리어
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x000000ff, 1.0f, 0L );

    //---------------------------------------------------------
    // 렌더
    //---------------------------------------------------------
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
        // ★정점 셰이더 설정
        m_pd3dDevice->SetVertexDeclaration(m_pVertexDeclaration);
        m_pd3dDevice->SetVertexShader( m_pVertexShader );
		
        // 폴리곤 렌더
        m_pd3dDevice->SetStreamSource( 0, m_pVB, 0,
                                       sizeof(CUSTOMVERTEX) );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

        RenderText();    // 화면상태와 도움말 출력

        // 렌더종료
        m_pd3dDevice->EndScene();
    }

    return S_OK;
}




//-------------------------------------------------------------
// Name: RenderText()
// Desc: 상태와 도움말을 화면에 출력
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    // 화면상태
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    // 조작법 & 파라메터 출력
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    return S_OK;
}




//-------------------------------------------------------------
// Name: MsgProc()
// Desc: WndProc 오버라이딩
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
                // 로드중
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
// Desc: RestoreDeviceObjects() 에서 생성한 오브젝트 해제
//-------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pVB );        // 정점버퍼

    m_pFont->InvalidateDeviceObjects();    // 폰트

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() 에서 생성한 오브젝트 해제
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    // ★ 정점셰이더 해제
	SAFE_RELEASE( m_pVertexShader );
    SAFE_RELEASE( m_pVertexDeclaration );

	// 폰트
    m_pFont->DeleteDeviceObjects();

    return S_OK;
}




//-------------------------------------------------------------
// Name: FinalCleanup()
// Desc: 종료직전에 호출됨
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    SAFE_DELETE( m_pFont );    // 폰트

    return S_OK;
}




