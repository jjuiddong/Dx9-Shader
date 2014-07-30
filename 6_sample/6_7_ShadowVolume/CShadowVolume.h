// -------------------------------------------------------------
// �V���h�E�{�����[�����f��
// 
// Copyright (c) 2002,2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------
#ifndef _CShadowVolume_h_
#define _CShadowVolume_h_

#include <d3d9.h>
#include <d3dx9.h>

class CShadowVolume {
public:
	struct SHADOW_VOLUME_VERTEX { D3DXVECTOR3 p, n; };
	static const DWORD FVF;

    SHADOW_VOLUME_VERTEX*		m_pVertices;	// ���_�f�[�^
	DWORD						m_dwNumFaces;	// �ʂ̐�
public:
	
	CShadowVolume();
	~CShadowVolume();

	HRESULT Create( LPD3DXMESH pObject );// ����
	VOID Render( LPDIRECT3DDEVICE9 pd3dDevice );	// �`��
	VOID Destroy();									// �j��
};
#endif // !_CShadowVolume_h_
