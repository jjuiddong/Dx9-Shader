//-------------------------------------------------------------
// File: main.cpp
//
// Desc: º¯À§¸ÊÇÎ
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
// Àü¿ªº¯¼ö
//-------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;


//-------------------------------------------------------------
// Á¤Á¡¼±¾ð
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
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps
						, DWORD dwBehavior, D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
	// ¼ÎÀÌ´õ Ã¼Å©
	if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) &&
	  !(dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) )
		return E_FAIL;	// Á¤Á¡¼ÎÀÌ´õ

	if( pCaps->PixelShaderVersion  < D3DPS_VERSION(1,1) )
		return E_FAIL;	// ÇÈ¼¿¼ÎÀÌ´õ
	
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
    D3DFORMAT adptFormat = m_d3dSettings.DisplayMode().Format;

	// º¯À§¸ÊÇÎÀÌ °¡´ÉÇÑ Á¶°ÇµéÀ» È®ÀÎ
    m_bDMap = 
		// ÇÏµå¿þ¾î Á¤Á¡Ã³¸®
		(m_pd3dDevice->GetSoftwareVertexProcessing()==FALSE) && 
		// º¯À§¸ÊÀ» Áö¿øÇÏ´Â°¡ Ã¼Å©
        (m_d3dCaps.DevCaps2 & D3DDEVCAPS2_DMAPNPATCH ) &&
		// ÅØ½ºÃ³Æ÷¸Ë È®ÀÎ
        SUCCEEDED( m_pD3D->CheckDeviceFormat(
								m_d3dCaps.AdapterOrdinal,
                                m_d3dCaps.DeviceType,
                                adptFormat,
                                D3DUSAGE_DMAP,
                                D3DRTYPE_TEXTURE,
                                D3DFMT_L8 ) );

    // ¸Þ½Ã ÀÐ±â
	if( FAILED( hr=m_pMesh->Create( m_pd3dDevice, "earth.x" )))
        return DXTRACE_ERR( "Load Mesh", hr );

	
	// ¼ÎÀÌ´õ ÀÐ±â
    if( FAILED( hr = D3DXCreateEffectFromFile(
						m_pd3dDevice, "hlsl.fx", NULL, NULL, 
						0, NULL, &m_pEffect, NULL ) ) )
		return DXTRACE_ERR( "CreateEffectFromFile", hr );

	if(m_bDMap){
		// Á¤Á¡¼±¾ð ¿ÀºêÁ§Æ® »ý¼º
		if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration(
											decl, &m_pDecl )))
			return DXTRACE_ERR ("CreateVertexDeclaration", hr);
		
		// º¯À§¸Ê ÀÐ±â
		if(FAILED(hr = D3DXCreateTextureFromFileEx(
								  m_pd3dDevice		// µð¹ÙÀÌ½º
								, "earthbump.bmp"	// ÆÄÀÏ¸í
								, D3DX_DEFAULT		// Æø
								, D3DX_DEFAULT		// ³ôÀÌ
								, D3DX_DEFAULT		// ¹Ó ·¹º§
								, D3DUSAGE_DMAP		// »ç¿ë¹ý
								, D3DFMT_L8			// Æ÷¸Ë
								, D3DPOOL_MANAGED	// ¸Þ¸ð¸®Å¬·¡½º
								, D3DX_DEFAULT		// ÇÊÅÍ¸µ
								, D3DX_DEFAULT		// ÇÊÅÍ¸µ
								, 0					// Åõ¸í°ª
								, NULL				// ¼Ò½ºÀÌ¹ÌÁö
								, NULL				// 256»ö ÆÈ·¹Æ®
								, &m_pDispMap
								)))
			return DXTRACE_ERR( "Load Texture", hr );

		// »ùÇÃ·¯ ½ºÅ×ÀÌÆ® ¼³Á¤
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

    m_pFont->InitDeviceObjects( m_pd3dDevice );// ÆùÆ®
    
	return S_OK;
}

//-------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: È­¸éÅ©±â°¡ º¯ÇßÀ»¶§ È£ÃâµÊ
//       È®º¸ÇÑ ¸Þ¸ð¸®´Â InvalidateDeviceObjects()¿¡¼­ ÇØÁ¦
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	// ´ÜÃà¸ÅÅ©·Î
	#define RS   m_pd3dDevice->SetRenderState
	#define TSS  m_pd3dDevice->SetTextureStageState
	#define SAMP m_pd3dDevice->SetSamplerState

    // ·»´õ¸µ »óÅÂ¼³Á¤
    RS  ( D3DRS_ZENABLE,        TRUE );
    RS  ( D3DRS_LIGHTING,       FALSE );

    // ÅØ½ºÃ³ »óÅÂ¼³Á¤
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


    // ¿ùµåÇà·Ä
    D3DXMatrixIdentity( &m_mWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m_mWorld );

	// ºäÇà·Ä
    D3DXVECTOR3 vFrom   = D3DXVECTOR3( 0.0f, 0.0f, -3.5f );
    D3DXVECTOR3 vLookat = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFrom, &vLookat, &vUp );

    // Åõ¿µÇà·Ä
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width )
				  / ((FLOAT)m_d3dsdBackBuffer.Height);
    D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect
								, 0.1f, 10.0f );

	// ±¤¿øÀ§Ä¡ ¼³Á¤
	m_LighPos = D3DXVECTOR4(-0.6f, 0.6f, -0.6f, 0.3f);

	
	if(m_bDMap){
		//-----------------------------------------------------
		// º¯À§¸ÊÀ» »ç¿ëÇÒ¶§´Â Á÷Á¢Ã³¸®
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
	if( m_pEffect!=NULL ) m_pEffect->OnResetDevice();// ¼ÎÀÌ´õ

    m_pFont->RestoreDeviceObjects();	// ÆùÆ®

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
    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;
	// ÁÜ
	if(m_UserInput.bZ && !m_UserInput.bX)
		m_degree += 0.01f;
	else if(m_UserInput.bX && !m_UserInput.bZ)
		m_degree -= 0.01f;

	// Çà·Ä°»½Å
 	D3DXMATRIX matRotX, matRotY;
    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );
    D3DXMatrixMultiply( &m_mWorld, &matRotY, &matRotX );

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
	pUserInput->bZ = ( m_bActive && (GetAsyncKeyState( 'Z' ) & 0x8000) == 0x8000 );
	pUserInput->bX = ( m_bActive && (GetAsyncKeyState( 'X' ) & 0x8000) == 0x8000 );
}




//-------------------------------------------------------------
// Name: Render()
// Desc: È­¸é ·»´õ
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    D3DXHANDLE hTechnique;
	D3DXMATRIX m;
	D3DXVECTOR4 v;

	// È­¸é Å¬¸®¾î
    m_pd3dDevice->Clear( 0L, NULL,
						D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						0x000000ff, 1.0f, 0L );

    if( SUCCEEDED( m_pd3dDevice->BeginScene()))	// ·»´õ ½ÃÀÛ
    {
		if(m_bDMap && m_pEffect != NULL)
		{	// º¯À§¸ÊÇÎ ÇÑ´Ù
			//-------------------------------------------------
			// ¼ÎÀÌ´õ ¼³Á¤
			//-------------------------------------------------
			hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
			m_pEffect->SetTechnique( hTechnique );
			m_pEffect->Begin( NULL, 0 );
			m_pEffect->Pass( 0 );
			
			//-------------------------------------------------
			// ¼ÎÀÌ´õ »ó¼ö ¼³Á¤
			//-------------------------------------------------
			// ÁÂÇ¥º¯È¯
			m = m_mWorld * m_mView * m_mProj;
		    D3DXMatrixTranspose( &m, &m );
			m_pd3dDevice->SetVertexShaderConstantF( 0,(float*)&m, 4);
			// º¯À§ÀÇ Å©±â
			v = D3DXVECTOR4(m_degree,0.0f,0.0f,0.0f);
			m_pd3dDevice->SetVertexShaderConstantF( 4,(float*)&v, 1);
			// Àû´çÇÑ »ó¼ö
			v = D3DXVECTOR4(0.0f,0.5f,1.0f,2.0f);
			m_pd3dDevice->SetVertexShaderConstantF(12,(float*)&v, 1);
			// ±¤¿ø
			D3DXMatrixInverse( &m, NULL, &m_mWorld);
			D3DXVec4Transform( &v, &m_LighPos, &m );
			D3DXVec4Normalize( &v, &v );v.w = 0.3f;
			m_pd3dDevice->SetVertexShaderConstantF(13,(float*)&v,1 );


			//-------------------------------------------------
			// º¯À§¸Ê ¼³Á¤
			//-------------------------------------------------
			m_pd3dDevice->SetTexture(D3DDMAPSAMPLER, m_pDispMap);
			m_pd3dDevice->SetNPatchMode(4);// Nƒpƒbƒ`‚Ìƒ‚[ƒh
			m_pd3dDevice->SetVertexDeclaration( m_pDecl );

			//-------------------------------------------------
			// ·»´õ
			//-------------------------------------------------
			m_pMesh->Render( m_pd3dDevice );

			//-------------------------------------------------
			// ·»´õÁ¾·á
			//-------------------------------------------------
			m_pd3dDevice->SetNPatchMode(0);
			m_pd3dDevice->SetVertexShader( NULL );
			m_pEffect->End();
		}else{
			// º¯À§¸ÊÇÎ ÇÏÁö ¾ÊÀ½
		    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_mWorld );
			m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_mView );
			m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );

			m_pd3dDevice->SetTexture(0,m_pMesh->m_pTextures[0]);
			m_pMesh->Render( m_pd3dDevice );
		}

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

    FLOAT fNextLine = 40.0f; 

    // µð½ºÇÃ·¹ÀÌ »óÅÂ Ãâ·Â
    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    // Á¶ÀÛ¹ý & ÀÎ¼ö Ãâ·Â
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
// Desc: WndProc ¿À¹ö¶óÀÌµù
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
                // ·ÎµùÁß
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
// Desc: RestoreDeviceObjects() ¿¡¼­ »ý¼ºÇÑ ¿ÀºêÁ§Æ® ÇØÁ¦
//-------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	m_pMesh->InvalidateDeviceObjects();				// ¸Þ½Ã
    if(m_pEffect!=NULL) m_pEffect->OnLostDevice();	// ¼ÎÀÌ´õ

    m_pFont->InvalidateDeviceObjects();	// ÆùÆ®

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() ¿¡¼­ »ý¼ºÇÑ ¿ÀºêÁ§Æ® ÇØÁ¦
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	m_pMesh->Destroy();				// ¸Þ½Ã
	SAFE_RELEASE( m_pEffect );		// ¼ÎÀÌ´õ
	SAFE_RELEASE( m_pDecl );		// Á¤Á¡¼±¾ð
	SAFE_RELEASE( m_pDispMap );		// º¯À§¸Ê

    m_pFont->DeleteDeviceObjects();	// ÆùÆ®

	return S_OK;
}




//-------------------------------------------------------------
// Name: FinalCleanup()
// Desc: Á¾·áÁ÷Àü¿¡ È£ÃâµÊ
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    SAFE_DELETE( m_pMesh );	// ¸Þ½Ã

    SAFE_DELETE( m_pFont );	// ÆùÆ®

    return S_OK;
}




