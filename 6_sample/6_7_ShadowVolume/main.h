//-------------------------------------------------------------
// File: main.h
//
// Desc: ∫º∑˝±◊∏≤¿⁄
//-------------------------------------------------------------
#pragma once
#include "CShadowVolume.h"



//-------------------------------------------------------------
// ¡§¿« & ªÛºˆ
//-------------------------------------------------------------
// ¿‘∑¬µ•¿Ã≈Õ∏¶ ∫∏¡∏«“ ±∏¡∂√º
struct UserInput
{
    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;
};



//-------------------------------------------------------------
// ¿¸√º»≠∏È∑ª¥ı ∆˙∏Æ∞Ô
//-------------------------------------------------------------
class CBigSquare {
private:
	typedef struct {
		D3DXVECTOR4 p;
		D3DCOLOR	color;
	} SHADOWVERTEX;
	static const DWORD FVF;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
public:
	CBigSquare();
	HRESULT		Create( LPDIRECT3DDEVICE9 pd3dDevice );
	void		RestoreDeviceObjects( FLOAT sx, FLOAT sy );
	void		Destroy();
	void		Render( LPDIRECT3DDEVICE9 pd3dDevice );
};




//-------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: æÓ«√∏Æƒ…¿Ãº« ≈¨∑°Ω∫
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	CBigSquare*				m_pBigSquare;	// ±◊∏≤¿⁄ ∑ª¥ıøÎ ¿¸√º»≠∏È ∆˙∏Æ∞Ô
	CD3DMesh*				m_pMeshBG;		// πË∞Ê ∏µ®
	CD3DMesh*				m_pMeshBox;		// ªÛ¿⁄ ∏µ®
	CShadowVolume*			m_pShadowBox;	// ªÛ¿⁄ ∏µ® ±◊∏≤¿⁄

	// ÉVÉFÅ[É_
	LPD3DXEFFECT			m_pEffect;		// ¿Ã∆Â∆Æ
	D3DXHANDLE				m_hmWVP;		// ∑Œƒ√~≈ıøµ«‡∑ƒ
	D3DXHANDLE				m_hvPos;		// ±§ø¯¿ßƒ°

	BOOL					m_bLoadingApp;	// ∑ŒµÂ¡ﬂ?
    CD3DFont*				m_pFont;		// ∆˘∆Æ
    UserInput				m_UserInput;	// ¿‘∑¬µ•¿Ã≈Õ
	
	D3DXMATRIX				m_mView;		// ∫‰«‡∑ƒ
	D3DXMATRIX				m_mProj;		// ≈ıøµ«‡∑ƒ
	D3DXVECTOR3				m_LighPos;		// ±§ø¯¿ßƒ°

    FLOAT					m_fWorldRotX;	// X√‡»∏¿¸
    FLOAT					m_fWorldRotY;	// Y√‡»∏¿¸

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT, D3DFORMAT );

    HRESULT RenderText();

    void    UpdateInput( UserInput* pUserInput );
public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

