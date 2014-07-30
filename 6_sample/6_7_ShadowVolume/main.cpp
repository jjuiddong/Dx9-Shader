//-------------------------------------------------------------
// File: main.cpp
//
// Desc: º¼·ý ±×¸²ÀÚ
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

#define RS   m_pd3dDevice->SetRenderState
#define TSS  m_pd3dDevice->SetTextureStageState
#define SAMP m_pd3dDevice->SetSamplerState

//-------------------------------------------------------------
// ÀüÃ¼È­¸é ·»´õ Æú¸®°ï
//-------------------------------------------------------------
const DWORD CBigSquare::FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

//-------------------------------------------------------------
// »ý¼ºÀÚ
//-------------------------------------------------------------
CBigSquare::CBigSquare()
{
	m_pVB=NULL;
}
//-------------------------------------------------------------
// ¸Þ¸ð¸®È®º¸
//-------------------------------------------------------------
HRESULT CBigSquare::Create( LPDIRECT3DDEVICE9 pd3dDevice )
{
	if( FAILED( pd3dDevice->CreateVertexBuffer(
				4*sizeof(SHADOWVERTEX),	// Á¤Á¡¹öÆÛÅ©±â
				D3DUSAGE_WRITEONLY,		// »ç¿ë¹ý
				FVF,					// Á¤Á¡Æ÷¸Ë
				D3DPOOL_MANAGED,		// À¯È¿ÇÑ ¸Þ¸ð¸® Å¬·¡½º
				&m_pVB,					// »ý¼ºµÈ Á¤Á¡¹öÆÛ
				NULL ) ) )				// NULL°íÁ¤
		return E_FAIL;

	return S_OK;
}
//-------------------------------------------------------------
// µ¥ÀÌÅÍ »ý¼º
//-------------------------------------------------------------
void CBigSquare::RestoreDeviceObjects( FLOAT sx, FLOAT sy )
{
	SHADOWVERTEX* v;
	m_pVB->Lock( 0, 0, (void**)&v, 0 );
	v[0].p = D3DXVECTOR4(  0, sy, 0.0f, 1.0f );
	v[1].p = D3DXVECTOR4(  0,  0, 0.0f, 1.0f );
	v[2].p = D3DXVECTOR4( sx, sy, 0.0f, 1.0f );
	v[3].p = D3DXVECTOR4( sx,  0, 0.0f, 1.0f );
	v[0].color = D3DCOLOR_RGBA(0,0,0,0x7f);
	v[1].color = D3DCOLOR_RGBA(0,0,0,0x7f);
	v[2].color = D3DCOLOR_RGBA(0,0,0,0x7f);
	v[3].color = D3DCOLOR_RGBA(0,0,0,0x7f);
	m_pVB->Unlock();
}
//-------------------------------------------------------------
// ¸Þ¸ð¸® ÇØÁ¦
//-------------------------------------------------------------
void CBigSquare::Destroy()
{
	SAFE_RELEASE( m_pVB );
}
//-------------------------------------------------------------
// È­¸é·»´õ
//-------------------------------------------------------------
void CBigSquare::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	pd3dDevice->SetFVF( FVF );
	pd3dDevice->SetStreamSource( 0, m_pVB, 0, sizeof(SHADOWVERTEX));
	pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
}




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
    m_pBigSquare				= new CBigSquare();
	m_pMeshBG					= new CD3DMesh();				
	m_pMeshBox					= new CD3DMesh();
	m_pShadowBox				= new CShadowVolume();

	m_pEffect					= NULL;
	m_hmWVP						= NULL;
	m_hvPos						= NULL;

	// ¼±ÅÃµÈ Àå¸é ¼³Á¤
    m_d3dEnumeration.AppUsesDepthBuffer = TRUE;
    m_d3dEnumeration.AppMinDepthBits = 15;
    m_d3dEnumeration.AppMinStencilBits = 1;

	m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "main" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    // ÆùÆ®
    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
    m_fWorldRotX                = 0.0f;
    m_fWorldRotY                = 0.0f;
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
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior,
D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat )
{
	// ¼ÎÀÌ´õ Ã¼Å©
	if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
		if( (dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) == 0 )
			return E_FAIL;
	
	// ¾ç¸é½ºÅÙ½Ç ±â´É È®ÀÎ
	if( !( pCaps->StencilCaps & D3DSTENCILCAPS_TWOSIDED ) ) return E_FAIL;
	
	// ½ºÅÙ½Ç±â´ÉÀ» Áö¿øÇÏ´ÂÁö Ã¼Å©
	if( FAILED( m_pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal
			, pCaps->DeviceType
			, adapterFormat
			, D3DUSAGE_RENDERTARGET
			// ÇÈ¼¿¼ÎÀÌ´õÈÄ ºí·»µù Áö¿ø ±â´ÉÀÌ ÀÖ´Â°¡?
			| D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING
			, D3DRTYPE_SURFACE
			, backBufferFormat ) ) )
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
	DWORD i;
	
	// ÀÌÆåÆ®
	if( FAILED( D3DXCreateEffectFromFile( m_pd3dDevice
								, "hlsl.fx", NULL, NULL 
								, 0, NULL, &m_pEffect, NULL )))
		return E_FAIL;
	m_hmWVP = m_pEffect->GetParameterByName( NULL, "mWVP" );
	m_hvPos = m_pEffect->GetParameterByName( NULL, "vLightPos" );

	// ÀüÃ¼È­¸é·»´õ Æú¸®°ï ÃÊ±âÈ­
	if( FAILED( m_pBigSquare->Create( m_pd3dDevice ) )) return E_FAIL;

	// ¹è°æ¸Þ½Ã ÀÐ±â
	if( FAILED( hr = m_pMeshBG->Create( m_pd3dDevice, "CornellNoBox.x" ) ) )
		return DXTRACE_ERR( "Load Mesh", hr );
	for(i=0;i<m_pMeshBG->m_dwNumMaterials;i++){
		m_pMeshBG->m_pMaterials[i].Ambient.r = m_pMeshBG->m_pMaterials[i].Diffuse.r*=2;
		m_pMeshBG->m_pMaterials[i].Ambient.g = m_pMeshBG->m_pMaterials[i].Diffuse.g*=2;
		m_pMeshBG->m_pMaterials[i].Ambient.b = m_pMeshBG->m_pMaterials[i].Diffuse.b*=2;
	}
	// »óÀÚ¸Þ½Ã ÀÐ±â
	if( FAILED( hr = m_pMeshBox->Create( m_pd3dDevice, "box.x" ) ) )
		return DXTRACE_ERR( "Load Mesh", hr );
	for(i=0;i<m_pMeshBox->m_dwNumMaterials;i++){
		m_pMeshBox->m_pMaterials[i].Ambient.r = m_pMeshBox->m_pMaterials[i].Diffuse.r*=1.13f;
		m_pMeshBox->m_pMaterials[i].Ambient.g = m_pMeshBox->m_pMaterials[i].Diffuse.g*=0.93f;
		m_pMeshBox->m_pMaterials[i].Ambient.b = m_pMeshBox->m_pMaterials[i].Diffuse.b*=0.53f;
	}
	
	// ±×¸²ÀÚº¼·ý »ý¼º
	m_pShadowBox->Create( m_pMeshBox->GetSysMemMesh() );

    // ÆùÆ® ÃÊ±âÈ­
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
	m_LighPos = D3DXVECTOR3(0.0f, 5.488f, 2.770f);

    // ÀçÁú¼³Á¤
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

    // ÅØ½ºÃ³ ¼³Á¤
    TSS( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
    TSS( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    TSS( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
    TSS( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
    TSS( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    TSS( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
    SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

    // ·»´õ¸µ »óÅÂ¼³Á¤
    RS( D3DRS_DITHERENABLE,   FALSE );
    RS( D3DRS_SPECULARENABLE, FALSE );
    RS( D3DRS_ZENABLE,        TRUE );
    RS( D3DRS_AMBIENT,        0x000F0F0F );

    // ¿ùµåÇà·Ä
    D3DXMATRIX matIdentity;
    D3DXMatrixIdentity( &matIdentity );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &matIdentity );

	// ºäÇà·Ä
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFromPt, &vLookatPt, &vUpVec );
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_mView );

    // Åõ¿µÇà·Ä
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &m_mProj, 0.21f*D3DX_PI, fAspect, 1.0f, 100.0f );
    m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );

	// ±¤¿ø¼³Á¤
    D3DLIGHT9 light;
    D3DUtil_InitLight( light, D3DLIGHT_DIRECTIONAL, -0.0f, -1.0f, 0.2f );
    light.Diffuse.r   = 0.5f;
    light.Diffuse.g   = 0.5f;
    light.Diffuse.b   = 0.5f;
    light.Ambient.r   = 0.5f;
    light.Ambient.g   = 0.5f;
    light.Ambient.b   = 0.5f;
    m_pd3dDevice->SetLight( 0, &light );
    m_pd3dDevice->LightEnable( 0, TRUE );
    m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	// »ç°¢Çü
	m_pBigSquare->RestoreDeviceObjects( (FLOAT)m_d3dsdBackBuffer.Width,
										(FLOAT)m_d3dsdBackBuffer.Height );
	// ¸Þ½Ã
	m_pMeshBG->RestoreDeviceObjects(m_pd3dDevice);
	m_pMeshBox->RestoreDeviceObjects(m_pd3dDevice);

	// ÀÌÆåÆ®
	if( m_pEffect != NULL ) m_pEffect->OnResetDevice();

    m_pFont->RestoreDeviceObjects();	// ÆùÆ®
	
    return S_OK;
}




//-------------------------------------------------------------
// Name: FrameMove()
// Desc: ¸Å ÇÁ·¹ÀÓ¸¶´Ù È£ÃâµÊ. ¾Ö´Ï¸ÞÀÌ¼Ç Ã³¸®µî ´ã´ç
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
    UpdateInput( &m_UserInput );// “ü—Íƒf[ƒ^‚ÌXV

	//---------------------------------------------------------
	// ÀÔ·Â¿¡ µû¶ó ÁÂÇ¥°è¸¦ °»½ÅÇÑ´Ù
	//---------------------------------------------------------
	// È¸Àü
    D3DXMATRIX m;
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;

    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;

	//---------------------------------------------------------
	// Çà·Ä °»½Å
	//---------------------------------------------------------
	// ¿ùµå È¸Àü
    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );
    D3DXMatrixMultiply( &m, &matRotX, &matRotY );

    // ºäÇà·Ä
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 2.73f, -8.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 2.73f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFromPt, &vLookatPt, &vUpVec );
	m_mView = m * m_mView;
    m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_mView );

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
}




//-------------------------------------------------------------
// Name: Render()
// Desc: È­¸é ·»´õ
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
	D3DXMATRIX m, mW, mS, mR, mT;
	D3DXVECTOR4 v;

	
	// È­¸éÅ¬¸®¾î
    m_pd3dDevice->Clear( 0L, NULL
			, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL
			//   »ö     ±íÀÌ  ½ºÅÙ½Ç
			, 0xffffff, 1.0f, 0L );

    // ·»´õ ½ÃÀÛ
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) ) {

		// ----------------------------------------------------
		// ÁØºñ:±×¸²ÀÚ ¾ø´ÂºÎºÐ ·»´õ
		// ----------------------------------------------------
		D3DXMatrixIdentity( &m );
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m );
		m_pMeshBG->Render( m_pd3dDevice );

		// ÀÛÀº »óÀÚ
		D3DXMatrixScaling( &mS, 1.82f,1.65f, 1.82f );
		D3DXMatrixRotationY( &mR, 0.59f*D3DX_PI );
		D3DXMatrixTranslation( &mT, 2.73f-1.85f, 0.f , 1.69f );
		m = mS * mR * mT;
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m );
		m_pMeshBox->Render( m_pd3dDevice );

		// Å« »óÀÚ
		D3DXMatrixScaling( &mS, 1.69f, 3.30f, 1.69f );
		D3DXMatrixRotationY( &mR, 0.91f*D3DX_PI );
		D3DXMatrixTranslation( &mT, 2.73f-3.685f, 0, 3.51f );
		m = mS * mR * mT;
		m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m );
		m_pMeshBox->Render( m_pd3dDevice );

		// ----------------------------------------------------
		// ÆÐ½º2:±×¸²ÀÚ º¼·ý ·»´õ
		// ----------------------------------------------------
		// ±íÀÌ¹öÆÛ¿¡ ¾²±â±ÝÁö
		RS( D3DRS_ZWRITEENABLE,  FALSE );
		// ·»´õ¸µÅ¸°Ù¿¡ ¾²±â±ÝÁö
		RS( D3DRS_COLORWRITEENABLE,  FALSE );
		// ÇÃ·§¼ÎÀÌµù
		RS( D3DRS_SHADEMODE,	 D3DSHADE_FLAT );
		// ¾ç¸é·»´õ
		RS( D3DRS_CULLMODE,  D3DCULL_NONE );

		// ¾ç¸é½ºÅÙ½Ç »ç¿ë
		RS( D3DRS_STENCILENABLE, TRUE );
		RS( D3DRS_TWOSIDEDSTENCILMODE, TRUE );

		// ½ºÅÙ½ÇÅ×½ºÆ®´Â ±âº»ÀûÀ¸·Î ÇÕ°Ý(Å×½ºÆ® ÇÏÁö ¾ÊÀ½)
		RS( D3DRS_STENCILFUNC,  D3DCMP_ALWAYS );
		RS( D3DRS_CCW_STENCILFUNC,  D3DCMP_ALWAYS );
		// ½ºÅÙ½Ç¹öÆÛÀÇ Áõ°¨°ªÀ» 1·Î ¼³Á¤
		RS( D3DRS_STENCILREF,	   0x1 );
		RS( D3DRS_STENCILMASK,	  0xffffffff );
		RS( D3DRS_STENCILWRITEMASK, 0xffffffff );
		// ¾Õ¸éÀÌ ±íÀÌÅ×½ºÆ®¿¡ ÇÕ°ÝÇÏ¸é ½ºÅÙ½Ç¹öÆÛÀÇ ³»¿ëÀ» +1
		RS( D3DRS_STENCILPASS,  D3DSTENCILOP_INCR );
		RS( D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP );
		RS( D3DRS_STENCILFAIL,  D3DSTENCILOP_KEEP );
		// µÞ¸éÀÌ ±íÀÌÅ×½ºÆ®¿¡ ÇÕ°ÝÇÏ¸é ½ºÅÙ½Ç¹öÆÛÀÇ ³»¿ëÀ» -1
		RS( D3DRS_CCW_STENCILPASS, D3DSTENCILOP_DECR );
		RS( D3DRS_CCW_STENCILZFAIL, D3DSTENCILOP_KEEP );
		RS( D3DRS_CCW_STENCILFAIL,  D3DSTENCILOP_KEEP );

		// ·»´õÇÑ´Ù
		if( m_pEffect != NULL ){
			D3DXHANDLE hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
			m_pEffect->SetTechnique( hTechnique );
			m_pEffect->Begin( NULL, 0 );
			m_pEffect->Pass( 0 );

			// ÀÛÀº »óÀÚ
			D3DXMatrixScaling( &mS, 1.82f,1.65f, 1.82f );
			D3DXMatrixRotationY( &mR, 0.59f*D3DX_PI );
			D3DXMatrixTranslation( &mT, 2.73f-1.85f, 0.f , 1.69f );
			mW = mS * mR * mT;
			m = mW * m_mView * m_mProj;
			if( m_hmWVP != NULL ) m_pEffect->SetMatrix( m_hmWVP, &m );
			D3DXMatrixInverse( &m, NULL, &mW);
			D3DXVec3Transform( &v, &m_LighPos, &m );
			if( m_hvPos != NULL ) m_pEffect->SetVector( m_hvPos, &v );
			m_pShadowBox->Render( m_pd3dDevice );

			// Å« »óÀÚ
			D3DXMatrixScaling( &mS, 1.69f, 3.30f, 1.69f );
			D3DXMatrixRotationY( &mR, 0.91f*D3DX_PI );
			D3DXMatrixTranslation( &mT, 2.73f-3.685f, 0, 3.51f );
			mW = mS * mR * mT;
			m = mW * m_mView * m_mProj;
			if( m_hmWVP != NULL ) m_pEffect->SetMatrix( m_hmWVP, &m );
			D3DXMatrixInverse( &m, NULL, &mW);
			D3DXVec3Transform( &v, &m_LighPos, &m );
			if( m_hvPos != NULL ) m_pEffect->SetVector( m_hvPos, &v );
			m_pShadowBox->Render( m_pd3dDevice );

			m_pEffect->End();
		}

		// ·»´õ»óÅÂ ¿ø»óº¹±¸
		RS( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
		RS( D3DRS_CULLMODE,  D3DCULL_CCW );
		RS( D3DRS_ZWRITEENABLE,	 TRUE );
		RS( D3DRS_COLORWRITEENABLE,  0xf );
		RS( D3DRS_STENCILENABLE,	FALSE );
		RS( D3DRS_ALPHABLENDENABLE, FALSE );
		RS( D3DRS_TWOSIDEDSTENCILMODE, FALSE );

		// ----------------------------------------------------
		// ÆÐ½º3:±×¸²ÀÚ ·»´õ
		// ----------------------------------------------------
		// ±íÀÌÅ×½ºÆ® ±ÝÁö
		RS( D3DRS_ZENABLE,		  FALSE );
		// ½ºÅÙ½ÇÅ×½ºÆ® »ç¿ë
		RS( D3DRS_STENCILENABLE,	TRUE );
		// ¾ËÆÄºí·»µùÀº ¼±ÇüÇÕ¼º
		RS( D3DRS_ALPHABLENDENABLE, TRUE );
		RS( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		RS( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
		// Æú¸®°ïÀ» ·»´õ¸µÇÒ¶§ ÅØ½ºÃ³¿Í Á¤Á¡»öÀ» ¸ðµÎ »ç¿ë
		TSS( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
		TSS( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );
		TSS( 0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1 );
		TSS( 0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE );



		// ½ºÅÙ½Ç¹öÆÛÀÇ °ªÀÌ 1ÀÌ»óÀÎ °æ¿ì¿¡¸¸ ±×¸°´Ù
		RS( D3DRS_STENCILREF,  0x1 );
		RS( D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL );
		RS( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
		
		m_pBigSquare->Render( m_pd3dDevice );

		// ·»´õ»óÅÂ ¿ø»óº¹±¸
		RS( D3DRS_ZENABLE,		  TRUE );
		RS( D3DRS_STENCILENABLE,	FALSE );
		RS( D3DRS_ALPHABLENDENABLE, FALSE );

		RenderText();				// µµ¿ò¸» Ãâ·Â

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

    // µð½ºÇÃ·¹ÀÌÀÇ »óÅÂÃâ·Â
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    // Á¶ÀÛ¹æ¹ý
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    wsprintf( szMsg, TEXT("Arrow keys: Up=%d Down=%d Left=%d Right=%d"), 
              m_UserInput.bRotateUp, m_UserInput.bRotateDown, m_UserInput.bRotateLeft, m_UserInput.bRotateRight );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Use arrow keys to rotate object") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    return S_OK;
}




//-------------------------------------------------------------
// Name: MsgProc()
// Desc: WndProc ¿À¹ö¶óÀÌµù
//-------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT msg, WPARAM wParam,
                                    LPARAM lParam )
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
                wsprintf( strMsg, TEXT("Loading... Please wait") );
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
// Desc: RestoreDeviceObjects() ¿¡¼­ »ý¼ºÇÑ ¿ÀºêÁ§Æ® ÇØÁ¦
//-------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	// ÀÌÆåÆ®
	if( m_pEffect != NULL ) m_pEffect->OnLostDevice();
	// ¸Þ½Ã
	m_pMeshBG->InvalidateDeviceObjects();
	m_pMeshBox->InvalidateDeviceObjects();
	// ÆùÆ®
    m_pFont->InvalidateDeviceObjects();

	return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() ¿¡¼­ »ý¼ºÇÑ ¿ÀºêÁ§Æ® ÇØÁ¦
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	// ÀÌÆåÆ®
	SAFE_RELEASE( m_pEffect );
	// ¸Þ½Ã
	m_pMeshBG->Destroy();
	m_pMeshBox->Destroy();
	m_pShadowBox->Destroy();

	m_pBigSquare->Destroy();
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
	SAFE_DELETE( m_pShadowBox );
	SAFE_DELETE( m_pMeshBox );
	SAFE_DELETE( m_pMeshBG );
	SAFE_DELETE( m_pBigSquare );

    SAFE_DELETE( m_pFont );

    return S_OK;
}




