//-------------------------------------------------------------
// File: main.cpp
//
// Desc: ÅØ½ºÃ³ ÀÔÈ÷±â
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
// Á¤Á¡Æ÷¸Ë
//-------------------------------------------------------------
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ)

typedef struct {
    FLOAT x, y, z;    // ½ºÅ©¸°ÁÂÇ¥¿¡¼­ À§Ä¡
} CUSTOMVERTEX;



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
    // ¡Ú ÇÈ¼¿¼ÎÀÌ´õ Å¬¸®¾î
    m_pPixelShader     = NULL;

    // Á¤Á¡¼ÎÀÌ´õ Å¬¸®¾î
    m_pVertexShader    = NULL;
    m_pVertexDeclaration = NULL;

    m_pVB                       = NULL;

    m_dwCreationWidth           = 300;
    m_dwCreationHeight          = 300;
    m_strWindowTitle            = TEXT( "main" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
    m_bStartFullscreen          = false;
    m_bShowCursorWhenFullscreen = false;

    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
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
    
    BOOL bCapsAcceptable;

    // ±×·¡ÇÈº¸µå°¡ ÇÁ·Î±×·¥À» ½ÇÇàÇÒ ´É·ÂÀÌ ÀÖ´Â°¡ È®ÀÎ
    bCapsAcceptable = TRUE;

	// ¡Ú ÇÈ¼¿¼ÎÀÌ´õ ¹öÀüÈ®ÀÎ
	if( pCaps->PixelShaderVersion < D3DPS_VERSION(1,1) )
	        bCapsAcceptable = FALSE;

    // Á¤Á¡¼ÎÀÌ´õ ¹öÀüÈ®ÀÎ
    if( (dwBehavior & D3DCREATE_HARDWARE_VERTEXPROCESSING ) ||
        (dwBehavior & D3DCREATE_MIXED_VERTEXPROCESSING ) )
    {
        if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) )
            bCapsAcceptable = FALSE;
    }

    if( bCapsAcceptable )         
        return S_OK;
    else
        return E_FAIL;
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
    LPD3DXBUFFER pCode;
    
    // ---------------------------------------------------------
	// ¡Ú ÇÈ¼¿¼ÎÀÌ´õ »ý¼º
    // ---------------------------------------------------------
    if( FAILED( hr = D3DXAssembleShaderFromFile(
						_T("PixelShader.psh"), 
						NULL, NULL, 0, &pCode, NULL ) ) ){
        return hr;
	}
    SAFE_RELEASE( m_pPixelShader );
    if (FAILED( hr = m_pd3dDevice->CreatePixelShader(
						(DWORD*)pCode->GetBufferPointer(),
						&m_pPixelShader ) ) )
        return hr;
    SAFE_RELEASE( pCode );

    // ---------------------------------------------------------
    // Á¤Á¡¼ÎÀÌ´õ »ý¼º
    // ---------------------------------------------------------

    // ’¸“_éŒ¾ƒIƒuƒWƒFƒNƒg‚Ì¶¬
    D3DVERTEXELEMENT9 decl[] = {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT,
                 D3DDECLUSAGE_POSITION, 0 },
        D3DDECL_END()
    };
    
    if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration( decl, &m_pVertexDeclaration ) ) ) {
        return hr;
    }

    // ¼ÎÀÌ´õ ÀÐ±â
    if( FAILED( hr = D3DXAssembleShaderFromFile(
                        _T("VertexShader.vsh"), NULL, NULL, 0, &pCode, NULL ))) {
        return hr;
    }

    // Á¤Á¡¼ÎÀÌ´õ »ý¼º
    hr = m_pd3dDevice->CreateVertexShader(
                          (DWORD*)pCode->GetBufferPointer(),
                          &m_pVertexShader );
    pCode->Release();
    if( FAILED(hr) ) return hr;

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
    // Á¤Á¡°ú »öÁ¤º¸
    CUSTOMVERTEX vertices[] = {
        //   x,     y,      z,  
        {-0.5f, +0.5f, 0},
        {+0.5f, +0.5f, 0},
        {-0.5f, -0.5f, 0},
        {+0.5f, -0.5f, 0},
    };
    
    // Á¤Á¡¹öÆÛ »ý¼º
    if( FAILED( m_pd3dDevice->CreateVertexBuffer( 
                4*sizeof(CUSTOMVERTEX),        // Á¤Á¡¹öÆÛ Å©±â
                0, D3DFVF_CUSTOMVERTEX,        // »ç¿ë¹ý, Á¤Á¡Æ÷¸Ë
                D3DPOOL_DEFAULT,            // ¸Þ¸ð¸® Å¬·¡½º
                &m_pVB, NULL )))            // Á¤Á¡¹öÆÛ ¸®¼Ò½º
        return E_FAIL;

    // Á¤Á¡¹öÆÛ¿¡ Á¤º¸ ÀúÀå
    VOID* pVertices;
    if(FAILED( m_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
        return E_FAIL;
    memcpy( pVertices, vertices, sizeof(vertices) );
    m_pVB->Unlock();

    // ´ÜÃà¸ÅÅ©·Î
    #define RS   m_pd3dDevice->SetRenderState
    #define TSS  m_pd3dDevice->SetTextureStageState
    #define SAMP m_pd3dDevice->SetSamplerState

    // ·»´õ¸µ »óÅÂ¼³Á¤
    RS( D3DRS_DITHERENABLE,   FALSE );
    RS( D3DRS_SPECULARENABLE, FALSE );
    RS( D3DRS_ZENABLE,        TRUE );
    RS( D3DRS_AMBIENT,        0x000F0F0F );
    
    TSS( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
    TSS( 0, D3DTSS_COLORARG1, D3DTA_DIFFUSE );

    // ÆùÆ®
    m_pFont->RestoreDeviceObjects();

    // ºäÇà·Ä ¼³Á¤
    D3DXVECTOR3 vEye = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
    D3DXVECTOR3 vAt  = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp  = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtRH( &m_matView, &vEye, &vAt, &vUp );

    // Åõ¿µÇà·Ä ¼³Á¤
    FLOAT fAspectRatio = (FLOAT)m_d3dsdBackBuffer.Width
                       / (FLOAT)m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovRH( &m_matProj, D3DXToRadian(60.0f),
                                fAspectRatio, 0.1f, 100.0f );

	return S_OK;
}




//-------------------------------------------------------------
// Name: FrameMove()
// Desc: ¸Å ÇÁ·¹ÀÓ¸¶´Ù È£ÃâµÊ. ¾Ö´Ï¸ÞÀÌ¼Ç Ã³¸®µî ´ã´ç
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
    D3DXMATRIXA16 mat;
    
    // ¡Ú ÁÂÇ¥º¯È¯¿ë »ó¼ö ·¹Áö½ºÅÍ ¼³Á¤
    D3DXMatrixMultiply( &mat, &m_matView, &m_matProj );
    D3DXMatrixTranspose( &mat, &mat );// ÀüÄ¡Çà·Ä·Î º¯È¯
    m_pd3dDevice->SetVertexShaderConstantF(0, (float*)&mat, 4);

    // ¡Ú Á¤Á¡»ö »ó¼ö ·¹Áö½ºÅÍ ¼³Á¤
    float color[] = {1.0f, 0.0f, 0.0f, 1.0f};// Àû»ö
    m_pd3dDevice->SetVertexShaderConstantF(4, (float*)&color, 1);

    return S_OK;
}




//-------------------------------------------------------------
// Name: Render()
// Desc: È­¸é ·»´õ
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    // È­¸é Å¬¸®¾î
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
                         0x000000ff, 1.0f, 0L );

    //---------------------------------------------------------
    // ·»´õ
    //---------------------------------------------------------
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		// ¡Ú ÇÈ¼¿¼ÎÀÌ´õ ¼³Á¤
		m_pd3dDevice->SetPixelShader( m_pPixelShader );

        // Á¤Á¡¼Î¿¡´õ ¼³Á¤
        m_pd3dDevice->SetVertexDeclaration(m_pVertexDeclaration);
        m_pd3dDevice->SetVertexShader( m_pVertexShader );
		
        // Æú¸®°ï ·»´õ
        m_pd3dDevice->SetStreamSource( 0, m_pVB, 0,
                                       sizeof(CUSTOMVERTEX) );
        m_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );

        RenderText();    // È­¸é»óÅÂ¿Í µµ¿ò¸» Ãâ·Â

        // ·»´õÁ¾·á
        m_pd3dDevice->EndScene();
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


    // È­¸é»óÅÂ
    FLOAT fNextLine = 40.0f; 

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    // Á¶ÀÛ¹ý & ÆÄ¶ó¸ÞÅÍ Ãâ·Â
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
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
    SAFE_RELEASE( m_pVB );        // Á¤Á¡¹öÆÛ

    m_pFont->InvalidateDeviceObjects();    // ÆùÆ®

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() ¿¡¼­ »ý¼ºÇÑ ¿ÀºêÁ§Æ® ÇØÁ¦
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
    // ¡Ú ÇÈ¼¿¼ÎÀÌ´õ ÇØÁ¦
	SAFE_RELEASE( m_pPixelShader );

    // ¡Ú Á¤Á¡¼ÎÀÌ´õ ÇØÁ¦
	SAFE_RELEASE( m_pVertexShader );
    SAFE_RELEASE( m_pVertexDeclaration );

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
    SAFE_DELETE( m_pFont );    // ÆùÆ®

    return S_OK;
}




