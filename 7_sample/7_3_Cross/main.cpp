//-------------------------------------------------------------
// File: main.cpp
//
// Desc: 크로스필터
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
#include "framework/DXUtil.h"
#include "framework/D3DEnumeration.h"
#include "framework/D3DSettings.h"
#include "framework/D3DApp.h"
#include "framework/D3DFont.h"
#include "framework/D3DFile.h"
#include "framework/D3DUtil.h"
#include "resource.h"
#include "main.h"

// 단축매크로
#define RS   m_pd3dDevice->SetRenderState
#define TSS  m_pd3dDevice->SetTextureStageState
#define SAMP m_pd3dDevice->SetSamplerState


//-------------------------------------------------------------
// 정점선언
//-------------------------------------------------------------
D3DVERTEXELEMENT9 decl[] =
{
	{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
	{0, 24, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,	0},
	{0, 36, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};

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
	m_pTexScene					= NULL;
	m_pSurfScene				= NULL;
	m_pTexSceneScaled			= NULL;
	m_pSurfSceneScaled			= NULL;
	m_pTexBrightPass			= NULL;
	m_pSurfBrightPass			= NULL;
	m_pTexStarSource			= NULL;
	m_pSurfStarSource			= NULL;
	ZeroMemory( m_apTexStar,  sizeof(m_apTexStar)  );
	ZeroMemory( m_apSurfStar, sizeof(m_apSurfStar) );

	m_pMesh						= new CD3DMesh();
	m_pMeshBg					= new CD3DMesh();
	m_pDecl						= NULL;
	m_pNormalMap				= NULL;

	m_pEffect					= NULL;
	m_hTechnique  				= NULL;
	m_hmWVP  					= NULL;
	m_hvLightDir  				= NULL;
	m_hvColor	  				= NULL;
	m_hvEyePos					= NULL;
	m_htDecaleTex  				= NULL;
	m_htNormalMap				= NULL;

	m_fWorldRotX                = -D3DX_PI/10;
    m_fWorldRotY                = D3DX_PI/2;
	m_fViewZoom				    = 5.0f;

	m_dwCreationWidth           = 640;
    m_dwCreationHeight          = 480;
    m_strWindowTitle            = TEXT( "main" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
}



//-----------------------------------------------------------------------------
// Name: DrawFullScreenQuad
// Desc: 사각형을 전체화면에 렌더한다
//-----------------------------------------------------------------------------
void CMyD3DApplication::DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV)
{
    D3DSURFACE_DESC desc;
    PDIRECT3DSURFACE9 pSurf;

    // 렌더링타겟 정보(폭과 높이)를 얻어낸다
    m_pd3dDevice->GetRenderTarget(0, &pSurf);
    pSurf->GetDesc(&desc);
    pSurf->Release();
    FLOAT w = (FLOAT)desc.Width;
    FLOAT h = (FLOAT)desc.Height;

	typedef struct{
		float p[4]; // 위치좌표
		float t[2]; // 텍스처좌표
	} ScreenVertex;
	ScreenVertex svQuad[4] = {
		// x       y      z     w       u       v
		{0-0.5f, 0-0.5f, 0.5f, 1.0f, fLeftU,  fTopV,},
		{w-0.5f, 0-0.5f, 0.5f, 1.0f, fRightU, fTopV,},
		{0-0.5f, h-0.5f, 0.5f, 1.0f, fLeftU,  fBottomV,},
		{w-0.5f, h-0.5f, 0.5f, 1.0f, fRightU, fBottomV,},
	};

    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);	// 깊이버퍼 죽이기
    m_pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
    m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, svQuad, sizeof(ScreenVertex));
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);	// 깊이버퍼 부활
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
    LPDIRECT3DTEXTURE9	pHeightTexture;
    D3DSURFACE_DESC desc;

	// 법선맵 생성
    D3DUtil_CreateTexture( m_pd3dDevice,// 높이맵 읽기
		_T("height.bmp"), &pHeightTexture );
    pHeightTexture->GetLevelDesc(0,&desc);// 텍스처정보 얻기
    D3DXCreateTexture(m_pd3dDevice, desc.Width, desc.Height, 0, 0, 
        D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_pNormalMap);// 텍스처 생서
    D3DXComputeNormalMap(m_pNormalMap,	// 법선맵 생성
        pHeightTexture, NULL, 0, D3DX_CHANNEL_RED, 5.0f);
    SAFE_RELEASE( pHeightTexture );		// 사용하지 않는 리소스 반환

	// 정점선언 오브젝트 생성
	if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration(
										decl, &m_pDecl )))
		return DXTRACE_ERR ("CreateVertexDeclaration", hr);
	
	// 주전자 읽기
	if(FAILED(hr=m_pMesh  ->Create( m_pd3dDevice, _T("t-pot.x"))))
        return DXTRACE_ERR( "Load Object", hr );
        
	// 지형 읽기
	if(FAILED(hr=m_pMeshBg->Create( m_pd3dDevice, _T("map.x"))))
        return DXTRACE_ERR( "Load Ground", hr );
        
	// 셰이더 읽기
    LPD3DXBUFFER pErr=NULL;
    if( FAILED( hr = D3DXCreateEffectFromFile(
                m_pd3dDevice, "hlsl.fx", NULL, NULL, 
                0 , NULL, &m_pEffect, &pErr ))){
        // 셰이더 읽시 실패
        MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer()
                    , "ERROR", MB_OK);
    }else{
		m_hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
		m_hmWVP      = m_pEffect->GetParameterByName( NULL, "mWVP" );
		m_hvLightDir = m_pEffect->GetParameterByName( NULL, "vLightDir" );
		m_hvColor    = m_pEffect->GetParameterByName( NULL, "vColor" );
		m_hvEyePos   = m_pEffect->GetParameterByName( NULL, "vEyePos" );
		m_htDecaleTex= m_pEffect->GetParameterByName( NULL, "DecaleTex" );
		m_htNormalMap= m_pEffect->GetParameterByName( NULL, "NormalMap" );
    }
    SAFE_RELEASE(pErr);

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
    HRESULT hr;

	//---------------------------------------------------------
	// 장면을 렌더할 HDR 포맷 렌더링타겟
	//---------------------------------------------------------
    hr = m_pd3dDevice->CreateTexture(
				m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 
                1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, 
                D3DPOOL_DEFAULT, &m_pTexScene, NULL);
    if( FAILED(hr) ) return hr;
	hr = m_pTexScene->GetSurfaceLevel( 0, &m_pSurfScene );
    if( FAILED(hr) ) return hr;

	//---------------------------------------------------------
	// 축소버퍼
	//---------------------------------------------------------
	// 축소버퍼의 기본크기(4의 배수)
    m_dwCropWidth  = m_d3dsdBackBuffer.Width  - m_d3dsdBackBuffer.Width  % 4;
    m_dwCropHeight = m_d3dsdBackBuffer.Height - m_d3dsdBackBuffer.Height % 4;

    hr = m_pd3dDevice->CreateTexture(
				m_dwCropWidth / 4, m_dwCropHeight / 4, 
                1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F,
				D3DPOOL_DEFAULT, &m_pTexSceneScaled, NULL);
    if( FAILED(hr) ) return hr;
	hr = m_pTexSceneScaled->GetSurfaceLevel( 0, &m_pSurfSceneScaled );
    if( FAILED(hr) ) return hr;

	//---------------------------------------------------------
	// 휘도추출
	//---------------------------------------------------------
    hr = m_pd3dDevice->CreateTexture(
				m_dwCropWidth / 4 + 2, m_dwCropHeight / 4 + 2, 
                1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
				D3DPOOL_DEFAULT, &m_pTexBrightPass, NULL);
    if( FAILED(hr) ) return hr;
	hr = m_pTexBrightPass->GetSurfaceLevel( 0, &m_pSurfBrightPass );
    if( FAILED(hr) ) return hr;
	// 주변부를 검게칠한다
	m_pd3dDevice->ColorFill( m_pSurfBrightPass, NULL
								, D3DCOLOR_ARGB(0, 0, 0, 0) );

	//---------------------------------------------------------
	// 날리지 않도록 흐릿하게 뭉개는 텍스처
	//---------------------------------------------------------
    hr = m_pd3dDevice->CreateTexture(
				m_dwCropWidth / 4 + 2, m_dwCropHeight / 4 + 2, 
                1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
				D3DPOOL_DEFAULT, &m_pTexStarSource, NULL);
    if( FAILED(hr) ) return hr;
	hr = m_pTexStarSource->GetSurfaceLevel( 0, &m_pSurfStarSource );
    if( FAILED(hr) ) return hr;
	// 주변부를 검게칠한다
	m_pd3dDevice->ColorFill( m_pSurfStarSource, NULL
								, D3DCOLOR_ARGB(0, 0, 0, 0) );

	//---------------------------------------------------------
	// 광선텍스처
	//---------------------------------------------------------
    for(int i=0; i < NUM_STAR_TEXTURES; i++ ) {
        hr = m_pd3dDevice->CreateTexture(
			    m_dwCropWidth /4, m_dwCropHeight / 4,
                1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F,
				D3DPOOL_DEFAULT, &m_apTexStar[i], NULL );
        if( FAILED(hr) ) return hr;
		hr = m_apTexStar[i]->GetSurfaceLevel( 0, &m_apSurfStar[i] );
		if( FAILED(hr) ) return hr;
    }

	// 이펙트
	if(m_pEffect) m_pEffect->OnResetDevice();

	// 메시
	m_pMeshBg->RestoreDeviceObjects( m_pd3dDevice );

	//---------------------------------------------------------
	// 메시직접 처리
	//---------------------------------------------------------
	if( m_pMesh && m_pMesh->GetSysMemMesh() ){
		LPD3DXMESH pMesh;

		m_pMesh->GetSysMemMesh()->CloneMesh(
			m_pMesh->GetSysMemMesh()->GetOptions(), decl,
			m_pd3dDevice, &pMesh );
		D3DXComputeNormals( pMesh, NULL );
		D3DXComputeTangent( pMesh, 0, 0, 0, TRUE, NULL );

		SAFE_RELEASE(m_pMesh->m_pLocalMesh);
		m_pMesh->m_pLocalMesh = pMesh;
	}

    // 렌더링 상태설정
    RS( D3DRS_ZENABLE,        TRUE );
	RS( D3DRS_LIGHTING, FALSE );
    
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
	D3DXMATRIX mCamera;

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

    D3DXMatrixMultiply( &mCamera, &matRotY, &matRotX );
	
	//---------------------------------------------------------
	// 뷰행렬 설정
	//---------------------------------------------------------
	// 줌
    if( m_UserInput.bZoomIn && !m_UserInput.bZoomOut )
        m_fViewZoom += m_fElapsedTime;
    else if( m_UserInput.bZoomOut && !m_UserInput.bZoomIn )
        m_fViewZoom -= m_fElapsedTime;

    m_vFromPt   = D3DXVECTOR4( 0.0f, 0.f, -m_fViewZoom,1 );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, (D3DXVECTOR3*)&m_vFromPt, &vLookatPt, &vUpVec );
	
	m_mView = mCamera * m_mView;

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
// Name: RenderScene()
// Desc: 화면 렌더
//-------------------------------------------------------------
void CMyD3DApplication::RenderScene()
{
    D3DXMATRIX m, mT, mR, mView, mProj;
	D3DXMATRIX mWorld;
	D3DXVECTOR4 v, light_pos, eye_pos;
	DWORD i;

	//-----------------------------------------------------
	// 지형렌더
	//-----------------------------------------------------
	// 월드행렬
	D3DXMatrixScaling( &m,  3.0f, 3.0f, 3.0f );
	D3DXMatrixRotationY( &mR,  D3DX_PI );
	D3DXMatrixTranslation( &mT, 0.0f,-2.0f ,0.0f );
	mWorld = m * mR * mT;

	// 행렬설정
	m_pd3dDevice->SetTransform( D3DTS_WORLD,  &mWorld);
	m_pd3dDevice->SetTransform( D3DTS_VIEW,  &m_mView );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION,  &m_mProj );

	TSS( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	TSS( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pMeshBg->Render( m_pd3dDevice );

	if( m_pEffect != NULL ) 
	{
		//-------------------------------------------------
		// 셰이더 설정
		//-------------------------------------------------
		m_pEffect->SetTechnique( m_hTechnique );
		m_pEffect->Begin( NULL, 0 );
		m_pEffect->Pass( 0 );
		
		//-------------------------------------------------
		// 주전자 렌더
		//-------------------------------------------------

		// 월드행렬(회전)
		D3DXMatrixRotationY( &mWorld,  m_fTime );

		// 로컬-투영변환행렬
		m = mWorld * m_mView * m_mProj;
		m_pEffect->SetMatrix( m_hmWVP, &m );

		// 광원방향(로컬좌표계)
		light_pos = D3DXVECTOR4( -0.577f, -0.577f, -0.577f,0);
		D3DXMatrixInverse( &m, NULL, &mWorld);
		D3DXVec4Transform( &v, &light_pos, &m );
		D3DXVec3Normalize( (D3DXVECTOR3 *)&v, (D3DXVECTOR3 *)&v );
		v.w = -0.5f;		// 환경광밝기
		m_pEffect->SetVector( m_hvLightDir, &v );
		
		// 시점(로컬좌표계)
		m = mWorld * m_mView;
		D3DXMatrixInverse( &m, NULL, &m);
		v = D3DXVECTOR4( 0, 0, 0, 1);
		D3DXVec4Transform( &v, &v, &m );
		m_pEffect->SetVector( m_hvEyePos, &v );

		// 법선맵
		m_pEffect->SetTexture( m_htNormalMap, m_pNormalMap );
		// 정점선언
		m_pd3dDevice->SetVertexDeclaration( m_pDecl );

		D3DMATERIAL9 *pMtrl = m_pMesh->m_pMaterials;
		for( i=0; i<m_pMesh->m_dwNumMaterials; i++ ) {
			v.x = pMtrl->Diffuse.r;
			v.y = pMtrl->Diffuse.g;
			v.z = pMtrl->Diffuse.b;
			m_pEffect->SetVector( m_hvColor, &v );
			m_pEffect->SetTexture( m_htDecaleTex, m_pMesh->m_pTextures[i] );
			m_pMesh->m_pLocalMesh->DrawSubset( i );  // 렌더
			pMtrl++;
		}

		m_pEffect->End();
	}
}


//-----------------------------------------------------------------------------
// Name: GetTextureCoords()
// Desc: 입력텍스처와 출력텍스처에서 텍스처좌표 계산
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::GetTextureCoords( PDIRECT3DTEXTURE9 pTexSrc, RECT* pRectSrc, 
                          PDIRECT3DTEXTURE9 pTexDest, RECT* pRectDest, CoordRect* pCoords )
{
    D3DSURFACE_DESC desc;

    if( pTexSrc == NULL || pTexDest == NULL || pCoords == NULL )
		return E_INVALIDARG;

    // 기본적으로 텍스처좌표를 그대로 출력
    pCoords->u0 = 0.0f;
    pCoords->v0 = 0.0f;
    pCoords->u1 = 1.0f; 
    pCoords->v1 = 1.0f;

    // 입력원본 표면에 관한 보정
    if( pRectSrc != NULL ) {
        pTexSrc->GetLevelDesc( 0, &desc );// 텍스처정보를 얻는다
        // 전송원본
        pCoords->u0 += pRectSrc->left                   / desc.Width;
        pCoords->v0 += pRectSrc->top                    / desc.Height;
        pCoords->u1 -= (desc.Width  - pRectSrc->right)  / desc.Width;
        pCoords->v1 -= (desc.Height - pRectSrc->bottom) / desc.Height;
    }

    // 출력할 표면에 관한 보정
    if( pRectDest != NULL ) {
        pTexDest->GetLevelDesc( 0, &desc );// 텍스처정보를 얻는다
        // 출력할 형태에 따라 텍스처좌표를 맞춘다
        pCoords->u0 -= pRectDest->left                   / desc.Width;
        pCoords->v0 -= pRectDest->top                    / desc.Height;
        pCoords->u1 += (desc.Width  - pRectDest->right)  / desc.Width;
        pCoords->v1 += (desc.Height - pRectDest->bottom) / desc.Height;
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Scene_To_SceneScaled()
// Desc: m_pTexScene를 1/4로 해서 m_pTexSceneScale에 넣는다
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Scene_To_SceneScaled()
{
    // 초과한 부분에 중심부분을 복사한다
    CoordRect coords;
    RECT rectSrc;
    rectSrc.left   = (m_d3dsdBackBuffer.Width  - m_dwCropWidth ) / 2;
    rectSrc.top    = (m_d3dsdBackBuffer.Height - m_dwCropHeight) / 2;
    rectSrc.right  = rectSrc.left + m_dwCropWidth;
    rectSrc.bottom = rectSrc.top  + m_dwCropHeight;
    // 렌더링타겟에 맞추여 텍스처좌표 계산
    GetTextureCoords( m_pTexScene, &rectSrc, m_pTexSceneScaled, NULL, &coords );

    // 주변 16텍셀을 샘플링점으로 해서
	// 0.5는 중심에 맞추귀위한 보정
    int index=0;
    D3DXVECTOR2 offsets[MAX_SAMPLES];

    for( int y=0; y < 4; y++ ) {
        for( int x=0; x < 4; x++ ) {
            offsets[ index ].x = (x - 1.5f) / m_d3dsdBackBuffer.Width;
            offsets[ index ].y = (y - 1.5f) / m_d3dsdBackBuffer.Height;
            index++;
        }
    }
	m_pEffect->SetValue("g_avSampleOffsets", offsets, sizeof(offsets));
    
	// 16텍셀을 샘플링해서 그 평균값을 축소버퍼에 출력하낟
    m_pd3dDevice->SetRenderTarget( 0, m_pSurfSceneScaled );
    m_pEffect->SetTechnique("DownScale4x4");
	m_pEffect->Begin(NULL, 0);
	m_pEffect->Pass(0);
    m_pd3dDevice->SetTexture( 0, m_pTexScene );
	DrawFullScreenQuad( coords.u0, coords.v0, coords.u1, coords.v1 );
	m_pEffect->End();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SceneScaled_To_BrightPass
// Desc: 축소버퍼에 복사할때 밝은 부분만 추출한다
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::SceneScaled_To_BrightPass()
{
    // 출력할 곳의 정보로부터 렌더링할 크기 설정
    D3DSURFACE_DESC desc;
	m_pTexBrightPass->GetLevelDesc( 0, &desc );
	RECT rectDest = {0,0,desc.Width,desc.Height};
    InflateRect( &rectDest, -1, -1 );// 뢯쀍먩궻묈궖궠귩덇됷귟룷궠궘궥귡
    m_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
    m_pd3dDevice->SetScissorRect( &rectDest );
	
	// 전체화면 복사
    m_pd3dDevice->SetRenderTarget( 0, m_pSurfBrightPass );
    m_pEffect->SetTechnique("BrightPassFilter");
    m_pEffect->Begin(NULL, 0);
    m_pEffect->Pass(0);
    m_pd3dDevice->SetTexture( 0, m_pTexSceneScaled );
	DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );
    m_pEffect->End();

    m_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetGaussBlur5x5
// Desc: 블러를 먹일때 가우스형 뭉개기가 되도록 계수 계산
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::GetGaussBlur5x5(
	DWORD dwD3DTexWidth, DWORD dwD3DTexHeight,
	D3DXVECTOR2* avTexCoordOffset, D3DXVECTOR4* avSampleWeight)
{
    float tu = 1.0f / (float)dwD3DTexWidth ;
    float tv = 1.0f / (float)dwD3DTexHeight ;

    float totalWeight = 0.0f;
    int index=0;
    for( int x = -2; x <= 2; x++ ) {
        for( int y = -2; y <= 2; y++ ) {
			// 계수가 작은 부분은 소거
            if( 2 < abs(x) + abs(y) ) continue;

            avTexCoordOffset[index] = D3DXVECTOR2( x * tu, y * tv );
			float fx = (FLOAT)x;
			float fy = (FLOAT)y;
			avSampleWeight[index].x = avSampleWeight[index].y =
			avSampleWeight[index].z = avSampleWeight[index].w
				= expf( -(fx*fx + fy*fy)/(2*1.0f) );
            totalWeight += avSampleWeight[index].x;

            index++;
        }
    }

	// 가중치 합계는 1.0
    for( int i=0; i < index; i++ ) avSampleWeight[i] *= 1.0f/totalWeight;

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: BrightPass_To_StarSource
// Desc: 축소버퍼에 복사할때 계단현상을 방지하기 위해서
//       가우스형 뭉개기를 한다
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::BrightPass_To_StarSource()
{
    // 출력할 곳의 정보로 부터 렌더링할 크기 설정
    D3DSURFACE_DESC desc;
	m_pTexStarSource->GetLevelDesc( 0, &desc );
	RECT rectDest = {0,0,desc.Width,desc.Height};
    InflateRect( &rectDest, -1, -1 );// 출력할곳의 크기를 1만큼 줄인다
    m_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
    m_pd3dDevice->SetScissorRect( &rectDest );

    // 텍스처좌표 산출
    CoordRect coords;
    GetTextureCoords( m_pTexBrightPass, NULL, m_pTexStarSource,
					&rectDest, &coords );

    // 원본 이미지의 크기로부터 가우스분포계수를 계산
    D3DXVECTOR2 offsets[MAX_SAMPLES];
    D3DXVECTOR4 weights[MAX_SAMPLES];
    m_pTexBrightPass->GetLevelDesc( 0, &desc );
    GetGaussBlur5x5( desc.Width, desc.Height, offsets, weights );
    m_pEffect->SetValue("g_avSampleOffsets", offsets, sizeof(offsets));
    m_pEffect->SetValue("g_avSampleWeights", weights, sizeof(weights));
    
    // 가우스 뭉개기
    m_pEffect->SetTechnique("GaussBlur5x5");
    m_pd3dDevice->SetRenderTarget( 0, m_pSurfStarSource );
    m_pEffect->Begin(NULL, 0);
    m_pEffect->Pass(0);
    m_pd3dDevice->SetTexture( 0, m_pTexBrightPass );
	DrawFullScreenQuad( coords.u0, coords.v0, coords.u1, coords.v1 );
    m_pEffect->End();

    m_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderStar()
// Desc: 광선만들기
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderStar()
{
    // 이펙트에서 사용하는 상수 설정
    static const s_maxPasses = 3;
    static const int nSamples = 8;
	// 광선색
    static const D3DXCOLOR s_colorWhite(0.63f, 0.63f, 0.63f, 0.0f) ;
	static const D3DXCOLOR s_ChromaticAberrationColor[8] = {
		D3DXCOLOR(0.5f, 0.5f, 0.5f,  0.0f),	// 뵏
		D3DXCOLOR(0.8f, 0.3f, 0.3f,  0.0f), // 먗
		D3DXCOLOR(1.0f, 0.2f, 0.2f,  0.0f),	// 먗
		D3DXCOLOR(0.5f, 0.2f, 0.6f,  0.0f), // 럤
		D3DXCOLOR(0.2f, 0.2f, 1.0f,  0.0f),	// 먃
		D3DXCOLOR(0.2f, 0.3f, 0.7f,  0.0f), // 먃
		D3DXCOLOR(0.2f, 0.6f, 0.2f,  0.0f),	// 쀎
		D3DXCOLOR(0.3f, 0.5f, 0.3f,  0.0f), // 쀎
	} ;

    static D3DXVECTOR4 s_aaColor[s_maxPasses][nSamples];

    for (int p = 0 ; p < s_maxPasses ; p ++) {
		// 중심에서의 거리에따라 광선색을 만든다
        float ratio = (float)(p + 1) / (float)s_maxPasses ;
        // 각각 샘플링해서 적당한 색을 만든다
        for (int s = 0 ; s < nSamples ; s ++) {
            D3DXCOLOR chromaticAberrColor ;
            D3DXColorLerp(&chromaticAberrColor,
				&( s_ChromaticAberrationColor[s] ),
                &s_colorWhite, ratio) ;
			// 전체적인 색 변화를 조정
            D3DXColorLerp( (D3DXCOLOR*)&( s_aaColor[p][s] ),
                &s_colorWhite, &chromaticAberrColor, 0.7f ) ;
        }
    }

    float radOffset = m_fWorldRotY/5;// 시점에 따라 회전

    // 원본화면의 폭과 높이 조사
    D3DSURFACE_DESC desc;
    m_pSurfStarSource->GetDesc( &desc );
    float srcW = (FLOAT) desc.Width;
    float srcH = (FLOAT) desc.Height;
	
	int nStarLines = 6;// 광선의 줄기 개수
    for (int d = 0 ; d < nStarLines ; d ++) {    // 방향에 따라 루프
        PDIRECT3DTEXTURE9 pTexSource = m_pTexStarSource;
        float rad = radOffset + 2*d*D3DX_PI/(FLOAT)nStarLines;// 각도
        float sn = sinf(rad);
        float cs = cosf(rad);
	    D3DXVECTOR2 vtStepUV = D3DXVECTOR2(0.3f * sn / srcW,
										   0.3f * cs / srcH);
        
        float attnPowScale = (atanf(D3DX_PI/4) + 0.1f) *
                       (160.0f + 120.0f) / (srcW + srcH);

        int iWorkTexture = 0;
        for (int p = 0 ; p < s_maxPasses; p++) {
			// 렌더링할 곳 결정
		    PDIRECT3DSURFACE9 pSurfDest = NULL;
            if (p == s_maxPasses - 1) {
                // 최종 패스는 보존용 보퍼에 확보한다
                pSurfDest = m_apSurfStar[d+2];
            } else {
                pSurfDest = m_apSurfStar[iWorkTexture];
            }
            m_pd3dDevice->SetRenderTarget( 0, pSurfDest );

            //텍스처좌표와 합성할때의 가중치를 계산
			D3DXVECTOR4 avSampleWeights[MAX_SAMPLES];
			D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];

            for (int i = 0 ; i < nSamples ; i++) {
                // 각각의 가중치
                float lum = powf( 0.95f, attnPowScale * i );
                avSampleWeights[i] = s_aaColor[s_maxPasses-1-p][i]
                                * lum * (p+1.0f) * 0.5f ;
                                
                // 텍스처좌표를 늘린 양
                avSampleOffsets[i].x = vtStepUV.x * i ;
                avSampleOffsets[i].y = vtStepUV.y * i ;
                if ( 0.9f <= fabs(avSampleOffsets[i].x) ||
                     0.9f <= fabs(avSampleOffsets[i].y) ) {
                    avSampleOffsets[i].x = 0.0f ;
                    avSampleOffsets[i].y = 0.0f ;
                    avSampleWeights[i] *= 0.0f ;
                }
            }
            m_pEffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
            m_pEffect->SetVectorArray("g_avSampleWeights", avSampleWeights, nSamples);
            
            // 전체화면 복사
            m_pEffect->SetTechnique("Star");
            m_pEffect->Begin(NULL, 0);
            m_pEffect->Pass(0);
            m_pd3dDevice->SetTexture( 0, pTexSource );
            DrawFullScreenQuad(0.0f, 0.0f, 1.0f, 1.0f);
			m_pEffect->End();

            // 다음 패스를 위한 파라메터 설정
            vtStepUV *= nSamples;
            attnPowScale *= nSamples;

            // 렌더링한 출력을 다음 텍스처로 한다
            pTexSource = m_apTexStar[iWorkTexture];
			
			iWorkTexture ^= 1;
        }
    }


    // 모든 광선을 합성
    m_pd3dDevice->SetRenderTarget( 0, m_apSurfStar[0] );
  
	m_pEffect->SetTechnique("MergeTextures");
    m_pEffect->Begin(NULL, 0);
    m_pEffect->Pass(0);
	m_pd3dDevice->SetTexture( 0, m_apTexStar[0+2] );
	m_pd3dDevice->SetTexture( 1, m_apTexStar[1+2] );
	m_pd3dDevice->SetTexture( 2, m_apTexStar[2+2] );
	m_pd3dDevice->SetTexture( 3, m_apTexStar[3+2] );
	m_pd3dDevice->SetTexture( 4, m_apTexStar[4+2] );
	m_pd3dDevice->SetTexture( 5, m_apTexStar[5+2] );

	DrawFullScreenQuad(0.0f, 0.0f, 1.0f, 1.0f);

    m_pEffect->End();

    return S_OK;
}
//-------------------------------------------------------------
// Name: Render()
// Desc: 화면 렌더
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{

    PDIRECT3DSURFACE9 pBackBuffer;
    
    // HDR렌더링 타겟 설정
    m_pd3dDevice->GetRenderTarget(0, &pBackBuffer);// 백업
    m_pd3dDevice->SetRenderTarget(0, m_pSurfScene);// 렌더할 곳으로 전환

	//---------------------------------------------------------
	// 렌더
	//---------------------------------------------------------
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		// 렌더링타겟 클리어
		m_pd3dDevice->Clear(0L, NULL
						, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
						, 0x0060c0, 1.0f, 0L);

		// 주전자 등을 화면에 렌더
		this->RenderScene();
		
        this->Scene_To_SceneScaled();     // 축소버퍼에 장면을 복사
        this->SceneScaled_To_BrightPass();// 밝은부분 추출
        this->BrightPass_To_StarSource(); // 광선을 위해 뭉개기
        this->RenderStar();               // 광선 생성

		// 렌더링타겟 복구
        m_pd3dDevice->SetRenderTarget(0, pBackBuffer);
	    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

		// 마지막에 LDR로 다운시켜서 렌더
		float w = (float)this->m_d3dsdBackBuffer.Width ;
		float h = (float)this->m_d3dsdBackBuffer.Height;
		typedef struct{float x,y,z,w,u,v;}TVERTEX;
		TVERTEX VertexFinal[4] = {
			//x  y  z rhw    tu        tv
			{ 0, 0, 0, 1, 0+0.5f/w, 0+0.5f/h,},
			{ w, 0, 0, 1, 1+0.5f/w, 0+0.5f/h,},
			{ w, h, 0, 1, 1+0.5f/w, 1+0.5f/h,},
			{ 0, h, 0, 1, 0+0.5f/w, 1+0.5f/h,},
		};
		m_pd3dDevice->SetFVF(D3DFVF_XYZRHW| D3DFVF_TEX1);
		m_pd3dDevice->SetTexture( 0, m_pTexScene );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
		                  , 2, VertexFinal, sizeof( TVERTEX ) );

		// 광선들 덧셈합성
		m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE) ;
		m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE) ;
        m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) ;
		m_pd3dDevice->SetTexture( 0, m_apTexStar[0] );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
		                  , 2, VertexFinal, sizeof( TVERTEX ) );
        m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE) ;


#if 0	// 디버그용 텍스처 출력
		for(int i=0;i<4;i++){
			TVERTEX VertexFinal2[4] = {
				//x  y  z rhw    tu        tv
				{ 0,     0+h*(FLOAT)i/8, 0, 1, 0+0.5f/w, 0+0.5f/h,},
				{ w/8,   0+h*(FLOAT)i/8, 0, 1, 1+0.5f/w, 0+0.5f/h,},
				{ w/8, h/8+h*(FLOAT)i/8, 0, 1, 1+0.5f/w, 1+0.5f/h,},
				{ 0,   h/8+h*(FLOAT)i/8, 0, 1, 0+0.5f/w, 1+0.5f/h,},
			};
			m_pd3dDevice->SetFVF(D3DFVF_XYZRHW| D3DFVF_TEX1);
			switch(i){
			case 0:m_pd3dDevice->SetTexture( 0, m_pTexSceneScaled );break;
			case 1:m_pd3dDevice->SetTexture( 0, m_pTexBrightPass );break;
			case 2:m_pd3dDevice->SetTexture( 0, m_pTexStarSource );break;
			case 3:m_pd3dDevice->SetTexture( 0, m_apTexStar[0] );break;
			}
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
								, 2, VertexFinal2, sizeof( TVERTEX ) );
		}
#endif

		// 원상태로 복구
	    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
        
		RenderText();				// 도움말 출력

        m_pd3dDevice->EndScene();	// 렌더종료
    }

    // 표면 해제
    SAFE_RELEASE(pBackBuffer);

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
	SAFE_RELEASE(m_pSurfScene);
	SAFE_RELEASE(m_pTexScene);
	SAFE_RELEASE(m_pSurfSceneScaled);
	SAFE_RELEASE(m_pTexSceneScaled);
	SAFE_RELEASE(m_pSurfBrightPass);
	SAFE_RELEASE(m_pTexBrightPass);
	SAFE_RELEASE(m_pSurfStarSource);
	SAFE_RELEASE(m_pTexStarSource);
    for(int i=0; i < NUM_STAR_TEXTURES; i++ ) {
        SAFE_RELEASE(m_apSurfStar[i]);
        SAFE_RELEASE(m_apTexStar[i]);
    }

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
	SAFE_RELEASE( m_pEffect );      // 셰이더
	SAFE_RELEASE( m_pDecl );		// 정점선언
	SAFE_RELEASE( m_pNormalMap );
	
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




