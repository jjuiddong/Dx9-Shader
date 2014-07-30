//-------------------------------------------------------------
// File: main.cpp
//
// Desc: ±íÀÌ¹öÆÛ ±×¸²ÀÚ
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
// µð¹ö±×·Î Ãâ·ÂÇÒ ÅØ½ºÃ³¿ë ±¸Á¶Ã¼
//-------------------------------------------------------------
typedef struct {
    FLOAT       p[4];
    FLOAT       tu, tv;
} TVERTEX;

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
	m_pos						= D3DXVECTOR3( 1.5,1,2);

	m_pShadowTex				= NULL;
	m_pShadowSurf				= NULL;
	m_pZ						= NULL;

	m_pEffect					= NULL;
	m_hTechnique  				= NULL;
	m_hmWVP						= NULL;
	m_hmWLP						= NULL;
	m_hmWLPB					= NULL;
	m_hvCol						= NULL;
	m_hvDir						= NULL;
	m_htShadowMap				= NULL;
	m_pDecl						= NULL;

	m_fWorldRotX                = -0.65917611f;
    m_fWorldRotY                = 3.149265f;
	m_fViewZoom				    = 7.0f;
	m_LighPos					= D3DXVECTOR3( -5.0f, 5.0f,-3.0f );

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
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) )
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

	// UFOÀÐ±â
	if( FAILED( hr=m_pMesh  ->Create( m_pd3dDevice, _T("ufo.x"))))
        return DXTRACE_ERR( "LoadCar", hr );
	m_pMesh->SetFVF(m_pd3dDevice, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );
	// Áö¸éÀÐ±â
	if( FAILED( hr=m_pMeshBg->Create( m_pd3dDevice, _T("map.x"))))
        return DXTRACE_ERR( "LoadChess", hr );
        
    // ·»´õ¸µ½Ã ÅØ½ºÃ³¿Í ÀçÁú¼³Á¤À» ÇÏÁö ¾Ê´Â´Ù
	m_pMesh  ->UseMeshMaterials(FALSE);
	m_pMeshBg->UseMeshMaterials(FALSE);

	// ÆùÆ®
    m_pFont->InitDeviceObjects( m_pd3dDevice );

	// ¼ÎÀÌ´õ ÀÐ±â
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
	m_hmWLPB= m_pEffect->GetParameterByName( NULL, "mWLPB" );
	m_hvCol = m_pEffect->GetParameterByName( NULL, "vCol" );
	m_hvDir = m_pEffect->GetParameterByName( NULL, "vLightDir" );
	m_htShadowMap = m_pEffect->GetParameterByName( NULL, "ShadowMap" );

	// Á¤Á¡¼±¾ð¿ÀºêÁ§Æ® »ý¼º(ÁöÇü¿ë)
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
// Desc: È­¸éÅ©±â°¡ º¯ÇßÀ»¶§ È£ÃâµÊ
//       È®º¸ÇÑ ¸Þ¸ð¸®´Â InvalidateDeviceObjects()¿¡¼­ ÇØÁ¦
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	// ±×¸²ÀÚ¸Ê »ý¼º
	if (FAILED(m_pd3dDevice->CreateTexture(
						SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 1, 
						D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
						D3DPOOL_DEFAULT, &m_pShadowTex, NULL)))
		return E_FAIL;
	if (FAILED(m_pShadowTex->GetSurfaceLevel(0, &m_pShadowSurf)))
		return E_FAIL;
	if (FAILED(m_pd3dDevice->CreateDepthStencilSurface(
						SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 
						D3DFMT_D16, D3DMULTISAMPLE_NONE, 0,
						TRUE, &m_pZ, NULL)))
		return E_FAIL;

	// ¸Þ½Ã
	m_pMesh  ->RestoreDeviceObjects( m_pd3dDevice );
	m_pMeshBg->RestoreDeviceObjects( m_pd3dDevice );

    // ÀçÁú¼³Á¤
    D3DMATERIAL9 mtrl;
    D3DUtil_InitMaterial( mtrl, 1.0f, 0.0f, 0.0f );
    m_pd3dDevice->SetMaterial( &mtrl );

	// ´ÜÃà¸ÅÅ©·Î
	#define RS   m_pd3dDevice->SetRenderState
	#define TSS  m_pd3dDevice->SetTextureStageState
	#define SAMP m_pd3dDevice->SetSamplerState

    // ·»´õ¸µ »óÅÂ¼³Á¤
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

	m_pEffect->OnResetDevice();

	return S_OK;
}




//-------------------------------------------------------------
// Name: FrameMove()
// Desc: ¸Å ÇÁ·¹ÀÓ¸¶´Ù È£ÃâµÊ. ¾Ö´Ï¸ÞÀÌ¼Ç Ã³¸®µî ´ã´ç
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	// UFO¸¦ ¿òÁ÷ÀÎ´Ù
	m_pos.x = 1.5f*(FLOAT)cos(1.0f*this->m_fTime)+1.0f;
	m_pos.z = 1.5f*(FLOAT)sin(1.0f*this->m_fTime);
	m_pos.y = 1.3f;

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

    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -m_fViewZoom );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFromPt, &vLookatPt, &vUpVec );

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
}

//-------------------------------------------------------------
// °¢°¢ÀÇ ¸ðµ¨ ·»´õ
//-------------------------------------------------------------
VOID CMyD3DApplication::DrawModel( int pass )
{
    D3DXMATRIX m, mW, mS, mT, mR;
	D3DXVECTOR3 vDir;
	D3DXVECTOR4 v;
	D3DMATERIAL9 *pMtrl;
	DWORD i;
	
	//---------------------------------------------------------
	// Çà·Ä »ý¼º
	//---------------------------------------------------------
	// ·ÎÄÃ-Åõ¿µ Çà·Ä
	D3DXMATRIX mVP = m_mWorld * m_mView * m_mProj;
	
	// Åõ¿µ°ø°£¿¡¼­ ÅØ½ºÃ³°ø°£À¸·Î º¯È¯
	float fOffsetX = 0.5f + (0.5f / (float)SHADOW_MAP_SIZE);
	float fOffsetY = 0.5f + (0.5f / (float)SHADOW_MAP_SIZE);
	D3DXMATRIX mScaleBias(	0.5f,     0.0f,     0.0f,   0.0f,
							0.0f,    -0.5f,     0.0f,   0.0f,
							0.0f,     0.0f,     0.0f,	0.0f,
							fOffsetX, fOffsetY, 0.0f,   1.0f );

	//---------------------------------------------------------
	// UFO
	//---------------------------------------------------------
	// ¿ùµåÇà·Ä »ý¼º
	D3DXMatrixTranslation( &mW, m_pos.x, m_pos.y, m_pos.z );
	
	switch(pass){
	case 0:// ±×¸²ÀÚ¸Ê »ý¼º
		// ±¤¿ø°ø°£À¸·ÎÀÇ Åõ¿µÇà·Ä
		m = mW * m_mLightVP;
        m_pEffect->SetMatrix( m_hmWLP, &m );
        
		m_pMesh  ->Render( m_pd3dDevice );// ·»´õ
		break;
	default:// Àå¸é·»´õ
		m_pEffect->Pass( 2 );	// ÆÐ½º(2) ¼³Á¤
		m = mW * mVP;       // ÀÏ¹ÝÅõ¿µÇà·Ä
        m_pEffect->SetMatrix( m_hmWVP, &m );
        
		m = mW * m_mLightVP;// ±¤¿ø°ø°£À¸·ÎÀÇ Åõ¿µÇà·Ä
        m_pEffect->SetMatrix( m_hmWLP, &m );
        
		m = m * mScaleBias; // ÅØ½ºÃ³°ø°£À¸·ÎÀÇ Åõ¿µÇà·Ä
        m_pEffect->SetMatrix( m_hmWLPB, &m );
        
        // ·ÎÄÃÁÂÇ¥°è¿¡¼­ÀÇ ±¤¿øº¤ÅÍ
		D3DXMatrixInverse( &m, NULL, &mW);
		D3DXVec3Transform( &v, &m_LighPos, &m );
		D3DXVec4Normalize( &v, &v );v.w = 0;
		m_pEffect->SetVector( m_hvDir, &v );

		// ¸Þ½Ã³»ºÎ °¢°¢ÀÇ ºÎÇ°À» ·»´õ
		pMtrl = m_pMesh->m_pMaterials;
        for( i=0; i<m_pMesh->m_dwNumMaterials; i++ ) {
			v.x = pMtrl->Diffuse.r;
			v.y = pMtrl->Diffuse.g;
			v.z = pMtrl->Diffuse.b;
			v.w = pMtrl->Diffuse.a;
			m_pEffect->SetVector( m_hvCol, &v );        // Á¤Á¡»ö
            m_pMesh->m_pLocalMesh->DrawSubset( i );	// ·»´õ
			pMtrl++;
        }
		break;
	}

	//---------------------------------------------------------
	// ÁöÇü
	//---------------------------------------------------------
	// ¿ùµåÇà·Ä »ý¼º
	D3DXMatrixIdentity( &mW );
	switch(pass){
	case 0:// ±×¸²ÀÚ¸Ê »ý¼º
		// ±¤¿ø°ø°£À¸·ÎÀÇ Åõ¿µÇà·Ä
		m = mW * m_mLightVP;
        m_pEffect->SetMatrix( m_hmWLP, &m );
		m_pMeshBg->Render( m_pd3dDevice );// ·»´õ
		break;
	case 1:// Àå¸é·»´õ
		m_pEffect->Pass( 1 );	// ÆÐ½º(1) ¼³Á¤
		m = mW * mVP;       // ÀÏ¹Ý Åõ¿µÇà·Ä
        m_pEffect->SetMatrix( m_hmWVP, &m );
        
		m = mW * m_mLightVP;// ±¤¿ø°ø°£À¸·ÎÀÇ Åõ¿µÇà·Ä
        m_pEffect->SetMatrix( m_hmWLP, &m );
		
		m = m * mScaleBias; // ÅØ½ºÃ³°ø°£À¸·ÎÀÇ Åõ¿µÇà·Ä
        m_pEffect->SetMatrix( m_hmWLPB, &m );
        
        // ·ÎÄÃÁÂÇ¥°è¿¡¼­ÀÇ ±¤¿øº¤ÅÍ
		D3DXMatrixInverse( &m, NULL, &mW);
		D3DXVec3Transform( &v, &m_LighPos, &m );
		D3DXVec4Normalize( &v, &v );v.w = 0;
		m_pEffect->SetVector( m_hvDir, &v );
		
		// ¸Þ½Ã ³»ºÎÀÇ °¢°¢ÀÇ ºÎºÐÀ» ·»´õ
		pMtrl = m_pMeshBg->m_pMaterials;
        for( i=0; i<m_pMeshBg->m_dwNumMaterials; i++ ) {
			v.x = pMtrl->Diffuse.r;
			v.y = pMtrl->Diffuse.g;
			v.z = pMtrl->Diffuse.b;
			v.w = pMtrl->Diffuse.a;
			m_pEffect->SetVector( m_hvCol, &v );       // Á¤Á¡»ö
			m_pEffect->SetTexture("DecaleMap", m_pMeshBg->m_pTextures[i] );
            m_pMeshBg->m_pLocalMesh->DrawSubset(i); // ·»´õ
			pMtrl++;
        }
		break;
	}

}


//-------------------------------------------------------------
// Name: Render()
// Desc: È­¸é ·»´õ
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    D3DXMATRIX mLP, mView, mProj;
	LPDIRECT3DSURFACE9 pOldBackBuffer, pOldZBuffer;
	D3DVIEWPORT9 oldViewport;

	//---------------------------------------------------------
	// Çà·Ä»ý¼º
	//---------------------------------------------------------
    // ±¤¿ø¹æÇâ¿¡¼­ º» Åõ¿µ°ø°£¿¡ÀÇ Çà·Ä
	D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp       = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &mView, &m_LighPos, &vLookatPt, &vUp );
    
    D3DXMatrixPerspectiveFovLH( &mProj
							, 0.25f*D3DX_PI		// ½Ã¾ß°¢
							, 1.0f				// Á¾È¾ºñ
							, 5.0f, 12.0f );	// near far
	m_mLightVP = mView * mProj;

	//---------------------------------------------------------
	// ·»´õ
	//---------------------------------------------------------
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		//-------------------------------------------------
		// ·»´õ¸µÅ¸°Ù º¸Á¸
		//-------------------------------------------------
		m_pd3dDevice->GetRenderTarget(0, &pOldBackBuffer);
		m_pd3dDevice->GetDepthStencilSurface(&pOldZBuffer);
		m_pd3dDevice->GetViewport(&oldViewport);

		//-------------------------------------------------
		// ·»´õ¸µÅ¸°Ù º¯°æ
		//-------------------------------------------------
		m_pd3dDevice->SetRenderTarget(0, m_pShadowSurf);
		m_pd3dDevice->SetDepthStencilSurface(m_pZ);
		// ºäÆ÷Æ® º¯°æ
		D3DVIEWPORT9 viewport = {0,0      // ÁÂÃø»ó´ÜÁÂÇ¥
						, SHADOW_MAP_SIZE // Æø
						, SHADOW_MAP_SIZE // ³ôÀÌ
						, 0.0f,1.0f};     // Àü¸é ÈÄ¸é
		m_pd3dDevice->SetViewport(&viewport);

		// ±×¸²ÀÚ¸Ê Å¬¸®¾î
		m_pd3dDevice->Clear(0L, NULL
						, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
						, 0xFFFFFFFF, 1.0f, 0L);

		if( m_pEffect != NULL ) 
		{
			//-------------------------------------------------
			// ¼ÎÀÌ´õ ¼³Á¤
			//-------------------------------------------------
			m_pEffect->SetTechnique( m_hTechnique );
			m_pEffect->Begin( NULL, 0 );

			//-------------------------------------------------
			// 1ÆÐ½º:±×¸²ÀÚ¸Ê »ý¼º
			//-------------------------------------------------
			m_pEffect->Pass( 0 );	// ÆÐ½º(0)¼³Á¤
			DrawModel( 0 );			// ¸ðµ¨ ·»´õ

			//-------------------------------------------------
			// ·»´õ¸µÅ¸°Ù º¹±¸
			//-------------------------------------------------
			m_pd3dDevice->SetRenderTarget(0, pOldBackBuffer);
			m_pd3dDevice->SetDepthStencilSurface(pOldZBuffer);
			m_pd3dDevice->SetViewport(&oldViewport);
			pOldBackBuffer->Release();
			pOldZBuffer->Release();

			//-------------------------------------------------
			// 2ÆÐ½º:Àå¸é·»´õ
			//-------------------------------------------------
			// ¹öÆÛÅ¬¸®¾î
			m_pd3dDevice->Clear( 0L, NULL
							, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER
							, 0x00404080, 1.0f, 0L );

			//-------------------------------------------------
			// ·»´õ
			//-------------------------------------------------
			// ÅØ½ºÃ³ ¼³Á¤
			m_pEffect->SetTexture(m_htShadowMap, m_pShadowTex);
			m_pd3dDevice->SetVertexDeclaration( m_pDecl );
			DrawModel( 1 );			// ¸ðµ¨ ·»´õ

			m_pEffect->End();
		}

		RenderText();				// µµ¿ò¸» Ãâ·Â

#if 1 // µð¹ö±×¿ë ÅØ½ºÃ³ Ãâ·Â
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
		m_pd3dDevice->SetTexture( 0, m_pShadowTex );
		m_pd3dDevice->SetVertexShader(NULL);
		m_pd3dDevice->SetFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
		m_pd3dDevice->SetPixelShader(0);
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN, 2, Vertex, sizeof( TVERTEX ) );
		}
#endif		

        m_pd3dDevice->EndScene();	// ·»´õÁ¾·á
    }

    return S_OK;
}




//-------------------------------------------------------------
// Name: RenderText()
// Desc: ó‘Ô‚âƒwƒ‹ƒv‚ð‰æ–Ê‚É•\Ž¦‚·‚é
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    FLOAT fNextLine = 40.0f; // Ãâ·Â³ôÀÌ

    // Á¶ÀÛ¹æ¹ý
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
	// ±×¸²ÀÚ ÅØ½ºÃ³
	SAFE_RELEASE(m_pShadowSurf);
	SAFE_RELEASE(m_pShadowTex);
	SAFE_RELEASE(m_pZ);

	m_pMesh  ->InvalidateDeviceObjects(); // ¸Þ½Ã
	m_pMeshBg->InvalidateDeviceObjects();

    m_pFont->InvalidateDeviceObjects();	// ÆùÆ®

	// ¼ÎÀÌ´õ
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
	SAFE_RELEASE( m_pDecl );		// Á¤Á¡¼±¾ð

	// ¸Þ½Ã
	m_pMesh->Destroy();
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

