//-------------------------------------------------------------
// File: main.h
//
// Desc: ������ũ
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
// Name: class CMyD3DApplication
// Desc: ���ø����̼� Ŭ����
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	// ������ũ�� ������ �ؽ�ó
	LPDIRECT3DTEXTURE9		m_pTex;
	DWORD					m_Size;
	FLOAT					m_pos[2];
	FLOAT					m_vel[2];

	CD3DMesh				*m_pMesh;
	CD3DMesh				*m_pMeshBg;
			
	// ���̴�
	LPD3DXEFFECT		    m_pEffect;		// ����Ʈ
	D3DXHANDLE				m_hTechnique;	// ��ũ��
	D3DXHANDLE				m_htMask;		// �ؽ�ó
	D3DXHANDLE				m_htSrc;		// �ؽ�ó

	// �׸��ڸ�
	LPDIRECT3DSURFACE9		m_pMapZ;			// ���̹���
	LPDIRECT3DTEXTURE9		m_pOriginalMap;		// �ؽ�ó
	LPDIRECT3DSURFACE9		m_pOriginalMapSurf;	// ǥ��

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

