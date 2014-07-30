//-------------------------------------------------------------
// File: main.h
//
// Desc: 텍스처 입히기
//-------------------------------------------------------------
#pragma once



//-------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: 어플리케이션 클래스
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    // ★ 정점셰이더
    LPDIRECT3DVERTEXSHADER9 m_pVertexShader;     // ★ 정점셰이더
    LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;  // ★ 정점선언

	LPDIRECT3DVERTEXBUFFER9 m_pVB;		// 정점정보저장

    // 빾듂뛱쀱
    D3DXMATRIXA16  m_matView;			// 뷰행렬
    D3DXMATRIXA16  m_matProj;			// 투영행렬

	BOOL					m_bLoadingApp;	// 로드중?
    CD3DFont*				m_pFont;		// 폰트

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

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

