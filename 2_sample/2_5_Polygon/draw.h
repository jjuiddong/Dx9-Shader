//-----------------------------------------------------------------------------
// �Ϲ��Լ�
// 
//-----------------------------------------------------------------------------
#ifndef __draw_h__
#define __draw_h__

#include <d3d9.h>	// DirectX ���
#include "stdafx.h"

#define RELEASE(p) if(p){(p)->Release();(p)=NULL;}

// �Լ� ����
HRESULT Initialize(LPDIRECT3DDEVICE9 pD3DDev);	// �ʱ�ȭ
VOID    Update    (LPDIRECT3DDEVICE9 pD3DDev);	// ����
VOID    Close     (LPDIRECT3DDEVICE9 pD3DDev);	// ����

#endif // __draw_h__
