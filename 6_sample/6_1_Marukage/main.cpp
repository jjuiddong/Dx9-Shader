//-------------------------------------------------------------
// File: main.cpp
//
// Desc: 원형그림자
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
	m_pMeshBG					= new CD3DMesh();				
	m_pMesh						= new CD3DMesh();
	m_pTex						= NULL;

	m_pos						= D3DXVECTOR3(-0.5f,1,0);
	m_shadowH					= 0.25f;

    m_fWorldRotX                = -0.2f * D3DX_PI;
	m_fWorldRotY                = -0.25f*2.0f*D3DX_PI;
	m_zoom						= 5.0f;

	m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "main" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
    
	ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
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
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps
						, DWORD dwBehavior, D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
	return S_OK;
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

	// 배경 메시 읽기
	if( FAILED( hr = m_pMeshBG->Create(m_pd3dDevice, "map.x")))
		return DXTRACE_ERR( "Load Mesh", hr );
		
	// UFO모델
	if( FAILED( hr = m_pMesh->Create( m_pd3dDevice, "ufo.x" )))
		return DXTRACE_ERR( "Load Mesh", hr );

	// 그림자 텍스처 읽기
	D3DXCreateTextureFromFileEx(m_pd3dDevice, "shadow.bmp"
								, 0,0,0,0,D3DFMT_A8R8G8B8
                                , D3DPOOL_MANAGED
								, D3DX_FILTER_LINEAR
								, D3DX_FILTER_LINEAR
                                , 0, NULL, NULL, &m_pTex);

    m_pFont->InitDeviceObjects( m_pd3dDevice );// 폰트
    
	return S_OK;
}

//-------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: 화면크기가 변했을때 호출됨
//       확보한 메모리는 InvalidateDeviceObjects()에서 해제
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	m_pMeshBG->RestoreDeviceObjects(m_pd3dDevice);
	m_pMesh  ->RestoreDeviceObjects(m_pd3dDevice);

	// 단축매크로
	#define RS   m_pd3dDevice->SetRenderState
	#define SAMP m_pd3dDevice->SetSamplerState

    // 렌더링 상태설정
    RS  ( D3DRS_ZENABLE,        TRUE );
	SAMP( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
	SAMP( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );
    SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	// 방향성광원의 조명설정
	D3DLIGHT9 light;
	D3DUtil_InitLight( light
			, D3DLIGHT_DIRECTIONAL, -0.5f, -2.0f, 1.0f );
	m_pd3dDevice->SetLight( 0, &light );
	m_pd3dDevice->LightEnable( 0, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // 폰트
    D3DXMatrixIdentity( &m_mWorld );

    // 롅뎓뛱쀱궻먠믦
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width )
				  / ((FLOAT)m_d3dsdBackBuffer.Height);
    D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect
								, 1.0f, 100.0f );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );

	
    m_pFont->RestoreDeviceObjects();	// 긲긅깛긣

    return S_OK;
}




//-------------------------------------------------------------
// Name: FrameMove()
// Desc: 매 프레임마다 호출됨. 애니메이션 처리등 담당
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	m_pos.x = 1.5f*(FLOAT)cos(1.0f*this->m_fTime) + 1.0f;
	m_pos.z = 1.5f*(FLOAT)sin(1.0f*this->m_fTime) + 0.0f;
	m_pos.y =-0.5f*(FLOAT)sin(0.2f*this->m_fTime) + 1.0f;

	UpdateInput( &m_UserInput ); // 입력데이터 갱신

	//---------------------------------------------------------
	// 입력에 따라 좌표계를 갱신한다
	//---------------------------------------------------------
	// 회전
    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else
    if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else
    if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;

	// 줌
	if(m_UserInput.bZ && !m_UserInput.bX)
		m_zoom += 0.01f;
	else if(m_UserInput.bX && !m_UserInput.bZ)
		m_zoom -= 0.01f;

	//---------------------------------------------------------
	// 행렬 갱신
	//---------------------------------------------------------
	// 카메라 회전
	D3DXMATRIX m, matRotX, matRotY;
    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );
    D3DXMatrixMultiply( &m, &matRotY, &matRotX );

    // 뷰행렬
	D3DXVECTOR3 vEye    = D3DXVECTOR3( 0.0f, 0.0f, -m_zoom );
    D3DXVECTOR3 vLookat = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vEye, &vLookat, &vUp );
    m = m * m_mView;
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &m );


    return S_OK;
}




//-------------------------------------------------------------
// Name: UpdateInput()
// Desc: 입력데이터 갱신
//-------------------------------------------------------------
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    pUserInput->bRotateUp    = ( m_bActive && (GetAsyncKeyState( VK_UP )    & 0x8000) == 0x8000 );
    pUserInput->bRotateDown  = ( m_bActive && (GetAsyncKeyState( VK_DOWN )  & 0x8000) == 0x8000 );
    pUserInput->bRotateLeft  = ( m_bActive && (GetAsyncKeyState( VK_LEFT )  & 0x8000) == 0x8000 );
    pUserInput->bRotateRight = ( m_bActive && (GetAsyncKeyState( VK_RIGHT ) & 0x8000) == 0x8000 );
	pUserInput->bZ = ( m_bActive && (GetAsyncKeyState( 'Z' ) & 0x8000) == 0x8000 );
	pUserInput->bX = ( m_bActive && (GetAsyncKeyState( 'X' ) & 0x8000) == 0x8000 );
}




//-------------------------------------------------------------
// Name: Render()
// Desc: 화면 렌더
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	D3DXMATRIX m, mL;
	D3DXVECTOR4 v;

	#define RS   m_pd3dDevice->SetRenderState
	#define TSS  m_pd3dDevice->SetTextureStageState

	if( SUCCEEDED( m_pd3dDevice->BeginScene()))	// 렌더 시작
    {
		// 화면을 칠한다
		m_pd3dDevice->Clear( 0L, NULL,
						D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						0x4004080, 1.0f, 0L );
		
		// 배경 렌더
		D3DXMatrixIdentity( &m );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m );
		m_pMeshBG->Render( m_pd3dDevice );

		// 비행모델 렌더
		D3DXMatrixTranslation( &m, m_pos.x, m_pos.y, m_pos.z );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m );
		m_pMesh->Render( m_pd3dDevice );
		
		// 그림자 위치 행렬 설정
		D3DXMatrixTranslation(&m, m_pos.x, m_shadowH, m_pos.z);
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m );
		
		// 알파합성 설정
		RS( D3DRS_ALPHABLENDENABLE, TRUE );
		RS( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
		RS( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		RS( D3DRS_LIGHTING, FALSE );
		// 텍스처와 정점색을 모두 반영한다
		TSS( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
		TSS( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		TSS( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
		
		typedef struct {
			FLOAT		p[3];		// 정점좌표
			D3DCOLOR	color;		// 정점색
			FLOAT		tu, tv;		// 텍스처좌표
		} MyVERTEX;
		
		// 그림자의 크기
		FLOAT size = 0.6f*(m_pos.y-m_shadowH) + 0.4f;
							// UFO가 위에 있을수록 그림자 커진다
		// 그림자의 색(농도)
		FLOAT fc = 100.0f+200.0f*(1.0f-size);		
							// 크기에 비례해서 색은 약해진다
		DWORD c = min(255,(DWORD)((0<fc)?fc:0));	
							// 값의 범위를 0 ~ 255로 제한

		MyVERTEX Vertex[4] = {
			// x    y    z          색       (적녹청α) tu tv
			{{-size, 0, -size}, D3DCOLOR_RGBA(c,c,c,0), 0, 0,},
			{{-size, 0,  size}, D3DCOLOR_RGBA(c,c,c,0), 0, 1,},
			{{ size, 0,  size}, D3DCOLOR_RGBA(c,c,c,0), 1, 1,},
			{{ size, 0, -size}, D3DCOLOR_RGBA(c,c,c,0), 1, 0,},
		};
		m_pd3dDevice->SetTexture( 0, m_pTex );
		m_pd3dDevice->SetFVF(
					D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
					, 2, Vertex, sizeof( MyVERTEX ) );
		
		// 설정 복구
		RS(D3DRS_ALPHABLENDENABLE, FALSE);
		RS( D3DRS_LIGHTING, TRUE );

		RenderText();				// 도우말 출력

        m_pd3dDevice->EndScene();	// 렌더 종료
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

    FLOAT fNextLine = 40.0f; 

    // 조작방법
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
	sprintf( szMsg, "Zoom: %f", m_zoom );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Use arrow keys to rotate object") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Press 'z' or 'x' to change zoom") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    // 디스플레이 상태
    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    
	return S_OK;
}




//-------------------------------------------------------------
// Name: MsgProc()
// Desc: WndProc 오버라이딩
//-------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd,
					UINT msg, WPARAM wParam, LPARAM lParam )
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
                DrawText( hDC, strMsg, -1, &rct,
						DT_CENTER|DT_VCENTER|DT_SINGLELINE );
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
	m_pMeshBG->InvalidateDeviceObjects(); // 메시
	m_pMesh->InvalidateDeviceObjects();
	
    m_pFont->InvalidateDeviceObjects();	// 폰트

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() 에서 생성한 오브젝트 해제
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	m_pMeshBG->Destroy();
	m_pMesh->Destroy();

	SAFE_RELEASE( m_pTex );

    m_pFont->DeleteDeviceObjects();	// 폰트

	return S_OK;
}




//-------------------------------------------------------------
// Name: FinalCleanup()
// Desc: 종료직전에 호출됨
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
	SAFE_DELETE( m_pMesh   ); // 메시
	SAFE_DELETE( m_pMeshBG );

    SAFE_DELETE( m_pFont );	// 폰트

    return S_OK;
}
