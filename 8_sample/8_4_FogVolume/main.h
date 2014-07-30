//-------------------------------------------------------------
// File: main.h
//
// Desc: ��������
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
	CD3DMesh						*m_pMesh;	// ��
	CD3DMesh						*m_pMeshBg;	// ����
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;	// ��������
	LPD3DXEFFECT					m_pEffect;	// ����Ʈ
	D3DXHANDLE						m_hmWVP;	// ����~�������
	D3DXHANDLE						m_hmWVPT;	// ����~�ؽ�ó���
	D3DXHANDLE						m_hvCol;	// �޽û�
	D3DXHANDLE						m_hvDir;	// ��������
	// ������Ÿ��
	LPDIRECT3DSURFACE9				m_pMapZ;	// ���� Z����
	LPDIRECT3DTEXTURE9				m_pColorMap;// ��
	LPDIRECT3DSURFACE9				m_pColorMapSurf;
	LPDIRECT3DTEXTURE9				m_pDepthMap;// ����
	LPDIRECT3DSURFACE9				m_pDepthMapSurf;
	LPDIRECT3DTEXTURE9				m_pFogMap;	// ���׳�
	LPDIRECT3DSURFACE9				m_pFogMapSurf;

	DWORD							m_Width;	// ����Ʈ ��
	DWORD							m_Height;	// ����
	UINT							m_MapW;		// �ؽ�ó ��
	UINT							m_MapH;		// ����

	D3DXMATRIX						m_mWorld;	// �������
	D3DXMATRIX						m_mView;	// �����
	D3DXMATRIX						m_mProj;	// �������
	D3DXVECTOR4						m_LightDir;	// ��������

	FLOAT					m_zoom;				// ��
    FLOAT                   m_fWorldRotX;       // X��ȸ��
    FLOAT                   m_fWorldRotY;       // Y��ȸ��

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
