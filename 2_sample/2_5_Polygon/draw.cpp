#include "draw.h"

LPDIRECT3DVERTEXBUFFER9 g_pVB     = NULL;		// 정점정보 저장

// ----------------------------------------------------------------------------
// 플레서블 정점포맷 정점선언
// ----------------------------------------------------------------------------
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

typedef struct {
    FLOAT x, y, z, rhw; // 스크린 좌표계 위치
    DWORD color;        // 정점색
} CUSTOMVERTEX;


//-----------------------------------------------------------------------------
// 초기화
//   정점버퍼 생성, vertices의 내용을 정점버퍼에 저장
//-----------------------------------------------------------------------------
HRESULT Initialize(LPDIRECT3DDEVICE9		pD3DDev)
{
    // 3각형 정점과 색정보
    CUSTOMVERTEX vertices[] = {
		//   x,     y,      z,  rhw,   color        (적   녹    청   α) 
        { 150.0f,  50.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff,0x00,0x00,0xff), },
        { 250.0f, 250.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0x00,0xff,0x00,0xff), },
        {  50.0f, 250.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0x00,0x00,0xff,0xff), },
    };
	
	// 정점버퍼 생성
    if( FAILED( pD3DDev->CreateVertexBuffer( 3*sizeof(CUSTOMVERTEX),
                                0, D3DFVF_CUSTOMVERTEX,
                                D3DPOOL_DEFAULT, &g_pVB, NULL))) return E_FAIL;

    // 정점버퍼에 정보저장
    VOID* pVertices;
    if(FAILED( g_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0))) return E_FAIL;
    memcpy( pVertices, vertices, sizeof(vertices) );
    g_pVB->Unlock();

    return S_OK;
}
//-----------------------------------------------------------------------------
// 렌더함수
//-----------------------------------------------------------------------------
VOID Update(LPDIRECT3DDEVICE9		pD3DDev)
{
    pD3DDev->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
    pD3DDev->SetFVF( D3DFVF_CUSTOMVERTEX );
    pD3DDev->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );
}
//-----------------------------------------------------------------------------
// 사용한 메모리 OS에 반환
//-----------------------------------------------------------------------------
VOID Close(LPDIRECT3DDEVICE9		pD3DDev)
{
	RELEASE(g_pVB);
}
