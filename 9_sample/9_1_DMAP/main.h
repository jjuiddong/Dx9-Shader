//-------------------------------------------------------------
// File: main.h
//
// Desc: 변위맵핑
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
	BOOL	 m_bDMap;	// 변위맵핑이 가능한가?
	FLOAT							m_degree;	// 변위의 크기
	CD3DMesh						*m_pMesh;	// 모델
	LPDIRECT3DTEXTURE9				m_pDispMap; // D-MAP
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;	// 정점선언
	LPD3DXEFFECT					m_pEffect;	// 셰이더

	D3DXMATRIX						m_mWorld;	// 월드행렬
	D3DXMATRIX						m_mView;	// 뷰행렬
	D3DXMATRIX						m_mProj;	// 투영행렬
	D3DXVECTOR4						m_LighPos;	// 광원의 방향

	BOOL                    m_bLoadingApp;      // 로드중?
    CD3DFont*               m_pFont;            // 폰트
    UserInput               m_UserInput;        // 입력데이터

    FLOAT                   m_fWorldRotX;       // X축회전
    FLOAT                   m_fWorldRotY;       // Y축회전

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
