//-------------------------------------------------------------
// File: main.h
//
// Desc: �ݿ��ݻ籤
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
	LPDIRECT3DTEXTURE9		m_pNormalMap;	// ������
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;// ��������
	CD3DMesh				*m_pMesh;		// ������ �޽�
	CD3DMesh				*m_pMeshBg;		// ��� �޽�

	// ���̴�
	LPD3DXEFFECT		    m_pEffect;		// ����Ʈ
	D3DXHANDLE				m_hTechnique;	// ��ũ��
	D3DXHANDLE				m_hmWVP;		// ����-������ȯ���
	D3DXHANDLE				m_hvLightDir;	// ��������
	D3DXHANDLE				m_hvColor;		// ������
	D3DXHANDLE				m_hvEyePos;		// ������ ��ġ
	D3DXHANDLE				m_htDecaleTex;	// �����ؽ�ó
	D3DXHANDLE				m_htNormalMap;	// ������
	

	// �Ϲ� ��ǥ��ȯ���
	D3DXVECTOR4				m_vFromPt;
	D3DXMATRIX				m_mView;
	D3DXMATRIX				m_mProj;

	BOOL					m_bLoadingApp;	// �ε���?
    CD3DFont*				m_pFont;		// ��Ʈ
    UserInput				m_UserInput;	// �Էµ�����

    FLOAT                   m_fWorldRotX;   // X��ȸ��
    FLOAT                   m_fWorldRotY;   // Y��ȸ��
    FLOAT                   m_fViewZoom;    // ������ �Ÿ�

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

