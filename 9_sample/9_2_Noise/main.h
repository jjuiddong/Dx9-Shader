//-------------------------------------------------------------
// File: main.h
//
// Desc:Perlin Noise
//-------------------------------------------------------------
#pragma once



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
    BOOL bZoomIn;
    BOOL bZoomOut;
};



//-------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: æÓ«√∏Æƒ…¿Ãº« ≈¨∑°Ω∫
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	// ÉVÅ[ÉìÇÃï`âÊóp
	CD3DMesh				*m_pMesh;		// ¡÷¿¸¿⁄ ∏ﬁΩ√
	LPDIRECT3DTEXTURE9		m_pTex;			// ≈ÿΩ∫√≥
	LPDIRECT3DTEXTURE9		m_pWoodTex;		// ≥™¿Ã≈◊ ≈ÿΩ∫√≥

	// ÉVÉFÅ[É_
	LPD3DXEFFECT		    m_pEffect;		// ¿Ã∆Â∆Æ
	D3DXHANDLE				m_hTechnique;	// ≈◊≈©¥–
	D3DXHANDLE				m_hmWVP;		// ∑Œƒ√-≈ıøµ∫Ø»Ø «‡∑ƒ
	D3DXHANDLE				m_htTex;		// ≥≠ºˆ ≈ÿΩ∫√≥
	D3DXHANDLE				m_htWoodTex;	// ∏æÁ ≈ÿΩ∫√≥
	

	// í èÌÇÃç¿ïWïœä∑çsóÒ
	D3DXVECTOR4				m_vFromPt;
	D3DXMATRIX				m_mView;
	D3DXMATRIX				m_mProj;

	BOOL					m_bLoadingApp;	// ∑Œµ˘¡ﬂ?
    CD3DFont*				m_pFont;		// ∆˘∆Æ
    UserInput				m_UserInput;	// ¿‘∑¬µ•¿Ã≈Õ

    FLOAT                   m_fWorldRotX;   // X√‡»∏¿¸
    FLOAT                   m_fWorldRotY;   // Y√‡»∏¿¸
    FLOAT                   m_fViewZoom;    // Ω√¡°¿« ∞≈∏Æ

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );

    HRESULT RenderText();

    void    UpdateInput( UserInput* pUserInput );

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

