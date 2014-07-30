//-------------------------------------------------------------
// File: main.cpp
//
// Desc: ������Ʈ ������ �����׸���
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

static const UINT MAP_SIZE = 256;

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
	m_pShadowTex	 = NULL;
	m_pShadowTexSurf = NULL;
	m_pShadowTexZ	 = NULL;

	m_pMesh						= new CD3DMesh();
	m_pTex						= NULL;

	m_pos						= D3DXVECTOR3(-0.5f,1,0);
	m_light						= D3DXVECTOR4(1,2, 2,1);

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
// Desc: ����̽��� ���������� �ʱ�ȭ
//       �����ӹ��� ���˰� ����̽� ������ ���ѵڿ� ȣ��
//       ���⼭ Ȯ���� �޸𸮴� DeleteDeviceObjects()���� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    HRESULT hr;

	// UFO��
	if( FAILED( hr = m_pMesh->Create( m_pd3dDevice, "ufo.x" )))
		return DXTRACE_ERR( "Load Mesh", hr );

	// ���� �ؽ�ó
	D3DXCreateTextureFromFileEx(m_pd3dDevice, "tatami.bmp"
								, 0,0,0,0,D3DFMT_A8R8G8B8
                                , D3DPOOL_MANAGED
								, D3DX_FILTER_LINEAR
								, D3DX_FILTER_LINEAR
                                , 0, NULL, NULL, &m_pTex);

    m_pFont->InitDeviceObjects( m_pd3dDevice );// ��Ʈ
    
	return S_OK;
}

//-------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: ȭ��ũ�Ⱑ �������� ȣ���
//       Ȯ���� �޸𸮴� InvalidateDeviceObjects()���� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	// �׸��� �ؽ�ó ����
	if (FAILED(m_pd3dDevice->CreateTexture(MAP_SIZE, MAP_SIZE, 1, 
					D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
					D3DPOOL_DEFAULT, &m_pShadowTex, NULL)))
		return E_FAIL;
	if (FAILED(m_pShadowTex->GetSurfaceLevel(0, &m_pShadowTexSurf)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateDepthStencilSurface(MAP_SIZE, MAP_SIZE, 
		D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pShadowTexZ, NULL)))
		return E_FAIL;


	m_pMesh  ->RestoreDeviceObjects(m_pd3dDevice);

	// �����ũ��
	#define RS   m_pd3dDevice->SetRenderState
	#define SAMP m_pd3dDevice->SetSamplerState

    // ������ ���¼���
    RS  ( D3DRS_ZENABLE,        TRUE );
	SAMP( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
	SAMP( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );
    SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	// ��������
	D3DLIGHT9 light;
	D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL
					, -m_light.x, -m_light.y, -m_light.z );
	m_pd3dDevice->SetLight( 0, &light );
	m_pd3dDevice->LightEnable( 0, TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

    // ���� ���
    D3DXMatrixIdentity( &m_mWorld );

    // ������� ����
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width )
				  / ((FLOAT)m_d3dsdBackBuffer.Height);
    D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect
								, 1.0f, 100.0f );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );

	
    m_pFont->RestoreDeviceObjects();	// ��Ʈ

    return S_OK;
}




//-------------------------------------------------------------
// Name: FrameMove()
// Desc: �� �����Ӹ��� ȣ���. �ִϸ��̼� ó���� ���
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	m_pos.x = 1.0f*(FLOAT)cos(1.0f*this->m_fTime);
	m_pos.z = 1.0f*(FLOAT)sin(1.0f*this->m_fTime);
	m_pos.y = 0.7f;

	UpdateInput( &m_UserInput ); // �Էµ����� ����

	//---------------------------------------------------------
	// �Է¿� ���� ��ǥ�踦 �����Ѵ�
	//---------------------------------------------------------
	// ȸ��
    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;

	// ��
	if(m_UserInput.bZ && !m_UserInput.bX)
		m_zoom += 0.01f;
	else if(m_UserInput.bX && !m_UserInput.bZ)
		m_zoom -= 0.01f;

	//---------------------------------------------------------
	// ��� ����
	//---------------------------------------------------------
	// ���� ȸ��
	D3DXMATRIX m, matRotX, matRotY;
    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );
    D3DXMatrixMultiply( &m, &matRotY, &matRotX );

    // �� ���
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
// Desc: �Էµ����� ����
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
// Desc: ȭ�� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	LPDIRECT3DSURFACE9 pOldBackBuffer, pOldZBuffer;
	D3DVIEWPORT9 oldViewport;
	D3DXMATRIX m, mW, mV, mP, mVP_inv;
	D3DXVECTOR4 v;

	if( SUCCEEDED( m_pd3dDevice->BeginScene()))	// ���� ����
    {
		//-----------------------------------------------------
		// ������Ÿ�� ����
		//-----------------------------------------------------
		m_pd3dDevice->GetRenderTarget(0, &pOldBackBuffer);
		m_pd3dDevice->GetDepthStencilSurface(&pOldZBuffer);
		m_pd3dDevice->GetViewport(&oldViewport);

		//-----------------------------------------------------
		// ������Ÿ�� ����
		//-----------------------------------------------------
		m_pd3dDevice->SetRenderTarget(0, m_pShadowTexSurf);
		m_pd3dDevice->SetDepthStencilSurface(m_pShadowTexZ);
		// ����Ʈ����            x y  width    height   minz maxz
		D3DVIEWPORT9 viewport = {0,0, MAP_SIZE,MAP_SIZE,0.0f,1.0f};
		m_pd3dDevice->SetViewport(&viewport);

		// �׸��ڸ� Ŭ����
		m_pd3dDevice->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
							0x00000000, 1.0f, 0L);

		//-----------------------------------------------------
		// 1�н�:�׸��ڸ� ����
		//-----------------------------------------------------
		// ����� ����
		m_pMesh->UseMeshMaterials(FALSE);
		D3DMATERIAL9 mtrl;
		ZeroMemory( &mtrl, sizeof(mtrl) );
		mtrl.Ambient.r = 1.0f;
		mtrl.Ambient.g = 1.0f;
		mtrl.Ambient.b = 1.0f;
		m_pd3dDevice->SetMaterial( &mtrl );
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffff );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

		// �������
		D3DXMatrixTranslation( &mW, m_pos.x, m_pos.y, m_pos.z );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &mW );
		// �� ���
		D3DXVECTOR3 vEye    = D3DXVECTOR3( m_light.x, m_light.y, m_light.z );
		D3DXVECTOR3 vUp     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
		D3DXMatrixLookAtLH( &mV, &vEye  //  ���� : ������ġ
			                   , &m_pos // �ָ��� : ������Ʈ
							   , &vUp );// ��溤��
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &mV );
		// �������
		D3DXMatrixPerspectiveFovLH( &mP, 0.20f*D3DX_PI, 1, 0.3f, 10.0f );
		m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &mP );

		m_pMesh->Render( m_pd3dDevice );
		
		m_pMesh->UseMeshMaterials(TRUE);

		//-----------------------------------------------------
		// ������Ÿ�� ����
		//-----------------------------------------------------
		m_pd3dDevice->SetRenderTarget(0, pOldBackBuffer);
		m_pd3dDevice->SetDepthStencilSurface(pOldZBuffer);
		m_pd3dDevice->SetViewport(&oldViewport);
		pOldBackBuffer->Release();
		pOldZBuffer->Release();
		m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_mView );
		m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );

		//-----------------------------------------------------
		// 2�н�:��鷻��
		//-----------------------------------------------------
		// ���� Ŭ����
		m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
							0x00404080, 1.0f, 0L );

		// ȭ�� Ŭ����
		m_pd3dDevice->Clear( 0L, NULL,
							D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
							0x4004080, 1.0f, 0L );
		
		// ����� ����
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0 );
		m_pd3dDevice->LightEnable( 0, TRUE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		D3DXMatrixTranslation( &mW, m_pos.x, m_pos.y, m_pos.z );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &mW );
		m_pMesh->Render( m_pd3dDevice );
		
		// ��� ����
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
		m_pd3dDevice->SetFVF( D3DFVF_XYZ | D3DFVF_TEX1 );

		typedef struct {
			FLOAT		p[3];
			FLOAT		tu, tv;
		} MyVERTEX;

		FLOAT size = 4.0f;

		MyVERTEX Vertex[4] = {
			// x    y    z     tu tv
			{{-size, 0, -size}, 0, 0,},
			{{-size, 0,  size}, 0, 1,},
			{{ size, 0,  size}, 1, 1,},
			{{ size, 0, -size}, 1, 0,},
		};
		D3DXMatrixIdentity( &m );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m );
		m_pd3dDevice->SetTexture( 0, m_pTex );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof( MyVERTEX ) );

		// �׸��� ����
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLOROP,	D3DTOP_MODULATE );
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
		m_pd3dDevice->SetTextureStageState(0,D3DTSS_COLORARG2,	D3DTA_DIFFUSE);
		m_pd3dDevice->SetTextureStageState(1,D3DTSS_COLOROP,    D3DTOP_DISABLE);
		// �����ռ� ����
		m_pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		m_pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCCOLOR );
		m_pd3dDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ZERO );
		m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
		
		typedef struct {
			FLOAT		p[3];
			DWORD		color;
			FLOAT		tu, tv;
		} LVERTEX;

		DWORD c = 0x80;
		LVERTEX ShadowVertex[4] = {
			// x   y  z       ��       (R G B A) tu tv
			{{-1, -1, 1}, D3DCOLOR_RGBA(c,c,c,0), 0, 1,},
			{{-1, +1, 1}, D3DCOLOR_RGBA(c,c,c,0), 0, 0,},
			{{+1, +1, 1}, D3DCOLOR_RGBA(c,c,c,0), 1, 0,},
			{{+1, -1, 1}, D3DCOLOR_RGBA(c,c,c,0), 1, 1,},
		};
		m = mV * mP;
		D3DXMatrixInverse(&mVP_inv, NULL, &m);
		D3DXPLANE Plane = D3DXPLANE(0,1,0, -0.01f);
		D3DXMatrixShadow( &m, &m_light, &Plane);
		m = mVP_inv * m;
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m );

		m_pd3dDevice->SetVertexShader(NULL);
		m_pd3dDevice->SetPixelShader(0);
		m_pd3dDevice->SetTexture( 0, m_pShadowTex );
		m_pd3dDevice->SetFVF( D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2,
								ShadowVertex, sizeof( LVERTEX ) );

		RS( D3DRS_ALPHABLENDENABLE, FALSE );

#if 0	// ������¿� ���߱�
		RS(  D3DRS_CULLMODE, D3DCULL_NONE  );
		LVERTEX ShadowLine[8] = {
			// x   y  z       ��       (R G B A) tu tv
			{{-1, -1, 0}, D3DCOLOR_RGBA(c,c,c,0), 0.5, 0.5,},
			{{-1, -1, 1}, D3DCOLOR_RGBA(c,c,c,0), 0.5, 0.5,},

			{{+1, -1, 0}, D3DCOLOR_RGBA(c,c,c,0), 0.5, 0.5,},
			{{+1, -1, 1}, D3DCOLOR_RGBA(c,c,c,0), 0.5, 0.5,},
			
			{{-1, +1, 0}, D3DCOLOR_RGBA(c,c,c,0), 0.5, 0.5,},
			{{-1, +1, 1}, D3DCOLOR_RGBA(c,c,c,0), 0.5, 0.5,},
			
			{{+1, +1, 0}, D3DCOLOR_RGBA(c,c,c,0), 0.5, 0.5,},
			{{+1, +1, 1}, D3DCOLOR_RGBA(c,c,c,0), 0.5, 0.5,},
		};
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_LINELIST, 4, ShadowLine, sizeof( LVERTEX ) );
#endif

		// ������ġ�� ������ ǥ��
		m = m_mView * m_mProj;
		D3DXVec4Transform(&v, &m_light, &m);
		m_pd3dDevice->GetViewport(&viewport);
		v.x = 0.5f*viewport.Width *( v.x/v.w + 1.0f);
		v.y = 0.5f*viewport.Height*(-v.y/v.w + 1.0f);
		v.z = v.z/v.w;

		typedef struct {FLOAT p[4]; DWORD color;} POINT_VERTEX;
		POINT_VERTEX PointVertex[4] = {
			// x y  z                                ��   (R    G    B    A)
			{{v.x-2.0f, v.y-2.0f, v.z, v.w}, D3DCOLOR_RGBA(0xff,0xff,0xff,0),},
			{{v.x+2.0f, v.y-2.0f, v.z, v.w}, D3DCOLOR_RGBA(0xff,0xff,0xff,0),},
			{{v.x+2.0f, v.y+2.0f, v.z, v.w}, D3DCOLOR_RGBA(0xff,0xff,0xff,0),},
			{{v.x-2.0f, v.y+2.0f, v.z, v.w}, D3DCOLOR_RGBA(0xff,0xff,0xff,0),},
		};
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_DIFFUSE );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, PointVertex, sizeof( POINT_VERTEX ) );

#if 1 // ����׿� �ؽ�ó ���
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
		RenderText();				// ���� ���

        m_pd3dDevice->EndScene();	// ��������
    }
	
    return S_OK;
}




//-------------------------------------------------------------
// Name: RenderText()
// Desc: ���¿� ������ ȭ�鿡 ���
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    FLOAT fNextLine = 40.0f; 

    // ���۹��
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
	sprintf( szMsg, "Zoom: %f", m_zoom );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Use arrow keys to rotate object") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Press 'z' or 'x' to change zoom") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    // ���÷��� ����
    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    
	return S_OK;
}




//-------------------------------------------------------------
// Name: MsgProc()
// Desc: WndProc �������̵�
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
                // �ε���
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
// Desc: RestoreDeviceObjects() ���� ������ ������Ʈ ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	// �׸��� �ؽ�ó
	SAFE_RELEASE(m_pShadowTexSurf);
	SAFE_RELEASE(m_pShadowTex);
	SAFE_RELEASE(m_pShadowTexZ);

	m_pMesh  ->InvalidateDeviceObjects();
	
    m_pFont->InvalidateDeviceObjects();	// ��Ʈ

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() ���� ������ ������Ʈ ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	m_pMesh  ->Destroy();

	SAFE_RELEASE( m_pTex );

    m_pFont->DeleteDeviceObjects();	// ��Ʈ

	return S_OK;
}




//-------------------------------------------------------------
// Name: FinalCleanup()
// Desc: ���������� ȣ���
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
	SAFE_DELETE( m_pMesh   ); // �޽�

    SAFE_DELETE( m_pFont );	// ��Ʈ

    return S_OK;
}




