//-------------------------------------------------------------
// File: main.h
//
// Desc: øÏº±º¯¿ßπˆ∆€ ±◊∏≤¿⁄
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
	CD3DMesh				*m_pMesh;		// UFO
	CD3DMesh				*m_pMeshBg;		// ¡ˆ∏È
	D3DXVECTOR3				m_pos;			// UFO¿« ¿ßƒ°
			
	// ÉVÉFÅ[É_
	LPD3DXEFFECT		    m_pEffect;		// ¿Ã∆Â∆Æ
	D3DXHANDLE				m_hTechnique;	// ≈◊≈©¥–
	D3DXHANDLE       m_hmWVP;	// ø˘µÂ*∫‰*≈ıøµ «‡∑ƒ
	D3DXHANDLE       m_hmWLP;	// ±§ø¯πÊ«‚ø°º≠¿« ∫Ø»Ø«‡∑ƒ
	D3DXHANDLE       m_hmWVPT;	// ±§ø¯πÊ«‚ø°º≠¿« ∫Ø»Ø«‡∑ƒ
	D3DXHANDLE       m_hvCol;	// ∏ﬁΩ√ªˆ
	D3DXHANDLE       m_hvId;	// øÏº±º¯¿ßπˆ∆€ π¯»£
	D3DXHANDLE       m_hvDir;	// ±§ø¯πÊ«‚
	D3DXHANDLE       m_htIdMap;// ≈ÿΩ∫√≥
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;	// ¡§¡°º±æ

	// ±◊∏≤¿⁄∏ 
	LPDIRECT3DTEXTURE9		m_pShadowMap;		// ≈ÿΩ∫√≥
	LPDIRECT3DSURFACE9		m_pShadowMapSurf;	// «•∏È
	LPDIRECT3DSURFACE9		m_pShadowMapZ;		// ±Ì¿Ãπˆ∆€

	// ¿œπ› ¡¬«•∫Ø»Ø«‡∑ƒ
	D3DXMATRIX				m_mWorld;
	D3DXMATRIX				m_mView;
	D3DXMATRIX				m_mProj;
	D3DXMATRIX				m_mLightVP;

	D3DXVECTOR3				m_LighPos;		// ±§ø¯¿« ¿ßƒ°

	BOOL						m_bLoadingApp;	// ∑ŒµÂ¡ﬂ?
    CD3DFont*					m_pFont;		// ∆˘∆Æ
    UserInput					m_UserInput;	// ¿‘∑¬µ•¿Ã≈Õ

	FLOAT						m_fViewZoom;	// ¡‹
    FLOAT						m_fWorldRotX;	// X√‡»∏¿¸
    FLOAT						m_fWorldRotY;	// Y√‡»∏¿¸


	VOID DrawModel( int pass );	// ∞¢∞¢¿« ∆–Ω∫ø°º≠ ∫“∑¡¡ˆ¥¬ ∏µ® ∑ª¥ı
	
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

