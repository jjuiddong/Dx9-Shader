//-------------------------------------------------------------
// File: main.cpp
//
// Desc: 범프맵
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

	m_dwCreationWidth           = 375;
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
    

	// 픽셀셰이더 버전확인
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) )
		return E_FAIL;

    // 정점셰이더 버전확인
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
    pHeightTexture->GetLevelDesc(0,&desc);// 텍스처 정보 얻기
    D3DXCreateTexture(m_pd3dDevice, desc.Width, desc.Height, 0, 0, 
        D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_pNormalMap);// 텍스처 생성
    D3DXComputeNormalMap(m_pNormalMap,	// 법선맵 생성
        pHeightTexture, NULL, 0, D3DX_CHANNEL_RED, 1.0f);
    SAFE_RELEASE( pHeightTexture );		// 필요없어진 리소스 해제

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
        // 셰이더 읽기 실패
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
	// 이펙트
	if(m_pEffect) m_pEffect->OnResetDevice();

	// 메시
	m_pMeshBg->RestoreDeviceObjects( m_pd3dDevice );

	//---------------------------------------------------------
	// FVF로 처리하지 않은 정점선언은 직접 처리
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
    
    SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    SAMP( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

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
	// 입력데이터 갱신
    UpdateInput( &m_UserInput );

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
	// 줌(zoom)
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
// Name: Render()
// Desc: 화면 렌더
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    D3DXMATRIX m, mT, mR, mView, mProj;
	D3DXMATRIX mWorld;
	D3DXVECTOR4 v, light_pos, eye_pos;
	DWORD i;

	//---------------------------------------------------------
	// 렌더
	//---------------------------------------------------------
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		// 렌더링 타겟 클리어
		m_pd3dDevice->Clear(0L, NULL
						, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
						, 0x0060c0, 1.0f, 0L);

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

			// 로컬-투영 변환행렬
			m = mWorld * m_mView * m_mProj;
			m_pEffect->SetMatrix( m_hmWVP, &m );

			// 광원의 방향(로컬좌표계)
			light_pos = D3DXVECTOR4( -0.577f, -0.577f, -0.577f,0);
			D3DXMatrixInverse( &m, NULL, &mWorld);
			D3DXVec4Transform( &v, &light_pos, &m );
			D3DXVec3Normalize( (D3DXVECTOR3 *)&v, (D3DXVECTOR3 *)&v );
			v.w = -0.7f;		// 환경광 강도
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

         // 도움말 출력
        RenderText();

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

    FLOAT fNextLine = 40.0f; // 출력높이

    // 화면상태
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
	m_pMesh->InvalidateDeviceObjects(); // 메시
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
	m_pMesh->Destroy();
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




