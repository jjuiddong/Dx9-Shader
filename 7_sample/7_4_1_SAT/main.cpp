//-------------------------------------------------------------
// File: main.cpp
//
// Desc: �������� ���̺�
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

typedef struct {
	FLOAT p[3];
	FLOAT tu, tv;
} VERTEX;

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
// Name: UpdateParam()
// Desc: ������ ����
//-------------------------------------------------------------
void CMyD3DApplication::UpdateParam()
{
	float size = 1.0f+(float)(int)m_size;// �Ҽ� ����
	m_pEffect->SetFloat("SIZE", size);
	m_pEffect->SetFloat("INV_SIZE2", 1.0f/(size*size));
}

//-------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: ���ø����̼� ������
//-------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
	m_pMesh						= new CD3DMesh();
	m_pMeshBg					= new CD3DMesh();
	m_size						= 40.0f;

	m_pMapZ						= NULL;
	m_pSatTex					= NULL;
	m_pSatSurf					= NULL;

	m_pEffect					= NULL;
	m_hTechnique  				= NULL;
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
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(1,1) )
		return E_FAIL;

	// �������̴� ������ ���������� ����Ʈ���� ó��
    if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1)
    &&  0==(dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) )
			return E_FAIL;

    return S_OK;
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
	m_htSrcMap  = m_pEffect->GetParameterByName( NULL, "SrcMap" );
	m_pEffect->SetFloat("MAP_WIDTH",  MAP_WIDTH);	// ���̐ݒ�
	m_pEffect->SetFloat("MAP_HEIGHT", MAP_HEIGHT);	// �����̐ݒ�
	
	this->UpdateParam();

	// UFO���б�
	if(FAILED(hr=m_pMesh  ->Create( m_pd3dDevice, _T("ufo.x"))))
        return DXTRACE_ERR( "LoadCar", hr );
	// �����б�
	if(FAILED(hr=m_pMeshBg->Create( m_pd3dDevice, _T("map.x"))))
        return DXTRACE_ERR( "Load BG", hr );
        
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
	// ������Ÿ�� ����
	if (FAILED(m_pd3dDevice->CreateDepthStencilSurface(
				MAP_WIDTH, MAP_HEIGHT, D3DFMT_D16,
				D3DMULTISAMPLE_NONE, 0, TRUE, &m_pMapZ, NULL)))
		return E_FAIL;
	// �������� ���̺�
	if (FAILED(m_pd3dDevice->CreateTexture(MAP_WIDTH, MAP_HEIGHT, 1,
				D3DUSAGE_RENDERTARGET, D3DFMT_A32B32G32R32F,
				D3DPOOL_DEFAULT, &m_pSatTex, NULL)))
		return E_FAIL;
	if (FAILED(m_pSatTex->GetSurfaceLevel(0, &m_pSatSurf)))
		return E_FAIL;
	
	// ����Ʈ
	m_pEffect->OnResetDevice();

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

	//---------------------------------------------------------
	// ������ �Ķ���� ����
	//---------------------------------------------------------
    if( m_UserInput.bSizeUp && !m_UserInput.bSizeDown )
        m_size += 10.0f*m_fElapsedTime;
    else if( m_UserInput.bSizeDown && !m_UserInput.bSizeUp )
        m_size -= 10.0f*m_fElapsedTime;
	this->UpdateParam();

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
	pUserInput->bSizeUp      = ( m_bActive && (GetAsyncKeyState( 'Q'     )  & 0x8000) == 0x8000 );
    pUserInput->bSizeDown    = ( m_bActive && (GetAsyncKeyState( 'W'      ) & 0x8000) == 0x8000 );
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
	DWORD i;

	//-----------------------------------------------------
	// ������Ÿ�� ����
	//-----------------------------------------------------
	m_pd3dDevice->GetRenderTarget(0, &pOldBackBuffer);
	m_pd3dDevice->GetDepthStencilSurface(&pOldZBuffer);
	m_pd3dDevice->GetViewport(&oldViewport);

	//---------------------------------------------------------
	// ����
	//---------------------------------------------------------
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		//-----------------------------------------------------
		// ������Ÿ�� ����
		//-----------------------------------------------------
		m_pd3dDevice->SetRenderTarget(0, m_pSatSurf);
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
						, 0, 1.0f, 0L);

		//-----------------------------------------------------
		// 1�н�:������Ÿ�� �����
		//-----------------------------------------------------
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

        m_pd3dDevice->EndScene();

		// ----------------------------------------------------
		// SAT ���
		// ----------------------------------------------------
		RS( D3DRS_ZENABLE, FALSE );
		RS( D3DRS_LIGHTING, FALSE );

		TSS(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		TSS(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		TSS(1,D3DTSS_COLOROP,	D3DTOP_ADD);
		TSS(1,D3DTSS_COLORARG1,	D3DTA_CURRENT);
		TSS(1,D3DTSS_COLORARG2,	D3DTA_TEXTURE);
		TSS(2,D3DTSS_COLOROP,   D3DTOP_DISABLE);

		m_pEffect->SetTechnique( m_hTechnique );
		m_pEffect->Begin( NULL, 0 );
		m_pd3dDevice->SetFVF( D3DFVF_XYZ | D3DFVF_TEX1 );
		m_pEffect->SetTexture(m_htSrcMap, m_pSatTex );
		
		// ���ι��� �հ�
		m_pEffect->Pass( 0 );
		for(i=0;i<MAP_WIDTH;i++){
			FLOAT dx = (1.0f/MAP_WIDTH);
			VERTEX Vertex[4] = {
				//       x                y     z        tu       tv
				{{ -1+2.0f*dx*(FLOAT)i, +1.0f, 0.1f}, dx*(FLOAT)i, 0,},
				{{ -1+2.0f*dx*(FLOAT)i, -1.0f, 0.1f}, dx*(FLOAT)i, 1,},
			};
			m_pd3dDevice->BeginScene();
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, Vertex, sizeof( VERTEX ) );
	        m_pd3dDevice->EndScene();
		}
		// ���ι��� �հ�
		m_pEffect->Pass( 1 );
		for(i=0;i<MAP_HEIGHT;i++){
			FLOAT dy = (1.0f/MAP_WIDTH);
			VERTEX Vertex[4] = {
				//   x            y              z     tu    tv
				{{ -1.0f,  +1-2.0f*dy*(FLOAT)i, 0.1f}, 0, dy*(FLOAT)i },
				{{ +1.0f,  +1-2.0f*dy*(FLOAT)i, 0.1f}, 1, dy*(FLOAT)i },
			};
			m_pd3dDevice->BeginScene();
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 1, Vertex, sizeof( VERTEX ) );
	        m_pd3dDevice->EndScene();
		}
		m_pEffect->End();

		//-----------------------------------------------------
		// ������Ÿ�� ����
		//-----------------------------------------------------
		m_pd3dDevice->SetRenderTarget(0, pOldBackBuffer);
		m_pd3dDevice->SetDepthStencilSurface(pOldZBuffer);
		m_pd3dDevice->SetViewport(&oldViewport);
	}
	
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		// ���� Ŭ����
		m_pd3dDevice->Clear( 0L, NULL
						, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER
						, 0x00404080, 1.0f, 0L );

		if( m_pEffect != NULL ) 
		{
			//-------------------------------------------------
			// �ؽ�ó�� ������ ���δ�
			//-------------------------------------------------
			m_pEffect->SetTechnique( m_hTechnique );
			m_pEffect->Begin( NULL, 0 );
			m_pEffect->Pass( 2 );

			TSS(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
			TSS(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
			TSS(1,D3DTSS_COLOROP,    D3DTOP_DISABLE);

			VERTEX Vertex[4] = {
				//   x      y     z      tu tv
				{{  1.0f, -1.0f, 0.1f},   1, 1,},
				{{ -1.0f, -1.0f, 0.1f},   0, 1,},
				{{ -1.0f,  1.0f, 0.1f},   0, 0,},
				{{  1.0f,  1.0f, 0.1f},   1, 0,},
			};
			m_pd3dDevice->SetFVF( D3DFVF_XYZ | D3DFVF_TEX1 );
			m_pEffect->SetTexture(m_htSrcMap, m_pSatTex );
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
							, 2, Vertex, sizeof( VERTEX ) );

			m_pEffect->End();
		}

		RS( D3DRS_ZENABLE, TRUE );
		RS( D3DRS_LIGHTING, TRUE );

#if 1 // ����׿� �ؽ�ó ���
		{
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,    D3DTOP_DISABLE);
		m_pd3dDevice->SetVertexShader(NULL);
		m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		m_pd3dDevice->SetPixelShader(0);
		float scale = 128.0f;
		typedef struct {FLOAT p[4]; FLOAT tu, tv;} TVERTEX;
		for(DWORD i=0; i<1; i++){
			TVERTEX Vertex[4] = {
				// x  y  z rhw tu tv
				{    0,(i+0)*scale,0, 1, 0, 0,},
				{scale,(i+0)*scale,0, 1, 1, 0,},
				{scale,(i+1)*scale,0, 1, 1, 1,},
				{    0,(i+1)*scale,0, 1, 0, 1,},
			};
			if(0==i) m_pd3dDevice->SetTexture( 0, m_pSatTex );
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof( TVERTEX ) );
		}
		}

#endif		
		RenderText();				// ���� ���

        m_pd3dDevice->EndScene();	// ��������
    }

	pOldBackBuffer->Release();
	pOldZBuffer->Release();
	
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
    sprintf( szMsg, TEXT("'Q' or 'w' keys change the bluring size, now %f"), m_size );
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
	// SAT
	SAFE_RELEASE(m_pSatSurf);
	SAFE_RELEASE(m_pSatTex);
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




