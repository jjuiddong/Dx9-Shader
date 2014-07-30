//-------------------------------------------------------------
// File: main.h
//
// Desc: 3D Perlin Noise
//-------------------------------------------------------------
#pragma once



//-------------------------------------------------------------
// 정의 & 상수
//-------------------------------------------------------------
// 입력데이터를 보존할 구조체
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
// Desc: 어플리케이션 클래스
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	// 장면 렌더용
	CD3DMesh				*m_pMesh;		// 주전자 메시
	LPDIRECT3DVOLUMETEXTURE9 m_pTex;		// 노이즈 텍스처
	LPDIRECT3DTEXTURE9		m_pWoodTex;		// 나이테 텍스처

	// 셰이더
	LPD3DXEFFECT		    m_pEffect;		// 이펙트
	D3DXHANDLE				m_hTechnique;	// 테크닉
	D3DXHANDLE				m_hmWVP;		// 로컬-투영변환 행렬
	D3DXHANDLE				m_htTex;		// 난수텍스처
	D3DXHANDLE				m_htWoodTex;	// 모양텍스처
	

	// 일반 좌표변환 행렬
	D3DXVECTOR4				m_vFromPt;
	D3DXMATRIX				m_mView;
	D3DXMATRIX				m_mProj;

	BOOL					m_bLoadingApp;	// 로드중?
    CD3DFont*				m_pFont;		// 폰트
    UserInput				m_UserInput;	// 입력데이터

    FLOAT                   m_fWorldRotX;   // X축회전
    FLOAT                   m_fWorldRotY;   // Y축회전
    FLOAT                   m_fViewZoom;    // 시점거리

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

