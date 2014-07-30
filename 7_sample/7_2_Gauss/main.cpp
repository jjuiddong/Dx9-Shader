//-------------------------------------------------------------
// File: main.cpp
//
// Desc: Gaussian ����
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


// �����ũ��
#define RS   m_pd3dDevice->SetRenderState
#define TSS  m_pd3dDevice->SetTextureStageState
#define SAMP m_pd3dDevice->SetSamplerState


//-------------------------------------------------------------
// ��������ü
//-------------------------------------------------------------
typedef struct {
    FLOAT       p[4];
    FLOAT       tu, tv;
} TVERTEX;

//-------------------------------------------------------------
// ��������
//-------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;


//-------------------------------------------------------------
// Name: WinMain()
// Desc: �����Լ�
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
// Desc: ���ø����̼� ������
//-------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
	m_dispersion_sq				= 5.0f;

	m_pMesh						= new CD3DMesh();
	m_pMeshBg					= new CD3DMesh();

	m_pMapZ						= NULL;
	m_pOriginalMap				= NULL;
	m_pOriginalMapSurf			= NULL;
	m_pXMap						= NULL;
	m_pXMapSurf					= NULL;
	m_pXYMap					= NULL;
	m_pXYMapSurf				= NULL;

	m_pEffect					= NULL;
	m_hTechnique  				= NULL;
	m_hafWeight					= NULL;
	m_htSrcMap					= NULL;

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
// Desc: �Ҹ���
//-------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
}




//-------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: �� �ѹ��� �ʱ�ȭ
//       ������ �ʱ�ȭ�� IDirect3D9�ʱ�ȭ�� ������
//       �׷��� LPDIRECT3DDEVICE9�ʱ�ȭ�� ������ ���� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    // �ε� �޽��� ���
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    m_bLoadingApp = FALSE;

    return S_OK;
}




//-------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: �ʱ�ȭ�� ȣ���. �ʿ��� �ɷ�(caps)üũ
//-------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps,
                     DWORD dwBehavior,    D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    

	// �ȼ����̴� ����üũ
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) )
		return E_FAIL;

	// �������̴� ������ ���������� ����Ʈ���� ó��
    if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1)
    &&  0==(dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) )
			return E_FAIL;

    return S_OK;
}




//-------------------------------------------------------------
// Name: UpdateWeight()
// Desc: ����ġ ���
//-------------------------------------------------------------
VOID CMyD3DApplication::UpdateWeight( FLOAT dispersion )
{
    DWORD i;

    FLOAT total=0;
    for( i=0; i<WEIGHT_MUN; i++ ){
        FLOAT pos = 1.0f+2.0f*(FLOAT)i;
        m_tbl[i] = expf(-0.5f*(FLOAT)(pos*pos)/dispersion);
        total += 2.0f*m_tbl[i];
    }
    // ����ȭ
    for( i=0; i<WEIGHT_MUN; i++ ) m_tbl[i] /= total;

    if(m_pEffect) m_pEffect->SetFloatArray(m_hafWeight
                                        , m_tbl, WEIGHT_MUN);

}




//-------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: ����̽��� ���������� �ʱ�ȭ
//       �����ӹ��� ���˰� ����̽� ������ ���ѵڿ� ȣ��
//       ���⼭ Ȯ���� �޸𸮴� DeleteDeviceObjects()���� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    HRESULT hr;
	D3DXVECTOR4 offset;

	// ���б�
	if(FAILED(hr=m_pMesh  ->Create( m_pd3dDevice, _T("ufo.x"))))
        return DXTRACE_ERR( "LoadCar", hr );
	// �����б�
	if(FAILED(hr=m_pMeshBg->Create( m_pd3dDevice, _T("map.x"))))
        return DXTRACE_ERR( "Load BG", hr );
        
	// ���̴��б�
	LPD3DXBUFFER pErr;
    if( FAILED( hr = D3DXCreateEffectFromFile(
				m_pd3dDevice, "hlsl.fx", NULL, NULL, 
				D3DXSHADER_DEBUG , NULL, &m_pEffect, &pErr ))){
		MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer()
					, "ERROR", MB_OK);
		return DXTRACE_ERR( "CreateEffectFromFile", hr );
	}
	m_hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
	m_hafWeight = m_pEffect->GetParameterByName( NULL, "weight" );
	m_htSrcMap  = m_pEffect->GetParameterByName( NULL, "SrcMap" );
	m_pEffect->SetFloat("MAP_WIDTH",  MAP_WIDTH);	// ��   ����
	m_pEffect->SetFloat("MAP_HEIGHT", MAP_HEIGHT);	// ���� ����
	
	// ���̴����� ����� 2�ؼ��� �����ϴ� ���
	offset.x = 16.0f/MAP_WIDTH;  offset.y = 0.0f/MAP_HEIGHT;
	m_pEffect->SetVector("offsetX",  &offset);
	offset.x = 0.0f/MAP_WIDTH;	offset.y = 16.0f/MAP_HEIGHT;
	m_pEffect->SetVector("offsetY",  &offset);

	// ����ġ ����
	this->UpdateWeight(m_dispersion_sq*m_dispersion_sq);

	// ��Ʈ
    m_pFont->InitDeviceObjects( m_pd3dDevice );

    return S_OK;
}

//-------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: ȭ��ũ�Ⱑ �������� ȣ���
//       Ȯ���� �޸𸮴� InvalidateDeviceObjects()���� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	// �޽�
	m_pMesh  ->RestoreDeviceObjects( m_pd3dDevice );
	m_pMeshBg->RestoreDeviceObjects( m_pd3dDevice );

    // ��������
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );


    // ������ ���¼���
    RS( D3DRS_DITHERENABLE,   FALSE );
    RS( D3DRS_SPECULARENABLE, FALSE );
    RS( D3DRS_ZENABLE,        TRUE );
    RS( D3DRS_AMBIENT,        0x000F0F0F );
    
    TSS( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    TSS( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    TSS( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    TSS( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
    TSS( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );
    SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
    SAMP( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );

    // �������
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &m_mWorld );

	// �����
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFromPt, &vLookatPt, &vUpVec );

    // �������
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );

    // ��Ʈ
    m_pFont->RestoreDeviceObjects();

	// ������Ÿ�� ����
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
	// ����
	if (FAILED(m_pd3dDevice->CreateTexture(
				MAP_WIDTH, MAP_HEIGHT, 1, D3DUSAGE_RENDERTARGET,
				D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pXMap, NULL)))
		return E_FAIL;
	if (FAILED(m_pXMap->GetSurfaceLevel(0, &m_pXMapSurf)))
		return E_FAIL;
	// ������ ���� ��
	if (FAILED(m_pd3dDevice->CreateTexture(
				MAP_WIDTH, MAP_HEIGHT, 1, D3DUSAGE_RENDERTARGET,
				D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pXYMap, NULL)))
		return E_FAIL;
	if (FAILED(m_pXYMap->GetSurfaceLevel(0, &m_pXYMapSurf)))
		return E_FAIL;

	m_pEffect->OnResetDevice();

	return S_OK;
}




//-------------------------------------------------------------
// Name: FrameMove()
// Desc: �� �����Ӹ��� ȣ���. �ִϸ��̼� ó���� ���
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	UpdateInput( &m_UserInput ); // �Էµ����� ����

	//---------------------------------------------------------
	// �Է¿� ���� ��ǥ�踦 �����Ѵ�
	//---------------------------------------------------------
	// ȸ��
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
	// ����� ����
	//---------------------------------------------------------
	// ��
    if( m_UserInput.bZoomIn && !m_UserInput.bZoomOut )
        m_fViewZoom += m_fElapsedTime;
    else if( m_UserInput.bZoomOut && !m_UserInput.bZoomIn )
        m_fViewZoom -= m_fElapsedTime;

    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -m_fViewZoom );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFromPt, &vLookatPt, &vUpVec );


	if( m_UserInput.bDispersionUp && !m_UserInput.bDispersionDown ){
        m_dispersion_sq += m_fElapsedTime;
		this->UpdateWeight(m_dispersion_sq*m_dispersion_sq);
	} else
	if( m_UserInput.bDispersionDown && !m_UserInput.bDispersionUp ){
        m_dispersion_sq -= m_fElapsedTime;
		this->UpdateWeight(m_dispersion_sq*m_dispersion_sq);
	}


	return S_OK;
}
//-------------------------------------------------------------
// Name: UpdateInput()
// Desc: �Էµ����� ����
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
// Desc: ȭ�� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    D3DXMATRIX m, mT, mR, mView, mProj;
	LPDIRECT3DSURFACE9 pOldBackBuffer, pOldZBuffer;
	D3DVIEWPORT9 oldViewport;

	//---------------------------------------------------------
	// ����
	//---------------------------------------------------------
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		//-------------------------------------------------
		// ������Ÿ�� ����
		//-------------------------------------------------
		m_pd3dDevice->GetRenderTarget(0, &pOldBackBuffer);
		m_pd3dDevice->GetDepthStencilSurface(&pOldZBuffer);
		m_pd3dDevice->GetViewport(&oldViewport);

		if( m_pEffect != NULL ) 
		{
			//-------------------------------------------------
			// ������Ÿ�� ����
			//-------------------------------------------------
			m_pd3dDevice->SetRenderTarget(0, m_pOriginalMapSurf);
			m_pd3dDevice->SetDepthStencilSurface(m_pMapZ);
			// ����Ʈ����
			D3DVIEWPORT9 viewport = {0,0      // �������
							, MAP_WIDTH  // ��
							, MAP_HEIGHT // ����
							, 0.0f,1.0f};     // ����,�ĸ�
			m_pd3dDevice->SetViewport(&viewport);
	
			// ������Ÿ�� Ŭ����
			m_pd3dDevice->Clear(0L, NULL
							, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
							, 0xffffffff, 1.0f, 0L);

			//-------------------------------------------------
			// 1�н�:������Ÿ�� �����
			//-------------------------------------------------
			m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m_mWorld);
			m_pd3dDevice->SetTransform( D3DTS_VIEW,  &m_mView );
			m_pd3dDevice->SetTransform( D3DTS_PROJECTION,  &m_mProj );

			TSS( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
			TSS( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
			m_pMeshBg->Render( m_pd3dDevice );
			
			// ����� ����
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

			//-------------------------------------------------
			// ���̴� ����
			//-------------------------------------------------
			m_pEffect->SetTechnique( m_hTechnique );
			m_pEffect->Begin( NULL, 0 );

			//-------------------------------------------------
			// 2�н�:������ X
			//-------------------------------------------------
			m_pd3dDevice->SetRenderTarget(0, m_pXMapSurf);
			m_pEffect->Pass( 0 );

			RS( D3DRS_ZENABLE, FALSE );
			RS( D3DRS_LIGHTING, FALSE );
			TSS(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
			TSS(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
			TSS(1,D3DTSS_COLOROP,    D3DTOP_DISABLE);

			typedef struct {FLOAT p[3];FLOAT tu, tv;} VERTEX;

			VERTEX Vertex1[4] = {
				//   x      y     z      tu tv
				{{  1.0f, -1.0f, 0.1f},   1, 1,},
				{{ -1.0f, -1.0f, 0.1f},   0, 1,},
				{{ -1.0f,  1.0f, 0.1f},   0, 0,},
				{{  1.0f,  1.0f, 0.1f},   1, 0,},
			};
			m_pd3dDevice->SetFVF( D3DFVF_XYZ | D3DFVF_TEX1 );
			m_pEffect->SetTexture(m_htSrcMap, m_pOriginalMap);
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
							, 2, Vertex1, sizeof( VERTEX ) );

			//-------------------------------------------------
			// 2�н�:������ Y
			//-------------------------------------------------
			m_pd3dDevice->SetRenderTarget(0, m_pXYMapSurf);
			m_pEffect->Pass( 1 );

			m_pEffect->SetTexture(m_htSrcMap, m_pXMap);
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
							, 2, Vertex1, sizeof( VERTEX ) );

			m_pEffect->End();
		}

		//-----------------------------------------------------
		// ������Ÿ�� ����
		//-----------------------------------------------------
		m_pd3dDevice->SetRenderTarget(0, pOldBackBuffer);
		m_pd3dDevice->SetDepthStencilSurface(pOldZBuffer);
		m_pd3dDevice->SetViewport(&oldViewport);
		pOldBackBuffer->Release();
		pOldZBuffer->Release();

		// ���� Ŭ����
		m_pd3dDevice->Clear( 0L, NULL
						, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER
						, 0x00404080, 1.0f, 0L );

		//-----------------------------------------------------
		// �״�� ���δ�
		//-----------------------------------------------------
		FLOAT w = (FLOAT)oldViewport.Width;
		FLOAT h = (FLOAT)oldViewport.Height;
		TVERTEX Vertex1[4] = {
			//x  y   z    rhw  tu tv
			{ 0, 0, 0.1f, 1.0f, 0+0.5f/MAP_WIDTH, 0+0.5f/MAP_HEIGHT,},
			{ w, 0, 0.1f, 1.0f, 1+0.5f/MAP_WIDTH, 0+0.5f/MAP_HEIGHT,},
			{ w, h, 0.1f, 1.0f, 1+0.5f/MAP_WIDTH, 1+0.5f/MAP_HEIGHT,},
			{ 0, h, 0.1f, 1.0f, 0+0.5f/MAP_WIDTH, 1+0.5f/MAP_HEIGHT,},
		};
		m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		m_pd3dDevice->SetTexture(0, m_pOriginalMap);
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
							, 2, Vertex1, sizeof( TVERTEX ) );

		//-----------------------------------------------------
		// ���� ���� ���δ�
		//-----------------------------------------------------

		TVERTEX Vertex2[4] = {
			//   x    y   z    rhw    tu    tv
			{ 0.0f,   0, 0.1f, 1.0f, 0.0f, 0.0f,},
			{ 0.5f*w, 0, 0.1f, 1.0f, 0.5f, 0.0f,},
			{ 0.5f*w, h, 0.1f, 1.0f, 0.5f, 1.0f,},
			{ 0.0f,   h, 0.1f, 1.0f, 0.0f, 1.0f,},
		};
		m_pd3dDevice->SetTexture(0, m_pXYMap);
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
							, 2, Vertex2, sizeof( TVERTEX ) );

		RS( D3DRS_ZENABLE, TRUE );
		RS( D3DRS_LIGHTING, TRUE );

		RenderText();				// ���� ���

#if 1 // ����׿� �ؽ�ó ���
		{
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,    D3DTOP_DISABLE);
		m_pd3dDevice->SetVertexShader(NULL);
		m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		m_pd3dDevice->SetPixelShader(0);
		float scale = 128.0f;
		for(DWORD i=0; i<3; i++){
			TVERTEX Vertex[4] = {
				// x  y  z rhw tu tv
				{    0,(i+0)*scale,0, 1, 0, 0,},
				{scale,(i+0)*scale,0, 1, 1, 0,},
				{scale,(i+1)*scale,0, 1, 1, 1,},
				{    0,(i+1)*scale,0, 1, 0, 1,},
			};
			if(0==i) m_pd3dDevice->SetTexture( 0, m_pOriginalMap );
			if(1==i) m_pd3dDevice->SetTexture( 0, m_pXMap );
			if(2==i) m_pd3dDevice->SetTexture( 0, m_pXYMap );
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof( TVERTEX ) );
		}
		}
#endif		

        m_pd3dDevice->EndScene();	// ��������
    }

    return S_OK;
}




//-------------------------------------------------------------
// Name: RenderText()
// Desc: ���¿� ���� ���
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    FLOAT fNextLine = 40.0f; // ��³���

    // ���۹��
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
	sprintf( szMsg, TEXT("Use Page Up/Down keys to change dispersion (Now %f^2)"), m_dispersion_sq );
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
// Desc: WndProc �������̵�
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
                // �ε���
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
// Desc: RestoreDeviceObjects() ���� ������ ������Ʈ ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	// ������ Ÿ��
	SAFE_RELEASE(m_pXYMapSurf);
	SAFE_RELEASE(m_pXYMap);
	SAFE_RELEASE(m_pXMapSurf);
	SAFE_RELEASE(m_pXMap);
	SAFE_RELEASE(m_pOriginalMapSurf);
	SAFE_RELEASE(m_pOriginalMap);
	SAFE_RELEASE(m_pMapZ);

	m_pMesh  ->InvalidateDeviceObjects(); // �޽�
	m_pMeshBg->InvalidateDeviceObjects();

    m_pFont->InvalidateDeviceObjects();	// ��Ʈ

	// ���̴�
    if( m_pEffect != NULL ) m_pEffect->OnLostDevice();

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() ���� ������ ������Ʈ ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    // ���̴�
	SAFE_RELEASE( m_pEffect );
	
	// �޽�
	m_pMesh  ->Destroy();
	m_pMeshBg->Destroy();

    // ��Ʈ
    m_pFont->DeleteDeviceObjects();

    return S_OK;
}




//-------------------------------------------------------------
// Name: FinalCleanup()
// Desc: ���������� ȣ���
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    SAFE_DELETE( m_pMeshBg ); // �޽�
	SAFE_DELETE( m_pMesh );

    SAFE_DELETE( m_pFont );	// ��Ʈ

    return S_OK;
}




