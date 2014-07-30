//-------------------------------------------------------------
// File: main.h
//
// Desc: 깊이버퍼 그림자
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
	CD3DMesh				*m_pMesh;
	CD3DMesh				*m_pMeshBg;
	D3DXVECTOR3				m_pos;			// UFO의 위치
			
	// 셰이더
	LPD3DXEFFECT		    m_pEffect;	  // 이펙트
	D3DXHANDLE				m_hTechnique; // 테크닉
	D3DXHANDLE				m_hmWVP;	  // 월드*뷰*투영 행렬
	D3DXHANDLE				m_hmWLP;	  // 광원방향에서의 변환행렬
	D3DXHANDLE				m_hmWLPB;	  // 광원방향에서의 변환행렬
	D3DXHANDLE				m_hvCol;	  // 메시색
	D3DXHANDLE				m_hvDir;	  // 광원방향
	D3DXHANDLE				m_htShadowMap;// 텍스처
	LPDIRECT3DVERTEXDECLARATION9 m_pDecl;	// 정점선언

	// 그림자맵
	LPDIRECT3DTEXTURE9		m_pShadowTex;	// 텍스처
	LPDIRECT3DSURFACE9		m_pShadowSurf;	// 표면
	LPDIRECT3DSURFACE9		m_pZ;			// 깊이버퍼

	// 일반 좌표변환행렬
	D3DXMATRIX				m_mWorld;
	D3DXMATRIX				m_mView;
	D3DXMATRIX				m_mProj;
	D3DXMATRIX				m_mLightVP;

	D3DXVECTOR3				m_LighPos;		// 광원위치

	BOOL					m_bLoadingApp;	// 로드중?
    CD3DFont*				m_pFont;		// 폰트
    UserInput				m_UserInput;	// 입력데이터

	FLOAT					m_fViewZoom;	// 줌
    FLOAT					m_fWorldRotX;	// X축회전
    FLOAT					m_fWorldRotY;	// Y축회전


	VOID DrawModel( int pass );	// 각 패스에서 불려진 모델 렌더
	
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

