//-----------------------------------------------------------------------------
// 일반함수
// 
//-----------------------------------------------------------------------------
#ifndef __draw_h__
#define __draw_h__

#include <d3d9.h>	// DirectX 사용
#include "stdafx.h"

#define RELEASE(p) if(p){(p)->Release();(p)=NULL;}

// 함수 선언
HRESULT Initialize(LPDIRECT3DDEVICE9 pD3DDev);	// 초기화
VOID    Update    (LPDIRECT3DDEVICE9 pD3DDev);	// 렌더
VOID    Close     (LPDIRECT3DDEVICE9 pD3DDev);	// 해제

#endif // __draw_h__
