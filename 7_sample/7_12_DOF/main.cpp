//-------------------------------------------------------------
// File: main.cpp
//
// Desc: ÇÇ»ç°è½Éµµ
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


// ´ÜÃà¸ÅÅ©·Î
#define RS   m_pd3dDevice->SetRenderState
#define TSS  m_pd3dDevice->SetTextureStageState
#define SAMP m_pd3dDevice->SetSamplerState


//-------------------------------------------------------------
// Á¤Á¡±¸Á¶Ã¼
//-------------------------------------------------------------
typedef struct {
    FLOAT       p[4];
    FLOAT       tu, tv;	// ÅØ½ºÃ³ 1Àå
} TVERTEX;

typedef struct {
    FLOAT       p[3];
    FLOAT       t[4][2];// ÅØ½ºÃ³ 4Àå
} T4VERTEX;

//-------------------------------------------------------------
// Àü¿ªº¯¼ö
//-------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;


//-------------------------------------------------------------
// Name: WinMain()
// Desc: ¸ÞÀÎÇÔ¼ö
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
// Desc: ¾îÇÃ¸®ÄÉÀÌ¼Ç »ý¼ºÀÚ
//-------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
	m_pMesh						= new CD3DMesh();
	m_pMeshBg					= new CD3DMesh();

	m_Focus						= 0.65f;
	m_Scale						= 1.0f;

	m_pMapZ						= NULL;
	m_pOriginalTex				= NULL;
	m_pOriginalSurf				= NULL;
	m_pPostTex[0]				= NULL;
	m_pPostTex[1]				= NULL;
	m_pPostSurf[0]				= NULL;
	m_pPostSurf[1]				= NULL;

	m_pEffect					= NULL;
	m_hTechnique  				= NULL;
	m_hmWVP						= NULL;
	m_hvCol						= NULL;
	m_hvDir						= NULL;
	m_hvCenter					= NULL;
	m_hvScale					= NULL;
	m_htSrcTex					= NULL;
	m_htBlurTex					= NULL;

	m_fWorldRotX                = -0.2f;
    m_fWorldRotY                = 0.0f;
	m_fViewZoom				    = 3.2f;

	m_LighPos					= D3DXVECTOR3( -5.0f, 5.0f,-2.0f );

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
// Desc: ¼Ò¸êÀÚ
//-------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
}




//-------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: ´Ü ÇÑ¹ø¸¸ ÃÊ±âÈ­
//       À©µµ¿ì ÃÊ±âÈ­¿Í IDirect3D9ÃÊ±âÈ­´Â ³¡³­µÚ
//       ±×·¯³ª LPDIRECT3DDEVICE9ÃÊ±âÈ­´Â ³¡³ªÁö ¾ÊÀº »óÅÂ
//-------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    // ·Îµù ¸Þ½ÃÁö Ãâ·Â
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    m_bLoadingApp = FALSE;

    return S_OK;
}




//-------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: ÃÊ±âÈ­½Ã È£ÃâµÊ. ÇÊ¿äÇÑ ´É·Â(caps)Ã¼Å©
//-------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps,
                     DWORD dwBehavior,    D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    

	// ÇÈ¼¿¼ÎÀÌ´õ ¹öÀüÃ¼Å©
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(1,1) )
		return E_FAIL;

    // Á¤Á¡¼ÎÀÌ´õ ¹öÀüÀÌ ¸ÂÁö¾ÊÀ¸¸é ¼ÒÇÁÆ®¿þ¾î Ã³¸®
    if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1)
    &&  0==(dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) )
			return E_FAIL;

    return S_OK;
}




//-------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: µð¹ÙÀÌ½º°¡ »ý¼ºµÈÈÄÀÇ ÃÊ±âÈ­
//       ÇÁ·¹ÀÓ¹öÆÛ Æ÷¸Ë°ú µð¹ÙÀÌ½º Á¾·ù°¡ º¯ÇÑµÚ¿¡ È£Ãâ
//       ¿©±â¼­ È®º¸ÇÑ ¸Þ¸ð¸®´Â DeleteDeviceObjects()¿¡¼­ ÇØÁ¦
//-------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    HRESULT hr;

	// ¸ðµ¨ÀÐ±â
	if(FAILED(hr=m_pMesh  ->Create( m_pd3dDevice, _T("t-pot.x"))))
        return DXTRACE_ERR( "LoadMesh", hr );
	m_pMesh  ->UseMeshMaterials(FALSE);// ·»´õ¸µ½Ã ÅØ½ºÃ³ ¼³Á¤À» ÇÏÁö ¾ÊÀ½
	// Áö¸éÀÐ±â
	if(FAILED(hr=m_pMeshBg->Create( m_pd3dDevice, _T("sky.x"))))
        return DXTRACE_ERR( "Load BG", hr );
	m_pMeshBg->UseMeshMaterials(FALSE);// ·»´õ¸µ½Ã ÅØ½ºÃ³ ¼³Á¤À» ÇÏÁö ¾ÊÀ½
        
	// ¼ÎÀÌ´õ ÀÐ±â
	LPD3DXBUFFER pErr;
    if( FAILED( hr = D3DXCreateEffectFromFile(
				m_pd3dDevice, "hlsl.fx", NULL, NULL, 
				D3DXSHADER_DEBUG , NULL, &m_pEffect, &pErr ))){
		MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer()
					, "ERROR", MB_OK);
		return DXTRACE_ERR( "CreateEffectFromFile", hr );
	}
	m_hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
	m_hmWVP		 = m_pEffect->GetParameterByName( NULL, "mWVP" );
	m_hvCol		 = m_pEffect->GetParameterByName( NULL, "vCol" );
	m_hvDir      = m_pEffect->GetParameterByName( NULL, "vLightDir" );
	m_hvCenter   = m_pEffect->GetParameterByName( NULL, "vCenter" );
	m_hvScale    = m_pEffect->GetParameterByName( NULL, "vScale" );

	m_htSrcTex   = m_pEffect->GetParameterByName( NULL, "SrcTex" );
	m_htBlurTex  = m_pEffect->GetParameterByName( NULL, "BlurTex" );
	m_pEffect->SetFloat("MAP_WIDTH",  MAP_WIDTH);
	m_pEffect->SetFloat("MAP_HEIGHT", MAP_HEIGHT);

	// ÆùÆ®
    m_pFont->InitDeviceObjects( m_pd3dDevice );

    return S_OK;
}

//-------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: È­¸éÅ©±â°¡ º¯ÇßÀ»¶§ È£ÃâµÊ
//       È®º¸ÇÑ ¸Þ¸ð¸®´Â InvalidateDeviceObjects()¿¡¼­ ÇØÁ¦
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	DWORD i;

	// ·»´õ¸µÅ¸°Ù »ý¼º
	if (FAILED(m_pd3dDevice->CreateDepthStencilSurface(MAP_WIDTH, MAP_HEIGHT, 
		D3DFMT_D16, D3DMULTISAMPLE_NONE, 0, TRUE, &m_pMapZ, NULL)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateTexture(MAP_WIDTH, MAP_HEIGHT, 1, 
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pOriginalTex, NULL)))
		return E_FAIL;
	if (FAILED(m_pOriginalTex->GetSurfaceLevel(0, &m_pOriginalSurf)))
		return E_FAIL;
	// ¹¶°³±âÀ§ÇÑ °Íµé
	for(i=0;i<2;i++){
		if (FAILED(m_pd3dDevice->CreateTexture(
							MAP_WIDTH/2, MAP_HEIGHT/2, 1,
							D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
							D3DPOOL_DEFAULT, &m_pPostTex[i], NULL)))
			return E_FAIL;
		if (FAILED(m_pPostTex[i]->GetSurfaceLevel(0, &m_pPostSurf[i])))
			return E_FAIL;
	}

	// ÀÌÆåÆ®
	m_pEffect->OnResetDevice();

	// ¸Þ½Ã
	m_pMesh  ->RestoreDeviceObjects( m_pd3dDevice );
	m_pMeshBg->RestoreDeviceObjects( m_pd3dDevice );

    // ÀçÁú¼³Á¤
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );


    // ·»´õ¸µ »óÅÂ¼³Á¤
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

    // ¿ùµåÇà·Ä
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &m_mWorld );

	// ºäÇà·Ä
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFromPt, &vLookatPt, &vUpVec );

    // Åõ¿µÇà·Ä
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );

    // ÆùÆ®
    m_pFont->RestoreDeviceObjects();

	return S_OK;
}




//-------------------------------------------------------------
// Name: FrameMove()
// Desc: ¸Å ÇÁ·¹ÀÓ¸¶´Ù È£ÃâµÊ. ¾Ö´Ï¸ÞÀÌ¼Ç Ã³¸®µî ´ã´ç
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	UpdateInput( &m_UserInput ); // ÀÔ·Âµ¥ÀÌÅÍ °»½Å

	//---------------------------------------------------------
	// ÀÔ·Â¿¡ µû¶ó ÁÂÇ¥°è¸¦ °»½ÅÇÑ´Ù
	//---------------------------------------------------------
	// È¸Àü
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
	// ºäÇà·Ä ¼³Á¤
	//---------------------------------------------------------
	// ÁÜ
    if( m_UserInput.bZoomIn && !m_UserInput.bZoomOut )
        m_fViewZoom += m_fElapsedTime;
    else if( m_UserInput.bZoomOut && !m_UserInput.bZoomIn )
        m_fViewZoom -= m_fElapsedTime;

    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.3f, -m_fViewZoom );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.3f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFromPt, &vLookatPt, &vUpVec );

	//---------------------------------------------------------
	// DOFÀÇ ÀÎ¼öÁ¶Á¤
	//---------------------------------------------------------
	// ÁÜ
    if( m_UserInput.bA && !m_UserInput.bS )
        m_Focus += 0.1f*m_fElapsedTime;
    else if( m_UserInput.bS && !m_UserInput.bA )
        m_Focus -= 0.1f*m_fElapsedTime;

    if( m_UserInput.bQ && !m_UserInput.bW )
        m_Scale += m_fElapsedTime;
    else if( m_UserInput.bW && !m_UserInput.bQ )
        m_Scale -= m_fElapsedTime;
	if(  m_Scale<0)m_Scale=0;
	if(2<m_Scale  )m_Scale=2;

	return S_OK;
}
//-------------------------------------------------------------
// Name: UpdateInput()
// Desc: ÀÔ·Âµ¥ÀÌÅÍ °»½Å
//-------------------------------------------------------------
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    pUserInput->bRotateUp    = ( m_bActive && (GetAsyncKeyState( VK_UP )    & 0x8000) == 0x8000 );
    pUserInput->bRotateDown  = ( m_bActive && (GetAsyncKeyState( VK_DOWN )  & 0x8000) == 0x8000 );
    pUserInput->bRotateLeft  = ( m_bActive && (GetAsyncKeyState( VK_LEFT )  & 0x8000) == 0x8000 );
    pUserInput->bRotateRight = ( m_bActive && (GetAsyncKeyState( VK_RIGHT ) & 0x8000) == 0x8000 );
    
	pUserInput->bZoomIn      = ( m_bActive && (GetAsyncKeyState( 'Z'     )  & 0x8000) == 0x8000 );
    pUserInput->bZoomOut     = ( m_bActive && (GetAsyncKeyState( 'X'      ) & 0x8000) == 0x8000 );

	pUserInput->bA           = ( m_bActive && (GetAsyncKeyState( 'A'     )  & 0x8000) == 0x8000 );
    pUserInput->bS           = ( m_bActive && (GetAsyncKeyState( 'S'      ) & 0x8000) == 0x8000 );
	pUserInput->bQ           = ( m_bActive && (GetAsyncKeyState( 'Q'     )  & 0x8000) == 0x8000 );
    pUserInput->bW           = ( m_bActive && (GetAsyncKeyState( 'W'      ) & 0x8000) == 0x8000 );
}


//-------------------------------------------------------------
// Name: Render()
// Desc: È­¸é ·»´õ
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    D3DXMATRIX m, mS, mT, mR, mW, mView, mProj;
	int i, j;
	D3DXVECTOR4 v;
	LPDIRECT3DSURFACE9 pOldBackBuffer, pOldZBuffer;
	D3DVIEWPORT9 oldViewport;
	D3DMATERIAL9 *pMtrl;

	//---------------------------------------------------------
	// ·»´õ
	//---------------------------------------------------------
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		if( m_pEffect != NULL ) 
		{
			//-------------------------------------------------
			// ·»´õ¸µÅ¸°Ù º¸Á¸
			//-------------------------------------------------
			m_pd3dDevice->GetRenderTarget(0, &pOldBackBuffer);
			m_pd3dDevice->GetDepthStencilSurface(&pOldZBuffer);
			m_pd3dDevice->GetViewport(&oldViewport);

			//-------------------------------------------------
			// ¼ÎÀÌ´õ ¼³Á¤
			//-------------------------------------------------
			m_pEffect->SetTechnique( m_hTechnique );
			m_pEffect->Begin( NULL, 0 );

			//-------------------------------------------------
			// ·»´õ¸µÅ¸°Ù º¯°æ
			//-------------------------------------------------
			m_pd3dDevice->SetRenderTarget(0, m_pOriginalSurf);
			m_pd3dDevice->SetDepthStencilSurface(m_pMapZ);
			// ºäÆ÷Æ® º¯°æ
			D3DVIEWPORT9 viewport = {0,0      // ÁÂÃø»ó´Ü
							, MAP_WIDTH  // Æø
							, MAP_HEIGHT // ³ôÀÌ
							, 0.0f,1.0f};     // Àü¸é,ÈÄ¸é
			m_pd3dDevice->SetViewport(&viewport);

			// ·»´õ¸µÅ¸°Ù Å¬¸®¾î
			m_pd3dDevice->Clear(0L, NULL
							, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
							, 0xff000000, 1.0f, 0L);

			//-------------------------------------------------
			// ¸ðµ¨·»´õ
			//-------------------------------------------------
			m_pEffect->Pass( 0 );
			m = m_mWorld * m_mView * m_mProj;
			m_pEffect->SetMatrix( m_hmWVP, &m );

			pMtrl = m_pMeshBg->m_pMaterials;
			for( i=0; i<m_pMeshBg->m_dwNumMaterials; i++ ) {
				m_pEffect->SetTexture(m_htSrcTex, m_pMeshBg->m_pTextures[i] );
				m_pMeshBg->GetLocalMesh()->DrawSubset( i );	// •`‰æ
				pMtrl++;
			}

			// ÁÖÀüÀÚ ·»´õ
			m_pEffect->Pass( 1 );
			D3DXMatrixScaling( &mS,  0.5f, 0.5f, 0.5f );
			D3DXMatrixRotationY( &mR,  m_fTime );
			for( i=0;i<9;i++){
				D3DXMatrixTranslation( &mT, 1.3f*(float)((i%3)-1), 0.3f,1.3f*(float)((i/3)-1) );
				mW = mS * mR * mT * m_mWorld;
				m = mW * m_mView * m_mProj;
				m_pEffect->SetMatrix( m_hmWVP, &m );
				// ±¤¿ø¼³Á¤
				D3DXMatrixInverse( &m, NULL, &mW );
				D3DXVec3Transform( &v, &m_LighPos, &m );
				D3DXVec4Normalize( &v, &v );v.w = 0.3;
				m_pEffect->SetVector( m_hvDir, &v );

				pMtrl = m_pMesh->m_pMaterials;
				for( j=0; j<m_pMesh->m_dwNumMaterials; j++ ) {
					v.x = pMtrl->Diffuse.r;
					v.y = pMtrl->Diffuse.g;
					v.z = pMtrl->Diffuse.b;
					v.w = pMtrl->Diffuse.a;
					m_pEffect->SetVector( m_hvCol, &v );
					m_pEffect->SetTexture(m_htSrcTex, m_pMesh->m_pTextures[j] );
					m_pMesh->GetLocalMesh()->DrawSubset( j );	// •`‰æ
					pMtrl++;
				}
			}

			//-------------------------------------------------
			// ÇÊÅÍ¸µ
			//-------------------------------------------------
			m_pEffect->Pass( 2 );
			RS( D3DRS_ZENABLE, FALSE );
			RS( D3DRS_LIGHTING, FALSE );
			TSS(0,D3DTSS_COLOROP,	D3DTOP_SELECTARG1);
			TSS(0,D3DTSS_COLORARG1,	D3DTA_TEXTURE);
			TSS(1,D3DTSS_COLOROP,   D3DTOP_DISABLE);
			
			float du = 1.0f/MAP_WIDTH;
			float dv = 1.0f/MAP_HEIGHT;
			T4VERTEX Vertex1[4] = {
				// x      y     z    u0 v0   u1  v1 u2   v2    u3    v3
				{-1.0f, +1.0f, 0.1f,  0, 0, 0+du, 0, 0, 0+dv, 0+du, 0+dv,},
				{+1.0f, +1.0f, 0.1f,  1, 0, 1+du, 0, 1, 0+dv, 1+du, 0+dv,},
				{+1.0f, -1.0f, 0.1f,  1, 1, 1+du, 1, 1, 1+dv, 1+du, 1+dv,},
				{-1.0f, -1.0f, 0.1f,  0, 1, 0+du, 1, 0, 1+dv, 0+du, 1+dv,},
			};
			m_pd3dDevice->SetFVF( D3DFVF_XYZ | D3DFVF_TEX4 );

			m_pd3dDevice->SetRenderTarget(0, m_pPostSurf[0]);
			m_pEffect->SetTexture(m_htSrcTex, m_pOriginalTex);
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
							, 2, Vertex1, sizeof( T4VERTEX ) );

			du = 0.5f/MAP_WIDTH;
			dv = 0.5f/MAP_HEIGHT;
			T4VERTEX Vertex2[4] = {
				// x      y     z    u0 v0   u1  v1 u2   v2    u3    v3
				{-1.0f, +1.0f, 0.1f,  0, 0, 0+du, 0, 0, 0+dv, 0+du, 0+dv,},
				{+1.0f, +1.0f, 0.1f,  1, 0, 1+du, 0, 1, 0+dv, 1+du, 0+dv,},
				{+1.0f, -1.0f, 0.1f,  1, 1, 1+du, 1, 1, 1+dv, 1+du, 1+dv,},
				{-1.0f, -1.0f, 0.1f,  0, 1, 0+du, 1, 0, 1+dv, 0+du, 1+dv,},
			};
			m_pd3dDevice->SetRenderTarget(0, m_pPostSurf[1]);
			m_pEffect->SetTexture(m_htSrcTex, m_pPostTex[0]);
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
							, 2, Vertex2, sizeof( T4VERTEX ) );

			m_pd3dDevice->SetRenderTarget(0, m_pPostSurf[0]);
			m_pEffect->SetTexture(m_htSrcTex, m_pPostTex[1]);
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
							, 2, Vertex2, sizeof( T4VERTEX ) );

			m_pd3dDevice->SetRenderTarget(0, m_pPostSurf[1]);
			m_pEffect->SetTexture(m_htSrcTex, m_pPostTex[0]);
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
							, 2, Vertex2, sizeof( T4VERTEX ) );

			//-----------------------------------------------------
			// ·»´õ¸µÅ¸°Ù º¹±¸
			//-----------------------------------------------------
			m_pd3dDevice->SetRenderTarget(0, pOldBackBuffer);
			m_pd3dDevice->SetDepthStencilSurface(pOldZBuffer);
			m_pd3dDevice->SetViewport(&oldViewport);
			pOldBackBuffer->Release();
			pOldZBuffer->Release();

			// ¹öÆÛÅ¬¸®¾î
			m_pd3dDevice->Clear( 0L, NULL
							, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER
							, 0x00404080, 1.0f, 0L );

			//-----------------------------------------------------
			// ºí·¯ÇÑ °ÍÀ» ºÙÀÎ´Ù
			//-----------------------------------------------------
			m_pEffect->Pass(3);
			
			v.w = m_Focus;
			m_pEffect->SetVector( m_hvCenter, &v );
			v.w = m_Scale;
			m_pEffect->SetVector( m_hvScale, &v );

			FLOAT w = (FLOAT)oldViewport.Width;
			FLOAT h = (FLOAT)oldViewport.Height;
			FLOAT dw = 0.5f/w;
			FLOAT dh = 0.5f/h;
			T4VERTEX Vertex[4] = {
				//x  y   z    w    tu    tv
				{ 0, 0, 0.1f, 1, 0+dw, 0+dh, 0+dw, 0+dh, 0+dw, 0+dh,},
				{ w, 0, 0.1f, 1, 1+dw, 0+dh, 1+dw, 0+dh, 1+dw, 0+dh,},
				{ w, h, 0.1f, 1, 1+dw, 1+dh, 1+dw, 1+dh, 1+dw, 1+dh,},
				{ 0, h, 0.1f, 1, 0+dw, 1+dh, 0+dw, 1+dh, 0+dw, 1+dh,},
			};
			m_pEffect->SetTexture( m_htSrcTex,   m_pOriginalTex );
			m_pEffect->SetTexture( m_htBlurTex,  m_pPostTex[1] );
			m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX4 );
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
								, 2, Vertex, sizeof( T4VERTEX ) );
			m_pEffect->End();
		}

		RS( D3DRS_ZENABLE, TRUE );
		RS( D3DRS_LIGHTING, TRUE );

		RenderText();				// µµ¿ò¸» Ãâ·Â

#if 0 // µð¹ö±×¿ë ÅØ½ºÃ³ Ãâ·Â
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
			if(0==i) m_pd3dDevice->SetTexture( 0, m_pOriginalTex );
			if(1==i) m_pd3dDevice->SetTexture( 0, m_pPostTex[1] );
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof( TVERTEX ) );
		}
		}
#endif		

        m_pd3dDevice->EndScene();	// ·»´õÁ¾·á
    }

    return S_OK;
}




//-------------------------------------------------------------
// Name: RenderText()
// Desc: »óÅÂ¿Í µµ¿ò¸»À» È­¸é¿¡ Ãâ·Â
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    FLOAT fNextLine = 40.0f; // Ãâ·Â³ôÀÌ

    // Á¶ÀÛ¹æ¹ý
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 

    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    sprintf( szMsg, TEXT("Press 'Q'-'W' to change strength of the DOF (Now %f)"), m_Scale );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    sprintf( szMsg, TEXT("Press 'A'-'S' to change the point of the focus (Now %f)"), m_Focus );
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
// Desc: WndProc ¿À¹ö¶óÀÌµù
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
                // ·ÎµåÁß
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
// Desc: RestoreDeviceObjects() ¿¡¼­ »ý¼ºÇÑ ¿ÀºêÁ§Æ® ÇØÁ¦
//-------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	// ·»´õ¸µÅ¸°Ù
	SAFE_RELEASE(m_pPostSurf[1]);
	SAFE_RELEASE(m_pPostTex[1]);
	SAFE_RELEASE(m_pPostSurf[0]);
	SAFE_RELEASE(m_pPostTex[0]);
	SAFE_RELEASE(m_pOriginalSurf);
	SAFE_RELEASE(m_pOriginalTex);
	SAFE_RELEASE(m_pMapZ);

	m_pMesh  ->InvalidateDeviceObjects(); // ¸Þ½Ã
	m_pMeshBg->InvalidateDeviceObjects();

    m_pFont->InvalidateDeviceObjects();	// ÆùÆ®

	// ÀÌÆåÆ®
    if( m_pEffect != NULL ) m_pEffect->OnLostDevice();

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() ¿¡¼­ »ý¼ºÇÑ ¿ÀºêÁ§Æ® ÇØÁ¦
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    // ¼ÎÀÌ´õ
	SAFE_RELEASE( m_pEffect );
	
	// ¸Þ½Ã
	m_pMesh  ->Destroy();
	m_pMeshBg->Destroy();

    // ÆùÆ®
    m_pFont->DeleteDeviceObjects();

    return S_OK;
}




//-------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Á¾·áÁ÷Àü¿¡ È£ÃâµÊ
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    SAFE_DELETE( m_pMeshBg ); // ¸Þ½Ã
	SAFE_DELETE( m_pMesh );

    SAFE_DELETE( m_pFont );	// ÆùÆ®

    return S_OK;
}




