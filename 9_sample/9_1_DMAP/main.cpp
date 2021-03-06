//-------------------------------------------------------------
// File: main.cpp
//
// Desc: 변위맵핑
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
// 정점선언
//-------------------------------------------------------------
D3DVERTEXELEMENT9 decl[] =
{
	{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
	{0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_LOOKUP,  D3DDECLUSAGE_SAMPLE,	0},
	D3DDECL_END()
};


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
	m_bDMap						= TRUE;
	m_degree					= 1.f;
	m_pMesh						= new CD3DMesh();
	m_pDispMap					= NULL;
	m_pDecl						= NULL;
	m_pEffect					= NULL;

    m_fWorldRotX                = 0;
	m_fWorldRotY                = -1;

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

	if( pCaps->PixelShaderVersion  < D3DPS_VERSION(1,1) )
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
    D3DFORMAT adptFormat = m_d3dSettings.DisplayMode().Format;

	// 변위맵핑이 가능한 조건들을 확인
    m_bDMap = 
		// 하드웨어 정점처리
		(m_pd3dDevice->GetSoftwareVertexProcessing()==FALSE) && 
		// 변위맵을 지원하는가 체크
        (m_d3dCaps.DevCaps2 & D3DDEVCAPS2_DMAPNPATCH ) &&
		// 텍스처포맷 확인
        SUCCEEDED( m_pD3D->CheckDeviceFormat(
								m_d3dCaps.AdapterOrdinal,
                                m_d3dCaps.DeviceType,
                                adptFormat,
                                D3DUSAGE_DMAP,
                                D3DRTYPE_TEXTURE,
                                D3DFMT_L8 ) );

    // 메시 읽기
	if( FAILED( hr=m_pMesh->Create( m_pd3dDevice, "earth.x" )))
        return DXTRACE_ERR( "Load Mesh", hr );

	
	// 셰이더 읽기
    if( FAILED( hr = D3DXCreateEffectFromFile(
						m_pd3dDevice, "hlsl.fx", NULL, NULL, 
						0, NULL, &m_pEffect, NULL ) ) )
		return DXTRACE_ERR( "CreateEffectFromFile", hr );

	if(m_bDMap){
		// 정점선언 오브젝트 생성
		if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration(
											decl, &m_pDecl )))
			return DXTRACE_ERR ("CreateVertexDeclaration", hr);
		
		// 변위맵 읽기
		if(FAILED(hr = D3DXCreateTextureFromFileEx(
								  m_pd3dDevice		// 디바이스
								, "earthbump.bmp"	// 파일명
								, D3DX_DEFAULT		// 폭
								, D3DX_DEFAULT		// 높이
								, D3DX_DEFAULT		// 밉 레벨
								, D3DUSAGE_DMAP		// 사용법
								, D3DFMT_L8			// 포맷
								, D3DPOOL_MANAGED	// 메모리클래스
								, D3DX_DEFAULT		// 필터링
								, D3DX_DEFAULT		// 필터링
								, 0					// 투명값
								, NULL				// 소스이미지
								, NULL				// 256색 팔레트
								, &m_pDispMap
								)))
			return DXTRACE_ERR( "Load Texture", hr );

		// 샘플러 스테이트 설정
		m_pd3dDevice->SetSamplerState( D3DDMAPSAMPLER,
						D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
		m_pd3dDevice->SetSamplerState( D3DDMAPSAMPLER,
						D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );
		m_pd3dDevice->SetSamplerState( D3DDMAPSAMPLER,
						D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		m_pd3dDevice->SetSamplerState( D3DDMAPSAMPLER,
						D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		m_pd3dDevice->SetSamplerState( D3DDMAPSAMPLER,
						D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	}

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
	// 단축매크로
	#define RS   m_pd3dDevice->SetRenderState
	#define TSS  m_pd3dDevice->SetTextureStageState
	#define SAMP m_pd3dDevice->SetSamplerState

    // 렌더링 상태설정
    RS  ( D3DRS_ZENABLE,        TRUE );
    RS  ( D3DRS_LIGHTING,       FALSE );

    // 텍스처 상태설정
    TSS ( 0, D3DTSS_COLOROP,    D3DTOP_MODULATE );
    TSS ( 0, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
    TSS ( 0, D3DTSS_COLORARG2,  D3DTA_DIFFUSE );
    TSS ( 0, D3DTSS_ALPHAOP,    D3DTOP_MODULATE );
    TSS ( 0, D3DTSS_ALPHAARG1,  D3DTA_TEXTURE );
    TSS ( 0, D3DTSS_ALPHAARG2,  D3DTA_DIFFUSE );
	SAMP( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
	SAMP( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );
    SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );


    // 월드행렬
    D3DXMatrixIdentity( &m_mWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m_mWorld );

	// 뷰행렬
    D3DXVECTOR3 vFrom   = D3DXVECTOR3( 0.0f, 0.0f, -3.5f );
    D3DXVECTOR3 vLookat = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFrom, &vLookat, &vUp );

    // 투영행렬
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width )
				  / ((FLOAT)m_d3dsdBackBuffer.Height);
    D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect
								, 0.1f, 10.0f );

	// 광원위치 설정
	m_LighPos = D3DXVECTOR4(-0.6f, 0.6f, -0.6f, 0.3f);

	
	if(m_bDMap){
		//-----------------------------------------------------
		// 변위맵을 사용할때는 직접처리
		//-----------------------------------------------------
		if( m_pMesh->m_pSysMemMesh ){
			if( FAILED( m_pMesh->m_pSysMemMesh->CloneMesh(
						0L|D3DXMESH_NPATCHES, decl,
						m_pd3dDevice,
						&m_pMesh->m_pLocalMesh )))
				return E_FAIL;
			D3DXComputeNormals( m_pMesh->m_pLocalMesh, NULL );
		}
	}else{
		m_pMesh->RestoreDeviceObjects(m_pd3dDevice);
	}
	if( m_pEffect!=NULL ) m_pEffect->OnResetDevice();// 셰이더

    m_pFont->RestoreDeviceObjects();	// 폰트

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
    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;
	// 줌
	if(m_UserInput.bZ && !m_UserInput.bX)
		m_degree += 0.01f;
	else if(m_UserInput.bX && !m_UserInput.bZ)
		m_degree -= 0.01f;

	// 행렬갱신
 	D3DXMATRIX matRotX, matRotY;
    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );
    D3DXMatrixMultiply( &m_mWorld, &matRotY, &matRotX );

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
    D3DXHANDLE hTechnique;
	D3DXMATRIX m;
	D3DXVECTOR4 v;

	// 화면 클리어
    m_pd3dDevice->Clear( 0L, NULL,
						D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						0x000000ff, 1.0f, 0L );

    if( SUCCEEDED( m_pd3dDevice->BeginScene()))	// 렌더 시작
    {
		if(m_bDMap && m_pEffect != NULL)
		{	// 변위맵핑 한다
			//-------------------------------------------------
			// 셰이더 설정
			//-------------------------------------------------
			hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
			m_pEffect->SetTechnique( hTechnique );
			m_pEffect->Begin( NULL, 0 );
			m_pEffect->Pass( 0 );
			
			//-------------------------------------------------
			// 셰이더 상수 설정
			//-------------------------------------------------
			// 좌표변환
			m = m_mWorld * m_mView * m_mProj;
		    D3DXMatrixTranspose( &m, &m );
			m_pd3dDevice->SetVertexShaderConstantF( 0,(float*)&m, 4);
			// 변위의 크기
			v = D3DXVECTOR4(m_degree,0.0f,0.0f,0.0f);
			m_pd3dDevice->SetVertexShaderConstantF( 4,(float*)&v, 1);
			// 적당한 상수
			v = D3DXVECTOR4(0.0f,0.5f,1.0f,2.0f);
			m_pd3dDevice->SetVertexShaderConstantF(12,(float*)&v, 1);
			// 광원
			D3DXMatrixInverse( &m, NULL, &m_mWorld);
			D3DXVec4Transform( &v, &m_LighPos, &m );
			D3DXVec4Normalize( &v, &v );v.w = 0.3f;
			m_pd3dDevice->SetVertexShaderConstantF(13,(float*)&v,1 );


			//-------------------------------------------------
			// 변위맵 설정
			//-------------------------------------------------
			m_pd3dDevice->SetTexture(D3DDMAPSAMPLER, m_pDispMap);
			m_pd3dDevice->SetNPatchMode(4);// N긬긞�`궻긾�[긤
			m_pd3dDevice->SetVertexDeclaration( m_pDecl );

			//-------------------------------------------------
			// 렌더
			//-------------------------------------------------
			m_pMesh->Render( m_pd3dDevice );

			//-------------------------------------------------
			// 렌더종료
			//-------------------------------------------------
			m_pd3dDevice->SetNPatchMode(0);
			m_pd3dDevice->SetVertexShader( NULL );
			m_pEffect->End();
		}else{
			// 변위맵핑 하지 않음
		    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_mWorld );
			m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_mView );
			m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );

			m_pd3dDevice->SetTexture(0,m_pMesh->m_pTextures[0]);
			m_pMesh->Render( m_pd3dDevice );
		}

		RenderText();				// 도움말 출력

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

    // 디스플레이 상태 출력
    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    // 조작법 & 인수 출력
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    sprintf( szMsg, "Displacement: %f", m_degree );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Use arrow keys to rotate object") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Press 'z' or 'x' to change degree of the displacement") );
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
                // 로딩중
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
	m_pMesh->InvalidateDeviceObjects();				// 메시
    if(m_pEffect!=NULL) m_pEffect->OnLostDevice();	// 셰이더

    m_pFont->InvalidateDeviceObjects();	// 폰트

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() 에서 생성한 오브젝트 해제
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	m_pMesh->Destroy();				// 메시
	SAFE_RELEASE( m_pEffect );		// 셰이더
	SAFE_RELEASE( m_pDecl );		// 정점선언
	SAFE_RELEASE( m_pDispMap );		// 변위맵

    m_pFont->DeleteDeviceObjects();	// 폰트

	return S_OK;
}




//-------------------------------------------------------------
// Name: FinalCleanup()
// Desc: 종료직전에 호출됨
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    SAFE_DELETE( m_pMesh );	// 메시

    SAFE_DELETE( m_pFont );	// 폰트

    return S_OK;
}




