#include <d3d9.h>							// DirectX ‚ðŽg‚¦‚é‚æ‚¤‚É‚·‚é
#include "stdafx.h"
#include "draw.h"							 // Æú¸®°ï ·»´õÇÔ¼ö È£Ãâ

// ƒOƒ[ƒoƒ‹•Ï” :
LPDIRECT3D9				g_pD3D    = NULL;	// Direct 3D¿¡ Á¢±Ù
LPDIRECT3DDEVICE9		g_pD3DDev = NULL;	// ºñµð¿ÀÄ«µå¿¡ Á¢±Ù

//-----------------------------------------------------------------------------
// Direct X Graphics ÃÊ±âÈ­
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Direct 3D »ç¿ë
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) ) return E_FAIL;

    // µð½ºÇÃ·¹ÀÌ ¸ðµå Á¶»ç
    D3DDISPLAYMODE d3ddm;
    if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
		return E_FAIL;

    // Direct 3D µð¹ÙÀÌ½º »ý¼º
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );        // ¸ù¶¥ 0À¸·Î
    d3dpp.Windowed = TRUE;                      // À©µµ¿ì¸ðµå
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;   // È­¸éÀüÈ¯ ¹æ¹ý
    d3dpp.BackBufferFormat = d3ddm.Format;      // ÇöÀç È­¸é Æ÷¸Ë »ç¿ë

    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pD3DDev ) ) ) return E_FAIL;
	
	if(FAILED(Initialize(g_pD3DDev))) return E_FAIL; // ƒAƒvƒŠƒP[ƒVƒ‡ƒ“‚Ì‰Šú‰»

	return S_OK;
}
//-----------------------------------------------------------------------------
// ·»´õÇÔ¼ö
//-----------------------------------------------------------------------------
VOID Render()
{
    // ¹è°æÀ» °Ë°ÔÄ¥ÇÑ´Ù
    g_pD3DDev->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
    
    g_pD3DDev->BeginScene(); // ·»´õ ½ÃÀÛ
    
	Update(g_pD3DDev);			// ·»´õ
    
    g_pD3DDev->EndScene(); // ·»´õ Á¾·á
    
    // Ãâ·ÂÇÑ ³»¿ëÀ» ½ÇÁ¦ À©µµ¿ì¿¡ ³ªÅ¸³ª°Ô ÇÑ´Ù
    g_pD3DDev->Present( NULL, NULL, NULL, NULL );
}
//-----------------------------------------------------------------------------
// Œã•Ð•t‚¯
//-----------------------------------------------------------------------------
VOID Cleanup()
{
    Close(g_pD3DDev);			// ¾îÇÃ¸®ÄÉÀÌ¼Ç Á¾·á

	RELEASE(g_pD3DDev);
    RELEASE(g_pD3D);
}