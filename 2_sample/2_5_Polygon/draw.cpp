#include "draw.h"

LPDIRECT3DVERTEXBUFFER9 g_pVB     = NULL;		// �������� ����

// ----------------------------------------------------------------------------
// �÷����� �������� ��������
// ----------------------------------------------------------------------------
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)

typedef struct {
    FLOAT x, y, z, rhw; // ��ũ�� ��ǥ�� ��ġ
    DWORD color;        // ������
} CUSTOMVERTEX;


//-----------------------------------------------------------------------------
// �ʱ�ȭ
//   �������� ����, vertices�� ������ �������ۿ� ����
//-----------------------------------------------------------------------------
HRESULT Initialize(LPDIRECT3DDEVICE9		pD3DDev)
{
    // 3���� ������ ������
    CUSTOMVERTEX vertices[] = {
		//   x,     y,      z,  rhw,   color        (��   ��    û   ��) 
        { 150.0f,  50.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0xff,0x00,0x00,0xff), },
        { 250.0f, 250.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0x00,0xff,0x00,0xff), },
        {  50.0f, 250.0f, 0.5f, 1.0f, D3DCOLOR_RGBA(0x00,0x00,0xff,0xff), },
    };
	
	// �������� ����
    if( FAILED( pD3DDev->CreateVertexBuffer( 3*sizeof(CUSTOMVERTEX),
                                0, D3DFVF_CUSTOMVERTEX,
                                D3DPOOL_DEFAULT, &g_pVB, NULL))) return E_FAIL;

    // �������ۿ� ��������
    VOID* pVertices;
    if(FAILED( g_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0))) return E_FAIL;
    memcpy( pVertices, vertices, sizeof(vertices) );
    g_pVB->Unlock();

    return S_OK;
}
//-----------------------------------------------------------------------------
// �����Լ�
//-----------------------------------------------------------------------------
VOID Update(LPDIRECT3DDEVICE9		pD3DDev)
{
    pD3DDev->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
    pD3DDev->SetFVF( D3DFVF_CUSTOMVERTEX );
    pD3DDev->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );
}
//-----------------------------------------------------------------------------
// ����� �޸� OS�� ��ȯ
//-----------------------------------------------------------------------------
VOID Close(LPDIRECT3DDEVICE9		pD3DDev)
{
	RELEASE(g_pVB);
}
