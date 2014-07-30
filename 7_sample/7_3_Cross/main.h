//-------------------------------------------------------------
// File: main.h
//
// Desc:크로스필터
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
// 텍스처좌표용 구조체
struct CoordRect
{
    float u0, v0;
    float u1, v1;
};



//-------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: 어플리케이션 클래스
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	enum{
		NUM_STAR_TEXTURES =  8, // 광선텍스처 개수
		MAX_SAMPLES       = 16, // 샘플러의 최대수
	};

	DWORD m_dwCropWidth;
	DWORD m_dwCropHeight;

    PDIRECT3DTEXTURE9		m_pTexScene;	    // HDR화면을 렌더할 곳
	PDIRECT3DSURFACE9		m_pSurfScene;
    PDIRECT3DTEXTURE9		m_pTexSceneScaled;	// 축소버퍼
	PDIRECT3DSURFACE9		m_pSurfSceneScaled;
    PDIRECT3DTEXTURE9		m_pTexBrightPass;	// 휘도 추출
	PDIRECT3DSURFACE9		m_pSurfBrightPass;
	PDIRECT3DTEXTURE9		m_pTexStarSource;   // 별의 원본이 될 이미지
	PDIRECT3DSURFACE9		m_pSurfStarSource;
    PDIRECT3DTEXTURE9		m_apTexStar[NUM_STAR_TEXTURES];// 임시
    PDIRECT3DSURFACE9		m_apSurfStar[NUM_STAR_TEXTURES];

	// 장면렌더용
	LPDIRECT3DTEXTURE9		m_pNormalMap;	// 법선맵
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;// 정점선언
	CD3DMesh				*m_pMesh;		// 주전자 메시
	CD3DMesh				*m_pMeshBg;		// 배경 메시

	// 셰이더
	LPD3DXEFFECT		    m_pEffect;		// 이펙트
	D3DXHANDLE				m_hTechnique;	// 테크닉
	D3DXHANDLE				m_hmWVP;		// 로컬-투영 변환행렬
	D3DXHANDLE				m_hvLightDir;	// 광원방향
	D3DXHANDLE				m_hvColor;		// 정점색
	D3DXHANDLE				m_hvEyePos;		// 시점
	D3DXHANDLE				m_htDecaleTex;	// 디컬텍스처
	D3DXHANDLE				m_htNormalMap;	// 법선맵
	

	// 변환행렬
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

	// 추가함수
    void    RenderScene();				// 주전자와 지면 렌더
    HRESULT Scene_To_SceneScaled();		// 축소버퍼에 복사
    HRESULT SceneScaled_To_BrightPass();// 밝은 부분 추출
    HRESULT BrightPass_To_StarSource();	// 약간 뭉개기
	HRESULT RenderStar();				// 광선 만들기
	// 전체화면 폴리곤 렌더
	void DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV);
	// 가우스 뭉개기 계수 계산
	HRESULT GetGaussBlur5x5(DWORD dwD3DTexWidth, DWORD dwD3DTexHeight,
				D3DXVECTOR2* avTexCoordOffset, D3DXVECTOR4* avSampleWeight );
	// 원본이미지와 출력할 곳의 크기에따라 텍스처를 보정
	HRESULT GetTextureCoords( PDIRECT3DTEXTURE9 pTexSrc, RECT* pRectSrc, 
               PDIRECT3DTEXTURE9 pTexDest, RECT* pRectDest, CoordRect* pCoords );

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

