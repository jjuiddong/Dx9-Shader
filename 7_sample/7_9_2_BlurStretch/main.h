//-------------------------------------------------------------
// File: main.h
//
// Desc: 모션블러
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
	BOOL bDispersionUp;
	BOOL bDispersionDown;
};




//-------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: 어플리케이션 클래스
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	CD3DMesh				*m_pMesh;
	CD3DMesh				*m_pMeshBg;
			
	// 셰이더
	LPD3DXEFFECT		    m_pEffect;		// 이펙트
	D3DXHANDLE				m_hTechnique;	// 테크닉
	D3DXHANDLE				m_hmWV;			// 로컬-뷰 변환행렬
	D3DXHANDLE				m_hmVP;			// 뷰-투영 변환행렬
	D3DXHANDLE				m_hmLastWV;		// 1프레임 직전 변환행렬
	D3DXHANDLE				m_hvLightDir;	// 광원방향
	D3DXHANDLE				m_hvEyePos;		// 시점
	D3DXHANDLE				m_hvCol;		// 정점색

	// 변환행렬
	D3DXMATRIX				m_mWorld;
	D3DXMATRIX				m_mView;
	D3DXMATRIX				m_mProj;
	D3DXMATRIX				m_mLastWV;

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

