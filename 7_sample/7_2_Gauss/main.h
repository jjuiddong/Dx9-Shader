//-------------------------------------------------------------
// File: main.h
//
// Desc: Gaussian ����
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
	BOOL bDispersionUp;
	BOOL bDispersionDown;
};




//-------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: ���ø����̼� Ŭ����
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	enum {
		WEIGHT_MUN = 8,						// ����ġ ����� ����
	};
	FLOAT					m_dispersion_sq;// �л��� ����
	FLOAT					m_tbl[WEIGHT_MUN];// ����ġ �迭
	VOID		UpdateWeight( FLOAT param );// ����ġ �迭 ���
	
	CD3DMesh				*m_pMesh;
	CD3DMesh				*m_pMeshBg;
			
	// ���̴�
	LPD3DXEFFECT		    m_pEffect;		// ����Ʈ
	D3DXHANDLE				m_hTechnique;	// ��ũ��
	D3DXHANDLE				m_hafWeight;	// ����ġ �迭
	D3DXHANDLE				m_htSrcMap;		// �ؽ�ó

	// �׸��ڸ�
	LPDIRECT3DSURFACE9		m_pMapZ;			// ���̹���
	LPDIRECT3DTEXTURE9		m_pOriginalMap;		// �ؽ�ó
	LPDIRECT3DSURFACE9		m_pOriginalMapSurf;	// ǥ��
	LPDIRECT3DTEXTURE9		m_pXMap;		// �ؽ�ó
	LPDIRECT3DSURFACE9		m_pXMapSurf;	// ǥ��
	LPDIRECT3DTEXTURE9		m_pXYMap;		// �ؽ�ó
	LPDIRECT3DSURFACE9		m_pXYMapSurf;	// ǥ��

	// ��ȯ���
	D3DXMATRIX				m_mWorld;
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
public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

