#include <d3d9.h>                            // Direct X 사용

// 전역변수
LPDIRECT3D9              g_pD3D    = NULL;   // Direct 3D에 접근
LPDIRECT3DDEVICE9        g_pD3DDev = NULL;   // 비디오카드에 접근


//-----------------------------------------------------------------------------
// Direct X Graphics 초기화
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
    // Direct 3D 사용
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ))) return E_FAIL;

    // 디스플레이 모드 조사
    D3DDISPLAYMODE d3ddm;
    if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm )))
        return E_FAIL;

    // Direct 3D 디바이스 생성
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );        // 몽땅 0으로
    d3dpp.Windowed = TRUE;                      // 윈도우모드
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;   // 화면전환 방법
    d3dpp.BackBufferFormat = d3ddm.Format;      // 현재 화면 포맷 사용

    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pD3DDev ) ) ) return E_FAIL;

    return S_OK;
}
//-----------------------------------------------------------------------------
// 렌더함수
//-----------------------------------------------------------------------------
VOID Render()
{
    // 배경을 검게칠한다
    g_pD3DDev->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,0), 1.0f, 0 );
    
    g_pD3DDev->BeginScene(); // 렌더 시작
    
    // 추가할 내용은 이곳에
    
    g_pD3DDev->EndScene(); // 렌더 종료
    
    // 출력한 내용을 실제 윈도우에 나타나게 한다
    g_pD3DDev->Present( NULL, NULL, NULL, NULL );
}
//-----------------------------------------------------------------------------		
// 해제	
//-----------------------------------------------------------------------------		
#define SAFE_RELEASE(p) if(p){(p)->Release();(p)=NULL;}		
		
VOID Cleanup()
{		
    SAFE_RELEASE(g_pD3DDev);		
    SAFE_RELEASE(g_pD3D);		
}	
