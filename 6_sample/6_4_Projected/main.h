//-------------------------------------------------------------
// File: main.h
//
// Desc: �����ؽ�ó
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
    BOOL bZ;
    BOOL bX;
};


//-------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: ���ø����̼� Ŭ����
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	CD3DMesh					*m_pMeshBg;		// ����
	D3DXVECTOR3					m_pos;			// UFO�� ��ġ
	D3DXVECTOR4					m_light;		// ������ ��ġ

	LPD3DXEFFECT				m_pEffect;		// ���̴�
	D3DXHANDLE					m_hTechnique;	// ��ũ��
	D3DXHANDLE					m_hmWVP;		// ����->�������
	D3DXHANDLE					m_hmWVPT;		// ����->�ؽ�ó���
	D3DXHANDLE					m_hvLightPos;	// ������ ����
	D3DXHANDLE					m_hDecaleMap;	// ���� �ؽ�ó
	D3DXHANDLE					m_hShadowMap;	// �׸��ڸ�
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;	// ��������

	// �׸����ؽ�ó
	LPDIRECT3DTEXTURE9			m_pShadowTex;
	LPDIRECT3DSURFACE9			m_pShadowSurf;
	LPDIRECT3DSURFACE9			m_pShadowTexZ;

	CD3DMesh*					m_pMesh;		// �����

	D3DXMATRIX					m_mWorld;		// ���� ���
	D3DXMATRIX					m_mView;		// �� ���
	D3DXMATRIX					m_mProj;		// ���� ���

	FLOAT						m_zoom;			// ��
    FLOAT						m_fWorldRotX;	// X��ȸ��
    FLOAT						m_fWorldRotY;	// Y��ȸ��

	BOOL						m_bLoadingApp;	// �ε���?
    CD3DFont*					m_pFont;		// ��Ʈ
    UserInput					m_UserInput;	// �Էµ�����
	
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
