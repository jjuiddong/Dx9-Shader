//-------------------------------------------------------------
// File: main.cpp
//
// Desc: 우선순위버퍼 그림자
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

#define SHADOW_MAP_SIZE   2048

//-------------------------------------------------------------
// 디버그로 출력할 텍스처용 구조체
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
	m_pos						= D3DXVECTOR3( 1.5,1,2);

	m_pShadowMap	 = NULL;
	m_pShadowMapSurf = NULL;
	m_pShadowMapZ	 = NULL;

	m_pEffect = NULL;
	m_hTechnique  				= NULL;
	m_hmWVP						= NULL;
	m_hmWLP						= NULL;
	m_hmWVPT					= NULL;
	m_hvCol						= NULL;
	m_hvId						= NULL;
	m_hvDir						= NULL;
	m_htIdMap					= NULL;
	m_pDecl						= NULL;

	m_fWorldRotX                = -0.76479256f;
    m_fWorldRotY                = 3.2536204f;
	m_fViewZoom				    = 6.8188891f;
	m_LighPos					= D3DXVECTOR3( -6.0f, 6.0f,-2.0f );

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
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps,
                     DWORD dwBehavior,    D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    

	// 픽셀셰이더 버전체크
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) )
		return E_FAIL;

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

	// UFO읽기
	if( FAILED( hr=m_pMesh->Create( m_pd3dDevice, _T("ufo.x"))))
        return DXTRACE_ERR( "LoadCar", hr );
	m_pMesh->SetFVF(m_pd3dDevice, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );
	// 지면읽기
	if( FAILED( hr=m_pMeshBg->Create( m_pd3dDevice, _T("map.x"))))
        return DXTRACE_ERR( "LoadChess", hr );
	m_pMeshBg->UseMeshMaterials(FALSE);
        
    // 렌더링시 텍스처와 재질설정을 하지 않는다
	m_pMesh  ->UseMeshMaterials(FALSE);
	m_pMeshBg->UseMeshMaterials(FALSE);

	// 폰트
    m_pFont->InitDeviceObjects( m_pd3dDevice );

	// 셰이더 읽기
	LPD3DXBUFFER pErr;
    if( FAILED( hr = D3DXCreateEffectFromFile(
					m_pd3dDevice, "hlsl.fx", NULL, NULL, 
				D3DXSHADER_DEBUG , NULL, &m_pEffect, &pErr ) ) ){
		MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer(), "ERROR", MB_OK);
		return DXTRACE_ERR( "CreateEffectFromFile", hr );
	}
	m_hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
	m_hmWVP = m_pEffect->GetParameterByName( NULL, "mWVP" );
	m_hmWLP = m_pEffect->GetParameterByName( NULL, "mWLP" );
	m_hmWVPT= m_pEffect->GetParameterByName( NULL, "mWVPT" );
	m_hvCol = m_pEffect->GetParameterByName( NULL, "vCol" );
	m_hvId  = m_pEffect->GetParameterByName( NULL, "vId" );
	m_hvDir = m_pEffect->GetParameterByName( NULL, "vLightDir" );
	m_htIdMap = m_pEffect->GetParameterByName( NULL, "IdMap" );

	// 정점선언오브젝트 생성(지형용)
	D3DVERTEXELEMENT9 decl[] =
	{
		{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
		{0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( decl, &m_pDecl )))
		return DXTRACE_ERR ("CreateVertexDeclaration", hr);

    return S_OK;
}

//-------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: 화면크기가 변했을때 호출됨
//       확보한 메모리는 InvalidateDeviceObjects()에서 해제
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	// 우선순위버퍼 생성
	if (FAILED(m_pd3dDevice->CreateTexture(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 1, 
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pShadowMap, NULL)))
		return E_FAIL;
	if (FAILED(m_pShadowMap->GetSurfaceLevel(0, &m_pShadowMapSurf)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateDepthStencilSurface(SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 
		D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pShadowMapZ, NULL)))
		return E_FAIL;

	// 메시
	m_pMesh  ->RestoreDeviceObjects( m_pd3dDevice );
	m_pMeshBg->RestoreDeviceObjects( m_pd3dDevice );

    // 재질설정
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

	// 단축매크로
	#define RS   m_pd3dDevice->SetRenderState
	#define TSS  m_pd3dDevice->SetTextureStageState
	#define SAMP m_pd3dDevice->SetSamplerState

    // 렌더링 상태설정
    RS( D3DRS_DITHERENABLE,   FALSE );
    RS( D3DRS_SPECULARENABLE, FALSE );
    RS( D3DRS_ZENABLE,        TRUE );
    RS( D3DRS_AMBIENT,        0x000F0F0F );
    
    TSS( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    TSS( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    TSS( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    TSS( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    TSS( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    TSS( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

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

	m_pEffect->OnResetDevice();

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
}

//-------------------------------------------------------------
// 각각의 모델 렌더
//-------------------------------------------------------------
VOID CMyD3DApplication::DrawModel( int pass )
{
    D3DXMATRIX m, mL;
	D3DXVECTOR3 vDir;
	D3DXVECTOR4 v;
	D3DMATERIAL9 *pMtrl;
	DWORD i;
	
	//---------------------------------------------------------
	// 행렬 생성
	//---------------------------------------------------------
	// 로컬-투영 행렬
	D3DXMATRIX mVP = m_mWorld * m_mView * m_mProj;
	
	// 투영공간에서 텍스처공간으로 변환
	float fOffsetX = 0.5f + (0.5f / (float)SHADOW_MAP_SIZE);
	float fOffsetY = 0.5f + (0.5f / (float)SHADOW_MAP_SIZE);
	D3DXMATRIX mScaleBias(	0.5f,     0.0f,     0.0f,   0.0f,
							0.0f,    -0.5f,     0.0f,   0.0f,
							0.0f,     0.0f,     0.0f,	0.0f,
							fOffsetX, fOffsetY, 0.0f,   1.0f );

	//---------------------------------------------------------
	// UFO
	//---------------------------------------------------------
	v.x = 0;
	m_pEffect->SetVector( m_hvId, &v );

	// 월드행렬 생성
	D3DXMatrixTranslation( &mL, m_pos.x, m_pos.y, m_pos.z );
	
	switch(pass){
	case 0:// 그림자맵 생성
		// 광원공간으로의 투영행렬
		m = mL * m_mLightVP;
        m_pEffect->SetMatrix( m_hmWLP, &m );
        
		m_pMesh  ->Render( m_pd3dDevice );// 렌더
		break;
	default:// 장면렌더
		m_pEffect->Pass( 2 );	// 패스(2) 설정
		m = mL * mVP;       // 일반 투영행렬
        m_pEffect->SetMatrix( m_hmWVP, &m );
        
		m = mL * m_mLightVP;// 광원공간으로의 투영행렬
        m_pEffect->SetMatrix( m_hmWLP, &m );
        
		m = m * mScaleBias; // 텍스처공간으로의 투영행렬
        m_pEffect->SetMatrix( m_hmWVPT, &m );
        
        // 로컬좌표계에서의 광원벡터
		D3DXMatrixInverse( &m, NULL, &mL);
		D3DXVec3Transform( &v, &m_LighPos, &m );
		D3DXVec4Normalize( &v, &v );v.w = 0;
		m_pEffect->SetVector( m_hvDir, &v );

		// 메시내부 각각의 부품을 렌더
		pMtrl = m_pMesh->m_pMaterials;
        for( i=0; i<m_pMesh->m_dwNumMaterials; i++ ) {
			v.x = pMtrl->Diffuse.r;
			v.y = pMtrl->Diffuse.g;
			v.z = pMtrl->Diffuse.b;
			v.w = pMtrl->Diffuse.a;
			m_pEffect->SetVector( m_hvCol, &v );        // 정점색
            m_pMesh->m_pLocalMesh->DrawSubset( i );	// 렌더
			pMtrl++;
        }
		break;
	}

	//---------------------------------------------------------
	// 지형
	//---------------------------------------------------------
	v.x = 0.5f;
	v.y = 0.5f;
	v.z = 0.5f;
	v.w = 1.0f;
	m_pEffect->SetVector( m_hvId, &v );       // 정점색

	// 월드행렬 생성
	D3DXMatrixIdentity( &mL );
	switch(pass){
	case 0:// 그림자맵 생성
		// 광원공간으로의 투영행렬
		m = mL * m_mLightVP;
        m_pEffect->SetMatrix( m_hmWLP, &m );
		m_pMeshBg->Render( m_pd3dDevice );// 렌더
		break;
	case 1:// 장면렌더
		m_pEffect->Pass( 1 );	// 패스(1) 설정
		m = mL * mVP;       // 일반 투영행렬
        m_pEffect->SetMatrix( m_hmWVP, &m );
        
		m = mL * m_mLightVP;// 광원공간으로의 투영행렬
        m_pEffect->SetMatrix( m_hmWLP, &m );
		
		m = m * mScaleBias; // 텍스처공간으로의 투영행렬
        m_pEffect->SetMatrix( m_hmWVPT, &m );
        
        // 로컬좌표계에서의 광원벡터
		D3DXMatrixInverse( &m, NULL, &mL);
		D3DXVec3Transform( &v, &m_LighPos, &m );
		D3DXVec4Normalize( &v, &v );v.w = 0;
		m_pEffect->SetVector( m_hvDir, &v );
		
		// 메시내부 각각의 부품을 렌더
		pMtrl = m_pMeshBg->m_pMaterials;
        for( i=0; i<m_pMeshBg->m_dwNumMaterials; i++ ) {
			v.x = pMtrl->Diffuse.r;
			v.y = pMtrl->Diffuse.g;
			v.z = pMtrl->Diffuse.b;
			v.w = pMtrl->Diffuse.a;
			m_pEffect->SetVector( m_hvCol, &v );       // 정점색
			m_pEffect->SetTexture("DecaleMap", m_pMeshBg->m_pTextures[i] );
            m_pMeshBg->m_pLocalMesh->DrawSubset(i); // 렌더
			pMtrl++;
        }
		break;
	}

}


//-------------------------------------------------------------
// Name: Render()
// Desc: 화면 렌더
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    D3DXMATRIX mLP, mView, mProj;
	LPDIRECT3DSURFACE9 pOldBackBuffer, pOldZBuffer;
	D3DVIEWPORT9 oldViewport;

	//---------------------------------------------------------
	// 행렬생성
	//---------------------------------------------------------
    // 광원방향에서 본 투영공간에의 행렬
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &mView, &m_LighPos, &vLookatPt, &vUp );
    
    D3DXMatrixPerspectiveFovLH( &mProj
							, 0.5f*D3DX_PI		// 시야각
							, 1.0f				// 종횡비
							, 1.0f, 12.0f );	// near far
	m_mLightVP = mView * mProj;

	//---------------------------------------------------------
	// 렌더
	//---------------------------------------------------------
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		if( m_pEffect != NULL ) 
		{
			//-------------------------------------------------
			// 셰이더 설정
			//-------------------------------------------------
			m_pEffect->SetTechnique( m_hTechnique );
			m_pEffect->Begin( NULL, 0 );

			//-------------------------------------------------
			// 렌더링타겟 보존
			//-------------------------------------------------
			m_pd3dDevice->GetRenderTarget(0, &pOldBackBuffer);
			m_pd3dDevice->GetDepthStencilSurface(&pOldZBuffer);
			m_pd3dDevice->GetViewport(&oldViewport);

			//-------------------------------------------------
			// 렌더링타겟 변경
			//-------------------------------------------------
			m_pd3dDevice->SetRenderTarget(0, m_pShadowMapSurf);
			m_pd3dDevice->SetDepthStencilSurface(m_pShadowMapZ);
			// 뷰포트 변경
			D3DVIEWPORT9 viewport = {0,0      // 좌측상단좌표
							, SHADOW_MAP_SIZE // 폭
							, SHADOW_MAP_SIZE // 높이
							, 0.0f,1.0f};     // 전면,후면
			m_pd3dDevice->SetViewport(&viewport);

			// 그림자맵 클리어
			m_pd3dDevice->Clear(0L, NULL
							, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
							, 0xFFFFFFFF, 1.0f, 0L);

			//-------------------------------------------------
			// 1패스:그림자맵 생성
			//-------------------------------------------------
			m_pEffect->Pass( 0 );	// 패스(0)설정
			DrawModel( 0 );			// 모델 렌더

			//-------------------------------------------------
			// 렌더링타겟 복구
			//-------------------------------------------------
			m_pd3dDevice->SetRenderTarget(0, pOldBackBuffer);
			m_pd3dDevice->SetDepthStencilSurface(pOldZBuffer);
			m_pd3dDevice->SetViewport(&oldViewport);
			pOldBackBuffer->Release();
			pOldZBuffer->Release();

			//-------------------------------------------------
			// 2패스:장면렌더
			//-------------------------------------------------
			// 버퍼클리어
			m_pd3dDevice->Clear( 0L, NULL
							, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER
							, 0x00404080, 1.0f, 0L );

			//-------------------------------------------------
			// 렌더
			//-------------------------------------------------
			// 텍스처 설정
			m_pEffect->SetTexture(m_htIdMap, m_pShadowMap);
			DrawModel( 1 );			// 모델 렌더
			m_pd3dDevice->SetVertexDeclaration( m_pDecl );

			m_pEffect->End();
		}

		RenderText();				// 도움말 출력

#if 1 // 디버그용 텍스처 출력
		{
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,    D3DTOP_DISABLE);
		float scale = 128.0f;
		TVERTEX Vertex[4] = {
			// x  y  z rhw tu tv
			{    0,    0,0, 1, 0, 0,},
			{scale,    0,0, 1, 1, 0,},
			{scale,scale,0, 1, 1, 1,},
			{    0,scale,0, 1, 0, 1,},
		};
		m_pd3dDevice->SetTexture( 0, m_pShadowMap );
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

    FLOAT fNextLine = 40.0f; // 출력높이

    // 조작방법
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    lstrcpy( szMsg, TEXT("Use arrow keys to rotate camera") );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
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
	// 그림자 텍스처
	SAFE_RELEASE(m_pShadowMapSurf);
	SAFE_RELEASE(m_pShadowMap);
	SAFE_RELEASE(m_pShadowMapZ);

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
    // 셰이더
	SAFE_RELEASE( m_pEffect );
	SAFE_RELEASE( m_pDecl );		// 정점선언
	
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




