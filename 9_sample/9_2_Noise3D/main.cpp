//-------------------------------------------------------------
// File: main.cpp
//
// Desc: 3D Perlin Noise
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
	m_pTex						= NULL;
	m_pWoodTex					= NULL;

	m_pEffect					= NULL;
	m_hTechnique  				= NULL;
	m_hmWVP  					= NULL;
	m_htTex		 				= NULL;

	m_fWorldRotX                = -D3DX_PI/10;
    m_fWorldRotY                = D3DX_PI/2;
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
    

	// ÇÈ¼¿¼ÎÀÌ´õ ¹öÀü Ã¼Å©
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) )
		return E_FAIL;

    // Á¤Á¡¼ÎÀÌ´õ ¹öÀüÀÌ ³·À¸¸é ¼ÒÇÁÆ®¿þ¾î Á¤Á¡Ã³¸®
    if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1)
    &&  0==(dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) )
			return E_FAIL;

    return S_OK;
}

//-------------------------------------------------------------
// ³ëÀÌÁî°¡ µé¾î°£ ÅØ½ºÃ³ »ý¼º
//-------------------------------------------------------------
VOID Create3DNoise( D3DXVECTOR4* pOut, CONST D3DXVECTOR3* pTexCoord, 
	CONST D3DXVECTOR3* pTexelSize, LPVOID pData )
{
	// 0`1 ‚Ì”ÍˆÍ‚Ì”’•‚ÌƒeƒNƒXƒ`ƒƒ
	pOut->x = pOut->y = pOut->z = pOut->w
		= (float)rand() / RAND_MAX;
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

	// ÁÖÀüÀÚ ÀÐ±â
	if(FAILED(hr=m_pMesh  ->Create( m_pd3dDevice, _T("t-pot.x"))))
        return DXTRACE_ERR( "Load Object", hr );
	m_pMesh->UseMeshMaterials(false);// ÅØ½ºÃ³´Â Á÷Á¢ Ã³¸®
    
	// ³­¼ö ÅØ½ºÃ³ »ý¼º
    LPDIRECT3DVOLUMETEXTURE9 pVolumeTex = NULL;
    if( SUCCEEDED( hr = D3DXCreateVolumeTexture( m_pd3dDevice
					, 32, 32, 32, 1, 0, D3DFMT_A8R8G8B8
					, D3DPOOL_MANAGED, &pVolumeTex) ) ) {
        if( SUCCEEDED( hr = D3DXFillVolumeTexture(
				pVolumeTex, (LPD3DXFILL3D)Create3DNoise, NULL ))){
            m_pTex = pVolumeTex;
		}
	}
	
	// ³ªÀÌÅ× ÅØ½ºÃ³ ÀÐ±â
	if(FAILED(hr=D3DXCreateTextureFromFile( m_pd3dDevice, "wood.bmp", &m_pWoodTex)))
        return DXTRACE_ERR( "Load Texture", hr );

	// ¼ÎÀÌ´õ ÀÐ±â
    LPD3DXBUFFER pErr=NULL;
    if( FAILED( hr = D3DXCreateEffectFromFile(
                m_pd3dDevice, "hlsl.fx", NULL, NULL, 
                0 , NULL, &m_pEffect, &pErr ))){
        // ¼ÎÀÌ´õ ÀÐ±â ½ÇÆÐ
        MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer()
                    , "ERROR", MB_OK);
    }else{
		m_hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
		m_hmWVP      = m_pEffect->GetParameterByName( NULL, "mWVP" );
		m_htTex      = m_pEffect->GetParameterByName( NULL, "Tex" );
		m_htWoodTex  = m_pEffect->GetParameterByName( NULL, "WoodTex" );
    }
    SAFE_RELEASE(pErr);

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
	// ÀÌÆåÆ®
	if(m_pEffect) m_pEffect->OnResetDevice();

	// ¸Þ½Ã
	m_pMesh->RestoreDeviceObjects( m_pd3dDevice );

	// ·»´õ¸µ»óÅÂ ¼³Á¤
    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE,        TRUE );
	m_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
    
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
	// ºäÇà·Ä ¼³Á¤
	//---------------------------------------------------------
	// ÁÜ
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
// Name: Render()
// Desc: È­¸é ·»´õ
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    D3DXMATRIX m, mT, mR, mView, mProj;
	D3DXMATRIX mWorld;
	D3DXVECTOR4 v, light_pos, eye_pos;
    
	//---------------------------------------------------------
	// ·»´õ
	//---------------------------------------------------------
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		// ·»´õ¸µÅ¸°Ù Å¬¸®¾î
		m_pd3dDevice->Clear(0L, NULL
						, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
						, 0x3B6EA3, 1.0f, 0L);

		// ÁÖÀüÀÚµîÀ» È­¸é¿¡ ·»´õ
		if( m_pEffect != NULL ) 
		{
			//-------------------------------------------------
			// ¼ÎÀÌ´õ ¼³Á¤
			//-------------------------------------------------
			m_pEffect->SetTechnique( m_hTechnique );
			m_pEffect->Begin( NULL, 0 );
			m_pEffect->Pass( 0 );
			
			//-------------------------------------------------
			// ÁÖÀüÀÚ ·»´õ
			//-------------------------------------------------

			// ¿ùµåÇà·Ä(È¸Àü)
//			D3DXMatrixRotationY( &mWorld,  m_fTime );
			D3DXMatrixRotationY( &mWorld,  -1.0 );// ‰ñ“]‚µ‚È‚¢

			// ·ÎÄÃ-Åõ¿µº¯È¯ Çà·Ä
			m = mWorld * m_mView * m_mProj;
			m_pEffect->SetMatrix( m_hmWVP, &m );
			// ÅØ½ºÃ³
			m_pEffect->SetTexture(m_htTex, m_pTex);
			m_pEffect->SetTexture(m_htWoodTex, m_pWoodTex);

			// ±¤¿ø¹æÇâ(·ÎÄÃÁÂÇ¥°è)
			light_pos = D3DXVECTOR4(  0.577f,  0.577f,  0.577f,0);
			D3DXMatrixInverse( &m, NULL, &mWorld);
			D3DXVec4Transform( &v, &light_pos, &m );
			D3DXVec3Normalize( (D3DXVECTOR3 *)&v, (D3DXVECTOR3 *)&v );
			v.w = 0.3f;		// ŠÂ‹«Œõ‚Ì‹­‚³
			m_pEffect->SetVector( "LightDir", &v );
			
			// ¾Ö´Ï¸ÞÀÌ¼ÇÀ» À§ÇÑ ½Ã°£
			m_pEffect->SetFloat( "time", fmod(-0.1f*m_fTime, 1) );
			
			m_pMesh->Render( m_pd3dDevice );  // ·»´õ

			m_pEffect->End();
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

    FLOAT fNextLine = 40.0f; // Ãâ·Â ³ôÀÌ

    // Á¶ÀÛ¹ý & ÀÎ¼ö Ãâ·Â
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
                // ·ÎµùÁß
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
	m_pMesh->InvalidateDeviceObjects(); // ¸Þ½Ã

    m_pFont->InvalidateDeviceObjects();	// ÆùÆ®

	// ƒVƒF[ƒ_
    if( m_pEffect ) m_pEffect->OnLostDevice();

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() ¿¡¼­ »ý¼ºÇÑ ¿ÀºêÁ§Æ® ÇØÁ¦
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	SAFE_RELEASE( m_pEffect );      // ¼ÎÀÌ´õ
	
	SAFE_RELEASE( m_pTex );      // ÅØ½ºÃ³
	SAFE_RELEASE( m_pWoodTex );

	// ¸Þ½Ã
	m_pMesh->Destroy();

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
	SAFE_DELETE( m_pMesh ); // ¸Þ½Ã

    SAFE_DELETE( m_pFont );	// ÆùÆ®

    return S_OK;
}




