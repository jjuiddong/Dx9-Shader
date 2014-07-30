//-------------------------------------------------------------
// File: main.h
//
// Desc: �ε巯�� �׸���
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
	CD3DMesh				*m_pMeshCar;
	CD3DMesh				*m_pMeshBg;
			
	// ���̴�
	LPD3DXEFFECT		    m_pEffect;	  // ����Ʈ
	D3DXHANDLE				m_hTechnique; // ��ũ��
	D3DXHANDLE				m_hmWVP;	  // ����*��*���� ���
	D3DXHANDLE				m_hmWLP;	  // �������⿡���� ��ȯ���
	D3DXHANDLE				m_hmWLPB;	  // �������⿡���� ��ȯ���
	D3DXHANDLE				m_hvCol;	  // �޽û�
	D3DXHANDLE				m_hvDir;	  // ��������
	D3DXHANDLE				m_htShadowMap;// �ؽ�ó
	D3DXHANDLE				m_htSrcMap;   // �ؽ�ó

	// �׸��� ��
	LPDIRECT3DSURFACE9		m_pShadowMapZ;		// ���̹���
	LPDIRECT3DTEXTURE9		m_pShadowMap;		// �ؽ�ó
	LPDIRECT3DSURFACE9		m_pShadowMapSurf;	// ǥ��
	LPDIRECT3DTEXTURE9		m_pEdgeMap;			// �ؽ�ó
	LPDIRECT3DSURFACE9		m_pEdgeMapSurf;		// ǥ��
	LPDIRECT3DTEXTURE9		m_pSoftMap[2];		// �ؽ�ó
	LPDIRECT3DSURFACE9		m_pSoftMapSurf[2];	// ǥ��

	// �Ϲ� ��ǥ��ȯ���
	D3DXMATRIX				m_mWorld;
	D3DXMATRIX				m_mView;
	D3DXMATRIX				m_mProj;
	D3DXMATRIX				m_mLightVP;

	D3DXVECTOR3				m_LighPos;		// ������ġ

	BOOL					m_bLoadingApp;	// �ε���?
    CD3DFont*				m_pFont;		// ��Ʈ
    UserInput				m_UserInput;	// �Էµ�����

    FLOAT                   m_fWorldRotX;   // X��ȸ��
    FLOAT                   m_fWorldRotY;   // Y��ȸ��
    FLOAT                   m_fViewZoom;    // �����Ÿ�


	VOID DrawModel( int pass );	// �� �н����� �ҷ��� �� ����
	
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

