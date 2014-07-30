//-------------------------------------------------------------
// File: main.cpp
//
// Desc: 모션블러
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

#define MAP_WIDTH	512
#define MAP_HEIGHT	512


// 단축매크로
#define RS   m_pd3dDevice->SetRenderState
#define TSS  m_pd3dDevice->SetTextureStageState
#define SAMP m_pd3dDevice->SetSamplerState


//-------------------------------------------------------------
// 정점구조체
//-------------------------------------------------------------
typedef struct {
    FLOAT       p[4];
    FLOAT       tu, tv;
} TVERTEX;

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
	m_pMesh						= new CD3DMesh();
	m_pMeshBg					= new CD3DMesh();

	m_pEffect					= NULL;
	m_hTechnique  				= NULL;
	m_hmWV       				= NULL;
	m_hmVP       				= NULL;
	m_hmLastWV					= NULL;
	m_hvLightDir  				= NULL;
	m_hvEyePos    				= NULL;
	m_hvCol       				= NULL;

	m_fWorldRotX                = -0.54f;
    m_fWorldRotY                = 1.41f;
	m_fViewZoom				    = 4.0f;

	m_dwCreationWidth           = 512;
    m_dwCreationHeight          = 512;
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
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps,
                     DWORD dwBehavior,    D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    

    // 정점셰이더 버전이 맞지않으면 소프트웨어 처리
    if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1)
    &&  0==(dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) )
			return E_FAIL;

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

	// 모델읽기
	if(FAILED(hr=m_pMesh  ->Create( m_pd3dDevice, _T("ufo.x"))))
        return DXTRACE_ERR( "LoadCar", hr );
	// 지형읽기
	if(FAILED(hr=m_pMeshBg->Create( m_pd3dDevice, _T("map.x"))))
        return DXTRACE_ERR( "Load BG", hr );
        
	// 셰이더 읽기
	LPD3DXBUFFER pErr;
    if( FAILED( hr = D3DXCreateEffectFromFile(
				m_pd3dDevice, "hlsl.fx", NULL, NULL, 
				D3DXSHADER_DEBUG , NULL, &m_pEffect, &pErr ))){
		MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer()
					, "ERROR", MB_OK);
		return DXTRACE_ERR( "CreateEffectFromFile", hr );
	}
	m_hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
	m_hmWV       = m_pEffect->GetParameterByName( NULL, "mWV" );
	m_hmVP       = m_pEffect->GetParameterByName( NULL, "mVP" );
	m_hmLastWV   = m_pEffect->GetParameterByName( NULL, "mLastWV" );
	m_hvLightDir = m_pEffect->GetParameterByName( NULL, "vLightDir" );
	m_hvEyePos   = m_pEffect->GetParameterByName( NULL, "vEyePos" );
	m_hvCol      = m_pEffect->GetParameterByName( NULL, "vCol" );

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
	// 메시
	m_pMesh  ->RestoreDeviceObjects( m_pd3dDevice );
	m_pMeshBg->RestoreDeviceObjects( m_pd3dDevice );

	// 셰이더
	m_pEffect->OnResetDevice();

    // 재질설정
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );


    // 렌더링 상태설정
    RS( D3DRS_DITHERENABLE,   FALSE );
    RS( D3DRS_SPECULARENABLE, FALSE );
    RS( D3DRS_ZENABLE,        TRUE );
    RS( D3DRS_AMBIENT,        0x000F0F0F );
	RS( D3DRS_LIGHTING, TRUE );
    
    TSS( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    TSS( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    TSS( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    TSS( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
	TSS( 0, D3DTSS_ALPHAARG1,	D3DTA_TEXTURE);
    SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    SAMP( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

    // 월드행렬
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &m_mWorld );

	// 뷰행렬
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFromPt, &vLookatPt, &vUpVec );

    // 투영행렬
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );

    // 폰트
    m_pFont->RestoreDeviceObjects();

	return S_OK;
}




//-------------------------------------------------------------
// Name: FrameMove()
// Desc: 매 프레임마다 호출됨. 애니메이션 처리등 담당
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	UpdateInput( &m_UserInput ); // 입력데이터 갱신

	//---------------------------------------------------------
	// 입력에 따라 좌표계를 갱신한다
	//---------------------------------------------------------
	// 회전
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;

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

    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );

    D3DXMatrixMultiply( &m_mWorld, &matRotY, &matRotX );
	
	//---------------------------------------------------------
	// 뷰행렬 설정
	//---------------------------------------------------------
	// 줌
    if( m_UserInput.bZoomIn && !m_UserInput.bZoomOut )
        m_fViewZoom += m_fElapsedTime;
    else if( m_UserInput.bZoomOut && !m_UserInput.bZoomIn )
        m_fViewZoom -= m_fElapsedTime;

    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -m_fViewZoom );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFromPt, &vLookatPt, &vUpVec );

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
    
	pUserInput->bZoomIn      = ( m_bActive && (GetAsyncKeyState( 'Z'     )  & 0x8000) == 0x8000 );
    pUserInput->bZoomOut     = ( m_bActive && (GetAsyncKeyState( 'X'      ) & 0x8000) == 0x8000 );

    pUserInput->bDispersionUp   = ( m_bActive && (GetAsyncKeyState( VK_PRIOR) & 0x8000) == 0x8000 );
    pUserInput->bDispersionDown = ( m_bActive && (GetAsyncKeyState( VK_NEXT ) & 0x8000) == 0x8000 );
}


//-------------------------------------------------------------
// Name: Render()
// Desc: 화면 렌더
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    D3DXMATRIX m, mT, mR, mL, mView, mProj;
	D3DXVECTOR4 v, light_pos;
	D3DMATERIAL9 *pMtrl;
	DWORD i;

	//---------------------------------------------------------
	// 렌더
	//---------------------------------------------------------
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		m_pd3dDevice->Clear(0L, NULL
						, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
						, D3DCOLOR_RGBA(0,0,0,0), 1.0f, 0L);

		//-------------------------------------------------
		// 그냥 평범하게 렌더
		//-------------------------------------------------
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m_mWorld);
		m_pd3dDevice->SetTransform( D3DTS_VIEW,  &m_mView );
		m_pd3dDevice->SetTransform( D3DTS_PROJECTION,  &m_mProj );

		TSS( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		TSS( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pMeshBg->Render( m_pd3dDevice );
		
		// 비행모델 렌더
		TSS( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		TSS( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		TSS( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		TSS( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );

		//-------------------------------------------------
		// 확대한 모델 출력
		//-------------------------------------------------
		if( m_pEffect != NULL )
		{
			m_pEffect->SetTechnique( m_hTechnique );
			m_pEffect->Begin( NULL, 0 );
			
			// 로컬-월드 행렬
			D3DXMatrixTranslation( &m, 1.0f, 0.0f ,0.0f );
			D3DXMatrixRotationY( &mR,  3.0f*m_fTime );
			D3DXMatrixTranslation( &mT, 1.0f, 1.0f ,0.0f );
			mL = m * mR * mT * m_mWorld;

			// 광원방향
			D3DXMatrixInverse( &m, NULL, &mL);
			light_pos = D3DXVECTOR4(1,1,-1,0);
			D3DXVec4Transform( &v, &light_pos, &m );
			v.w = 0;
			D3DXVec4Normalize( &v, &v );
			m_pEffect->SetVector( m_hvLightDir, &v );
			// 시점
			D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -m_fViewZoom );
			D3DXVec3Transform( &v, &vFromPt, &m );
			m_pEffect->SetVector( m_hvEyePos, &v );
			// 로컬-투영변환행렬
			m = mL * m_mView;
			m_pEffect->SetMatrix( m_hmWV, &m );
			m_pEffect->SetMatrix( m_hmVP, &m_mProj );
			// 1프레임 직전 행렬
			m_pEffect->SetMatrix( m_hmLastWV, &m_mLastWV );
			m_mLastWV = m;

			for(int pass = 0; pass<2; pass++){
				m_pEffect->Pass( pass );// 패스를 바꿔서 출력
				if(1==pass){
					RS( D3DRS_ZWRITEENABLE, FALSE);
					RS( D3DRS_ALPHABLENDENABLE, TRUE);
					RS( D3DRS_SRCBLEND,   D3DBLEND_SRCALPHA);
					RS( D3DRS_DESTBLEND,  D3DBLEND_INVSRCALPHA);
				}

				pMtrl = m_pMesh->m_pMaterials;
				for( i=0; i<m_pMesh->m_dwNumMaterials; i++ ) {
					v.x = pMtrl->Diffuse.r;
					v.y = pMtrl->Diffuse.g;
					v.z = pMtrl->Diffuse.b;
					v.w = pMtrl->Diffuse.a;
					m_pEffect->SetVector( m_hvCol, &v );
					m_pMesh->m_pLocalMesh->DrawSubset( i );  // 렌더
					pMtrl++;
				}
			}

			m_pEffect->End();

			RS( D3DRS_ALPHABLENDENABLE, FALSE);
			RS( D3DRS_ZWRITEENABLE, TRUE);
		}
		RenderText();				// 도움말 출력

        m_pd3dDevice->EndScene();	// 렌더종료
    }
	Sleep(100);

    return S_OK;
}




//-------------------------------------------------------------
// Name: RenderText()
// Desc: 상태와 도움말 출력
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    FLOAT fNextLine = 40.0f; // 출력높이

    // 조작방법
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
	
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
	m_pMesh  ->InvalidateDeviceObjects(); // 메시
	m_pMeshBg->InvalidateDeviceObjects();

    m_pFont->InvalidateDeviceObjects();	// 폰트

	// 이펙트
    if( m_pEffect != NULL ) m_pEffect->OnLostDevice();

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() 에서 생성한 오브젝트 해제
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    // 셰이더
	SAFE_RELEASE( m_pEffect );
	
	// 메시
	m_pMesh  ->Destroy();
	m_pMeshBg->Destroy();

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
    SAFE_DELETE( m_pMeshBg ); // 메시
	SAFE_DELETE( m_pMesh );

    SAFE_DELETE( m_pFont );	// 폰트

    return S_OK;
}




