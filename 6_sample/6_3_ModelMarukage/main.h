//-------------------------------------------------------------
// File: main.h
//
// Desc: 오브젝트 형태의 원형 그림자
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
    BOOL bZ;
    BOOL bX;
};


//-------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: 어플리케이션 클래스
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	D3DXVECTOR3					m_pos;			// UFO의 위치
	D3DXVECTOR4					m_light;		// 광원의 위치

	// 그림자 텍스처
	LPDIRECT3DTEXTURE9			m_pShadowTex;
	LPDIRECT3DSURFACE9			m_pShadowTexSurf;
	LPDIRECT3DSURFACE9			m_pShadowTexZ;

	LPDIRECT3DTEXTURE9			m_pTex;
	CD3DMesh*					m_pMesh;		// 비행모델

	D3DXMATRIX					m_mWorld;		// 월드 행렬
	D3DXMATRIX					m_mView;		// 뷰 행렬
	D3DXMATRIX					m_mProj;		// 투영 행렬

	FLOAT						m_zoom;			// 줌
    FLOAT						m_fWorldRotX;	// X축회전
    FLOAT						m_fWorldRotY;	// Y축회전

	BOOL						m_bLoadingApp;	// 로드중?
    CD3DFont*					m_pFont;		// 폰트
    UserInput					m_UserInput;	// 입력데이터
	
protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice(D3DCAPS9*, DWORD, D3DFORMAT);

    HRESULT RenderText();

    void    UpdateInput( UserInput* pUserInput );
public:
    LRESULT MsgProc( HWND hWnd, UINT msg
					, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};
