//-------------------------------------------------------------
// File: main.cpp
//
// Desc: 투영텍스처
//-------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr.h>
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

static const UINT MAP_SIZE = 256;

// 단축매크로
#define RS   m_pd3dDevice->SetRenderState
#define TSS  m_pd3dDevice->SetTextureStageState
#define SAMP m_pd3dDevice->SetSamplerState

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
	m_pShadowTex	 = NULL;
	m_pShadowSurf	 = NULL;
	m_pShadowTexZ	 = NULL;

	m_pEffect		 = NULL;
	m_hTechnique	 = NULL;
	m_hmWVP			 = NULL;
	m_hmWVPT		 = NULL;
	m_hvLightPos	 = NULL;
	m_hDecaleMap	 = NULL;
	m_hShadowMap	 = NULL;
	m_pDecl			 = NULL;


	m_pMesh						= new CD3DMesh();
	m_pMeshBg					= new CD3DMesh();

	m_pos						= D3DXVECTOR3( 1.5,1,2);
	m_light						= D3DXVECTOR4(1,3, 2,1);

    m_fWorldRotX                = -0.2f * D3DX_PI;
	m_fWorldRotY                = -0.0f*2.0f*D3DX_PI;
	m_zoom						= 7.0f;

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
    
	// 셰이더 체크
	if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) &&
	  !(dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) )
		return E_FAIL;	// 정점셰이더
	
	if( pCaps->PixelShaderVersion < D3DPS_VERSION(2,0))
		return E_FAIL;	// 픽셀셰이더

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

	// UFO모델
	if( FAILED( hr = m_pMesh->Create( m_pd3dDevice, "ufo.x" )))
		return DXTRACE_ERR( "Load Mesh", hr );
	// 지형모델
	if( FAILED( hr = m_pMeshBg->Create( m_pd3dDevice, "map.x" )))
		return DXTRACE_ERR( "Load Mesh", hr );

	// 셰이더 읽기
	LPD3DXBUFFER pErr;
    if( FAILED( hr = D3DXCreateEffectFromFile(
					m_pd3dDevice, "hlsl.fx", NULL, NULL, 
				D3DXSHADER_DEBUG , NULL, &m_pEffect, &pErr ) ) ){
		MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK);
		return DXTRACE_ERR( "CreateEffectFromFile", hr );
	}
	m_hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
	m_hmWVP  = m_pEffect->GetParameterByName( NULL, "mWVP" );
	m_hmWVPT = m_pEffect->GetParameterByName( NULL, "mWVPT" );
	m_hvLightPos  = m_pEffect->GetParameterByName( NULL, "vLightPos" );
	m_hDecaleMap  = m_pEffect->GetParameterByName( NULL, "DecaleMap" );
	m_hShadowMap  = m_pEffect->GetParameterByName( NULL, "ShadowMap" );

	// 정점선언 오브젝트 생성(지면용)
	D3DVERTEXELEMENT9 decl[] =
	{
		{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
		{0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( decl, &m_pDecl )))
		return DXTRACE_ERR ("CreateVertexDeclaration", hr);

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
	// 그림자 텍스처 생성
	if (FAILED(m_pd3dDevice->CreateTexture(MAP_SIZE, MAP_SIZE, 1, 
						D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
						D3DPOOL_DEFAULT, &m_pShadowTex, NULL)))
		return E_FAIL;
	if (FAILED(m_pShadowTex->GetSurfaceLevel(0, &m_pShadowSurf)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateDepthStencilSurface(
						MAP_SIZE, MAP_SIZE, D3DFMT_D24S8, 
						D3DMULTISAMPLE_NONE, 0, TRUE,
						&m_pShadowTexZ, NULL)))
		return E_FAIL;

	// 메시
	m_pMesh  ->RestoreDeviceObjects(m_pd3dDevice);
	m_pMeshBg->RestoreDeviceObjects(m_pd3dDevice);

    // 렌더링 상태설정
    RS  ( D3DRS_ZENABLE,        TRUE );
	SAMP( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
	SAMP( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );
    SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	// 광원설정
	D3DLIGHT9 light;
	D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL
					, -m_light.x, -m_light.y, -m_light.z );
	m_pd3dDevice->SetLight( 0, &light );
	m_pd3dDevice->LightEnable( 0, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // 월드 행렬
    D3DXMatrixIdentity( &m_mWorld );

    // 투영행렬 설정
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width )
				  / ((FLOAT)m_d3dsdBackBuffer.Height);
    D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect
								, 1.0f, 100.0f );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );

	
    m_pFont->RestoreDeviceObjects();	// 폰트

    return S_OK;
}




//-------------------------------------------------------------
// Name: FrameMove()
// Desc: 매 프레임마다 호출됨. 애니메이션 처리등 담당
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	// UFO를 움직인다
	m_pos.x = 1.5f*(FLOAT)cos(1.0f*this->m_fTime)+1.0f;
	m_pos.z = 1.5f*(FLOAT)sin(1.0f*this->m_fTime);
	m_pos.y = 1.3f;

	UpdateInput( &m_UserInput ); // 입력데이터 갱신

	//---------------------------------------------------------
	// 입력에 따라 좌표계를 갱신한다
	//---------------------------------------------------------
	// 회전
    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft)
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;

	// 줌
	if(m_UserInput.bZ && !m_UserInput.bX)
		m_zoom += 0.01f;
	else if(m_UserInput.bX && !m_UserInput.bZ)
		m_zoom -= 0.01f;

	//---------------------------------------------------------
	// 행렬 갱신
	//---------------------------------------------------------
	// 월드 회전
	D3DXMATRIX m, matRotX, matRotY;
    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );
    D3DXMatrixMultiply( &m, &matRotY, &matRotX );

    // 뷰 행렬
	D3DXVECTOR3 vEye    = D3DXVECTOR3( 0.0f, 0.0f, -m_zoom );
    D3DXVECTOR3 vLookat = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vEye, &vLookat, &vUp );
    m_mView = m * m_mView;
	m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_mView );

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
	LPDIRECT3DSURFACE9 pOldBackBuffer, pOldZBuffer;
	D3DVIEWPORT9 oldViewport;
	D3DXMATRIX m, mL, mV, mP, mT;
	D3DXVECTOR4 v;

	mT = D3DXMATRIX(0.5f, 0.0f, 0.0f, 0.0f
				  , 0.0f,-0.5f, 0.0f, 0.0f
				  , 0.0f, 0.0f, 1.0f, 0.0f
				  , 0.5f, 0.5f, 0.0f, 1.0f);

	if( SUCCEEDED( m_pd3dDevice->BeginScene()))	// 렌더 시작
    {
		//-----------------------------------------------------
		// 렌더링타겟 보존
		//-----------------------------------------------------
		m_pd3dDevice->GetRenderTarget(0, &pOldBackBuffer);
		m_pd3dDevice->GetDepthStencilSurface(&pOldZBuffer);
		m_pd3dDevice->GetViewport(&oldViewport);

		//-----------------------------------------------------
		// 렌더링타겟 변경
		//-----------------------------------------------------
		m_pd3dDevice->SetRenderTarget(0, m_pShadowSurf);
		m_pd3dDevice->SetDepthStencilSurface(m_pShadowTexZ);
		// 뷰포트변경            x y  width    height   minz maxz
		D3DVIEWPORT9 viewport = {0,0, MAP_SIZE,MAP_SIZE,0.0f,1.0f};
		m_pd3dDevice->SetViewport(&viewport);

		// 그림자맵 클리어
		m_pd3dDevice->Clear(0L, NULL
							, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
							, 0x00000000, 1.0f, 0L);

		//-----------------------------------------------------
		// 1패스:그림자맵 생성
		//-----------------------------------------------------
		// 비행모델 렌더
		m_pMesh->UseMeshMaterials(FALSE);
		D3DMATERIAL9 mtrl;
		ZeroMemory( &mtrl, sizeof(mtrl) );
		mtrl.Ambient.r = 1.0f;
		mtrl.Ambient.g = 1.0f;
		mtrl.Ambient.b = 1.0f;
		m_pd3dDevice->SetMaterial( &mtrl );
		RS( D3DRS_LIGHTING, TRUE );
		RS( D3DRS_AMBIENT, 0xffffff );
		TSS( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		TSS( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

		// 월드행렬
		D3DXMatrixTranslation( &mL, m_pos.x, m_pos.y, m_pos.z );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &mL );
		// 뷰 행렬
		D3DXVECTOR3 vEye    = D3DXVECTOR3( m_light.x, m_light.y, m_light.z );
		D3DXVECTOR3 vLookat = D3DXVECTOR3( m_pos.x, m_pos.y, m_pos.z );
		D3DXVECTOR3 vUp     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
		D3DXMatrixLookAtLH( &mV, &vEye, &vLookat, &vUp );
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &mV );
		// 투영행렬
		D3DXMatrixPerspectiveFovLH( &mP, D3DX_PI/2.5, 1, 0.1f, 10.0f );
		m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &mP );

		m_pMesh->Render( m_pd3dDevice );
		
		m_pMesh->UseMeshMaterials(TRUE);

		//-----------------------------------------------------
		// 렌더링타겟 복구
		//-----------------------------------------------------
		m_pd3dDevice->SetRenderTarget(0, pOldBackBuffer);
		m_pd3dDevice->SetDepthStencilSurface(pOldZBuffer);
		m_pd3dDevice->SetViewport(&oldViewport);
		pOldBackBuffer->Release();
		pOldZBuffer->Release();
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_mView );
		m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );

		//-----------------------------------------------------
		// 2패스:장면렌더
		//-----------------------------------------------------
		// 화면 클리어
		m_pd3dDevice->Clear( 0L, NULL,
							D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
							0x4004080, 1.0f, 0L );
		
		// 비행모델 렌더
		RS( D3DRS_LIGHTING, TRUE );
		RS( D3DRS_AMBIENT, 0 );
		m_pd3dDevice->LightEnable( 0, TRUE );
		TSS( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		TSS( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		D3DXMatrixTranslation( &mL, m_pos.x, m_pos.y, m_pos.z );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &mL );
		m_pMesh->Render( m_pd3dDevice );
		
		// 배경 렌더
		RS( D3DRS_LIGHTING, FALSE );
		TSS( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		TSS( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );

		if(m_pEffect != NULL)
		{
			m_pEffect->SetTechnique( m_hTechnique );
			m_pEffect->Begin( NULL, 0 );
			m_pEffect->Pass( 0 );
			m_pd3dDevice->SetVertexDeclaration( m_pDecl );
			// 좌표변환
			m = m_mView * m_mProj;
			m_pEffect->SetMatrix( m_hmWVP, &m );

			m = mV * mP * mT;
			m_pEffect->SetMatrix( m_hmWVPT, &m );

			// 광원
			m_pEffect->SetVector( m_hvLightPos, &m_light );


			// 렌더
			m_pMeshBg->UseMeshMaterials(FALSE);
			m_pEffect->SetTexture(m_hDecaleMap
								, m_pMeshBg->m_pTextures[0]);
			m_pEffect->SetTexture(m_hShadowMap, m_pShadowTex);
			m_pMeshBg->Render( m_pd3dDevice );

			m_pEffect->End();
		}

		// 광원위치를 점으로 표시
		m = m_mView * m_mProj;
		D3DXVec4Transform(&v, &m_light, &m);
		m_pd3dDevice->GetViewport(&viewport);
		v.x = 0.5f*viewport.Width *( v.x/v.w + 1.0f);
		v.y = 0.5f*viewport.Height*(-v.y/v.w + 1.0f);
		v.z = v.z/v.w;

		typedef struct {FLOAT p[4]; DWORD color;} POINT_VERTEX;
		POINT_VERTEX PointVertex[4] = {
			// x y  z                                색   (R    G    B    A)
			{{v.x-2.0f, v.y-2.0f, v.z, v.w}, D3DCOLOR_RGBA(0xff,0xff,0xff,0),},
			{{v.x+2.0f, v.y-2.0f, v.z, v.w}, D3DCOLOR_RGBA(0xff,0xff,0xff,0),},
			{{v.x+2.0f, v.y+2.0f, v.z, v.w}, D3DCOLOR_RGBA(0xff,0xff,0xff,0),},
			{{v.x-2.0f, v.y+2.0f, v.z, v.w}, D3DCOLOR_RGBA(0xff,0xff,0xff,0),},
		};
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, PointVertex, sizeof( POINT_VERTEX ) );

		RenderText();				// 도움말 출력

#if 1 // 디버그용 텍스처 출력
		{
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,    D3DTOP_DISABLE);
		float scale = 128.0f;
		typedef struct {FLOAT p[4]; FLOAT tu, tv;} TVERTEX;

		TVERTEX Vertex[4] = {
			// x  y  z rhw tu tv
			{    0,    0,0, 1, 0, 0,},
			{scale,    0,0, 1, 1, 0,},
			{scale,scale,0, 1, 1, 1,},
			{    0,scale,0, 1, 0, 1,},
		};
		m_pd3dDevice->SetTexture( 0, m_pShadowTex );
		m_pd3dDevice->SetVertexShader(NULL);
		m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		m_pd3dDevice->SetPixelShader(0);
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof( TVERTEX ) );
		}
#endif		
        m_pd3dDevice->EndScene();	// 렌더종료
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
	// 그림자 텍스처
	SAFE_RELEASE(m_pShadowSurf);
	SAFE_RELEASE(m_pShadowTex);
	SAFE_RELEASE(m_pShadowTexZ);

	m_pMesh  ->InvalidateDeviceObjects();
	m_pMeshBg->InvalidateDeviceObjects();

	m_pEffect->OnLostDevice();	// 셰이더
	
    m_pFont->InvalidateDeviceObjects();	// 폰트

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() 에서 생성한 오브젝트 해제
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	m_pMesh  ->Destroy();
	m_pMeshBg->Destroy();

	SAFE_RELEASE( m_pEffect );		// 셰이더
	SAFE_RELEASE( m_pDecl );		// 정점선언

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
	SAFE_DELETE( m_pMeshBg );

    SAFE_DELETE( m_pFont );	// 폰트

    return S_OK;
}




