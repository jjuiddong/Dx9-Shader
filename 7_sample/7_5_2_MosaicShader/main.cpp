//-------------------------------------------------------------
// File: main.cpp
//
// Desc: 모자이크
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
	m_pTex						= NULL;
	m_Size						= 256;
	m_pos[0]					= (FLOAT)m_Size/2;
	m_pos[1]					= (FLOAT)m_Size/2;
	m_vel[0]					= 100.0f;
	m_vel[1]					= 70.0f;

	m_pMesh						= new CD3DMesh();
	m_pMeshBg					= new CD3DMesh();

	m_pMapZ						= NULL;
	m_pOriginalMap				= NULL;
	m_pOriginalMapSurf			= NULL;

	m_pEffect					= NULL;
	m_hTechnique  				= NULL;
	m_htMask					= NULL;
	m_htSrc						= NULL;

	m_fWorldRotX                = -0.41271535f;
    m_fWorldRotY                = 0.0f;
	m_fViewZoom				    = 5.0f;

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
    

	// 픽셀셰이더 버전체크
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) )
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
	D3DXVECTOR4 offset;

	// 텍스처 읽기
	D3DXCreateTextureFromFile(m_pd3dDevice, "mask.tga"
							, &m_pTex);
	// 모델읽기
	if(FAILED(hr=m_pMesh  ->Create( m_pd3dDevice, _T("ufo.x"))))
        return DXTRACE_ERR( "LoadCar", hr );
	// 지형읽기
	if(FAILED(hr=m_pMeshBg->Create( m_pd3dDevice, _T("map.x"))))
        return DXTRACE_ERR( "Load BG", hr );
        
	// 셰이더읽기
	LPD3DXBUFFER pErr;
    if( FAILED( hr = D3DXCreateEffectFromFile(
				m_pd3dDevice, "hlsl.fx", NULL, NULL, 
				D3DXSHADER_DEBUG , NULL, &m_pEffect, &pErr ))){
		MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer()
					, "ERROR", MB_OK);
		return DXTRACE_ERR( "CreateEffectFromFile", hr );
	}
	m_hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
	m_htMask     = m_pEffect->GetParameterByName( NULL, "tMask" );
	m_htSrc      = m_pEffect->GetParameterByName( NULL, "tSrc" );
	
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

    // 재질설정
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // 렌더링 상태설정
    RS( D3DRS_DITHERENABLE,   FALSE );
    RS( D3DRS_SPECULARENABLE, FALSE );
    RS( D3DRS_ZENABLE,        TRUE );
    RS( D3DRS_AMBIENT,        0x000F0F0F );
    
    TSS( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    TSS( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    TSS( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    TSS( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    TSS( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
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

	// 렌더링타겟 생성
	if (FAILED(m_pd3dDevice->CreateDepthStencilSurface(
				MAP_WIDTH, MAP_HEIGHT, D3DFMT_D16, 
				D3DMULTISAMPLE_NONE, 0, TRUE, &m_pMapZ, NULL)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateTexture(
				MAP_WIDTH, MAP_HEIGHT, 1, D3DUSAGE_RENDERTARGET,
				D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pOriginalMap, NULL)))
		return E_FAIL;
	if (FAILED(m_pOriginalMap->GetSurfaceLevel(0, &m_pOriginalMapSurf)))
		return E_FAIL;

	m_pEffect->OnResetDevice();

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

	//---------------------------------------------------------
	// 모자이크 이동
	//---------------------------------------------------------
	D3DVIEWPORT9 viewport;
	m_pd3dDevice->GetViewport(&viewport);

	m_pos[0] += m_fElapsedTime * m_vel[0];
	m_pos[1] += m_fElapsedTime * m_vel[1];
	if(m_pos[0]<m_Size/2){m_pos[0] = m_Size - m_pos[0]; m_vel[0]=-m_vel[0];}
	if(m_pos[1]<m_Size/2){m_pos[1] = m_Size - m_pos[1]; m_vel[1]=-m_vel[1];}
	if(viewport.Width -m_Size/2<m_pos[0]){m_pos[0] = 2*viewport.Width -m_Size - m_pos[0]; m_vel[0]=-m_vel[0];}
	if(viewport.Height-m_Size/2<m_pos[1]){m_pos[1] = 2*viewport.Height-m_Size - m_pos[1]; m_vel[1]=-m_vel[1];}

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
}


//-------------------------------------------------------------
// Name: Render()
// Desc: 화면 렌더
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    D3DXMATRIX m, mT, mR, mView, mProj;
	LPDIRECT3DSURFACE9 pOldBackBuffer, pOldZBuffer;
	D3DVIEWPORT9 oldViewport;

	//---------------------------------------------------------
	// 렌더
	//---------------------------------------------------------
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		//-------------------------------------------------
		// 렌더링타겟 보존
		//-------------------------------------------------
		m_pd3dDevice->GetRenderTarget(0, &pOldBackBuffer);
		m_pd3dDevice->GetDepthStencilSurface(&pOldZBuffer);
		m_pd3dDevice->GetViewport(&oldViewport);

		if( m_pEffect != NULL ) 
		{
			//-------------------------------------------------
			// 렌더링타겟 변경
			//-------------------------------------------------
			m_pd3dDevice->SetRenderTarget(0, m_pOriginalMapSurf);
			m_pd3dDevice->SetDepthStencilSurface(m_pMapZ);
			// 뷰포트변경
			D3DVIEWPORT9 viewport = {0,0      // 좌측상단
							, MAP_WIDTH  // 폭
							, MAP_HEIGHT // 높이
							, 0.0f,1.0f};     // 전면,후면
			m_pd3dDevice->SetViewport(&viewport);

			// 렌더링타겟 클리어
			m_pd3dDevice->Clear(0L, NULL
							, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
							, 0xffffffff, 1.0f, 0L);

			//-------------------------------------------------
			// 1패스:렌더링타겟 만들기
			//-------------------------------------------------
			m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m_mWorld);
			m_pd3dDevice->SetTransform( D3DTS_VIEW,  &m_mView );
			m_pd3dDevice->SetTransform( D3DTS_PROJECTION,  &m_mProj );

			// 지면 렌더
			TSS( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			TSS( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
			SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
			m_pMeshBg->Render( m_pd3dDevice );
			
			// 비행모델 렌더
			TSS( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			TSS( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
			RS( D3DRS_LIGHTING, TRUE );
			RS( D3DRS_AMBIENT, 0 );
			m_pd3dDevice->LightEnable( 0, TRUE );
			D3DXMatrixTranslation( &m, 1.0f, 0.0f ,0.0f );
			D3DXMatrixRotationY( &mR,  m_fTime );
			D3DXMatrixTranslation( &mT, 1.0f, 1.0f ,0.0f );
			m = m * mR * mT * m_mWorld;
			m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m);
			m_pMesh  ->Render( m_pd3dDevice );
		}

		//-----------------------------------------------------
		// 렌더링타겟 복구
		//-----------------------------------------------------
		m_pd3dDevice->SetRenderTarget(0, pOldBackBuffer);
		m_pd3dDevice->SetDepthStencilSurface(pOldZBuffer);
		m_pd3dDevice->SetViewport(&oldViewport);
		pOldBackBuffer->Release();
		pOldZBuffer->Release();

		//-----------------------------------------------------
		// 렌더링한 화면을 그대로 붙인다
		//-----------------------------------------------------
		RS( D3DRS_ZENABLE, FALSE );
		RS( D3DRS_LIGHTING, FALSE );
		TSS(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		TSS(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		TSS(1,D3DTSS_COLOROP,    D3DTOP_DISABLE);
		SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
		SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );

		FLOAT w = (FLOAT)oldViewport.Width;
		FLOAT h = (FLOAT)oldViewport.Height;

		TVERTEX Vertex1[4] = {
			//x  y   z    rhw  tu tv
			{ 0, 0, 0.1f, 1.0f, 0, 0,},
			{ w, 0, 0.1f, 1.0f, 1, 0,},
			{ w, h, 0.1f, 1.0f, 1, 1,},
			{ 0, h, 0.1f, 1.0f, 0, 1,},
		};
		m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		m_pd3dDevice->SetTexture(0, m_pOriginalMap);
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
							, 2, Vertex1, sizeof( TVERTEX ) );

		//-------------------------------------------------
		// 셰이더 설정
		//-------------------------------------------------
		m_pEffect->SetTechnique( m_hTechnique );
		m_pEffect->Begin( NULL, 0 );
		m_pEffect->Pass( 0 );

		//-----------------------------------------------------
		// 모자이크를 붙인다
		//-----------------------------------------------------
		FLOAT x0 = m_pos[0]-(FLOAT)m_Size/2;
		FLOAT x1 = m_pos[0]+(FLOAT)m_Size/2;
		FLOAT y0 = m_pos[1]-(FLOAT)m_Size/2;
		FLOAT y1 = m_pos[1]+(FLOAT)m_Size/2;
		w = (FLOAT)oldViewport.Width;
		h = (FLOAT)oldViewport.Height;

		typedef struct {
			FLOAT       p[4];
			FLOAT       t[2][2];
		} T2VERTEX;
		T2VERTEX Vertex2[4] = {
			//x   y    z    rhw    tu    tv    
			{ x0, y0, 0.1f, 1.0f, 0.0f, 0.0f, (x0+0.5f)/w, (y0+0.5f)/h,},
			{ x1, y0, 0.1f, 1.0f, 1.0f, 0.0f, (x1+0.5f)/w, (y0+0.5f)/h,},
			{ x1, y1, 0.1f, 1.0f, 1.0f, 1.0f, (x1+0.5f)/w, (y1+0.5f)/h,},
			{ x0, y1, 0.1f, 1.0f, 0.0f, 1.0f, (x0+0.5f)/w, (y1+0.5f)/h,},
		};

		RS(D3DRS_ALPHABLENDENABLE, TRUE);
		RS(D3DRS_SRCBLEND , D3DBLEND_SRCALPHA);
		RS(D3DRS_DESTBLEND , D3DBLEND_INVSRCALPHA);

		m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX2 );
		m_pEffect->SetTexture(m_htMask, m_pTex);
		m_pEffect->SetTexture(m_htSrc, m_pOriginalMap);
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
						, 2, Vertex2, sizeof( T2VERTEX ) );

		m_pEffect->End();

		RS( D3DRS_ZENABLE, TRUE );
		RS( D3DRS_LIGHTING, TRUE );
		RS(D3DRS_ALPHABLENDENABLE, FALSE);

		RenderText();				// 도움말 출력

#if 1 // 디버그용 텍스처 출력
		{
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,    D3DTOP_DISABLE);
		m_pd3dDevice->SetVertexShader(NULL);
		m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		m_pd3dDevice->SetPixelShader(0);
		float scale = 128.0f;
		for(DWORD i=0; i<2; i++){
			TVERTEX Vertex[4] = {
				// x  y  z rhw tu tv
				{    0,(i+0)*scale,0, 1, 0, 0,},
				{scale,(i+0)*scale,0, 1, 1, 0,},
				{scale,(i+1)*scale,0, 1, 1, 1,},
				{    0,(i+1)*scale,0, 1, 0, 1,},
			};
			if(0==i) m_pd3dDevice->SetTexture( 0, m_pOriginalMap );
			if(1==i) m_pd3dDevice->SetTexture( 0, m_pTex );
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof( TVERTEX ) );
		}
		}
#endif		

        m_pd3dDevice->EndScene();	// 렌더종료
    }

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
	// 렌더링타겟
	SAFE_RELEASE(m_pOriginalMapSurf);
	SAFE_RELEASE(m_pOriginalMap);
	SAFE_RELEASE(m_pMapZ);

	m_pMesh  ->InvalidateDeviceObjects(); // 메시
	m_pMeshBg->InvalidateDeviceObjects();

    m_pFont->InvalidateDeviceObjects();	// 폰트

	// 셰이더
    if( m_pEffect != NULL ) m_pEffect->OnLostDevice();

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() 에서 생성한 오브젝트 해제
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	SAFE_RELEASE(m_pTex);

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




