//-------------------------------------------------------------
// File: main.h
//
// Desc:ũ�ν�����
//-------------------------------------------------------------
#pragma once



//-------------------------------------------------------------
// ���� & ���
//-------------------------------------------------------------
// �Էµ����͸� ������ ����ü
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
// �ؽ�ó��ǥ�� ����ü
struct CoordRect
{
    float u0, v0;
    float u1, v1;
};



//-------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: ���ø����̼� Ŭ����
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	enum{
		NUM_STAR_TEXTURES =  8, // �����ؽ�ó ����
		MAX_SAMPLES       = 16, // ���÷��� �ִ��
	};

	DWORD m_dwCropWidth;
	DWORD m_dwCropHeight;

    PDIRECT3DTEXTURE9		m_pTexScene;	    // HDRȭ���� ������ ��
	PDIRECT3DSURFACE9		m_pSurfScene;
    PDIRECT3DTEXTURE9		m_pTexSceneScaled;	// ��ҹ���
	PDIRECT3DSURFACE9		m_pSurfSceneScaled;
    PDIRECT3DTEXTURE9		m_pTexBrightPass;	// �ֵ� ����
	PDIRECT3DSURFACE9		m_pSurfBrightPass;
	PDIRECT3DTEXTURE9		m_pTexStarSource;   // ���� ������ �� �̹���
	PDIRECT3DSURFACE9		m_pSurfStarSource;
    PDIRECT3DTEXTURE9		m_apTexStar[NUM_STAR_TEXTURES];// �ӽ�
    PDIRECT3DSURFACE9		m_apSurfStar[NUM_STAR_TEXTURES];

	// ��鷻����
	LPDIRECT3DTEXTURE9		m_pNormalMap;	// ������
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;// ��������
	CD3DMesh				*m_pMesh;		// ������ �޽�
	CD3DMesh				*m_pMeshBg;		// ��� �޽�

	// ���̴�
	LPD3DXEFFECT		    m_pEffect;		// ����Ʈ
	D3DXHANDLE				m_hTechnique;	// ��ũ��
	D3DXHANDLE				m_hmWVP;		// ����-���� ��ȯ���
	D3DXHANDLE				m_hvLightDir;	// ��������
	D3DXHANDLE				m_hvColor;		// ������
	D3DXHANDLE				m_hvEyePos;		// ����
	D3DXHANDLE				m_htDecaleTex;	// �����ؽ�ó
	D3DXHANDLE				m_htNormalMap;	// ������
	

	// ��ȯ���
	D3DXVECTOR4				m_vFromPt;
	D3DXMATRIX				m_mView;
	D3DXMATRIX				m_mProj;

	BOOL					m_bLoadingApp;	// �ε���?
    CD3DFont*				m_pFont;		// ��Ʈ
    UserInput				m_UserInput;	// �Էµ�����

    FLOAT                   m_fWorldRotX;   // X��ȸ��
    FLOAT                   m_fWorldRotY;   // Y��ȸ��
    FLOAT                   m_fViewZoom;    // �����Ÿ�

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

	// �߰��Լ�
    void    RenderScene();				// �����ڿ� ���� ����
    HRESULT Scene_To_SceneScaled();		// ��ҹ��ۿ� ����
    HRESULT SceneScaled_To_BrightPass();// ���� �κ� ����
    HRESULT BrightPass_To_StarSource();	// �ణ ������
	HRESULT RenderStar();				// ���� �����
	// ��üȭ�� ������ ����
	void DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV);
	// ���콺 ������ ��� ���
	HRESULT GetGaussBlur5x5(DWORD dwD3DTexWidth, DWORD dwD3DTexHeight,
				D3DXVECTOR2* avTexCoordOffset, D3DXVECTOR4* avSampleWeight );
	// �����̹����� ����� ���� ũ�⿡���� �ؽ�ó�� ����
	HRESULT GetTextureCoords( PDIRECT3DTEXTURE9 pTexSrc, RECT* pRectSrc, 
               PDIRECT3DTEXTURE9 pTexDest, RECT* pRectDest, CoordRect* pCoords );

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

