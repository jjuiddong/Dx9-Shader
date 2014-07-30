//-------------------------------------------------------------
// File: main.h
//
// Desc: �ǻ��ɵ�
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

    BOOL bA;
    BOOL bS;
    BOOL bQ;
    BOOL bW;
};




//-------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: ���ø����̼� Ŭ����
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	FLOAT					m_Focus;	// ��������
	FLOAT					m_Scale;	// ��������

	// ������Ÿ��
	LPDIRECT3DSURFACE9		m_pMapZ;		// ���̹���
	LPDIRECT3DTEXTURE9		m_pOriginalTex;	// �ؽ�ó
	LPDIRECT3DSURFACE9		m_pOriginalSurf;// ǥ��
	LPDIRECT3DTEXTURE9		m_pPostTex[2];	// �ؽ�ó
	LPDIRECT3DSURFACE9		m_pPostSurf[2];	// ǥ��

	// ���̴�
	LPD3DXEFFECT		    m_pEffect;		// ����Ʈ
	D3DXHANDLE				m_hTechnique;	// ��ũ��
	D3DXHANDLE				m_hmWVP;		// ��ȯ���
	D3DXHANDLE				m_hvCol;		// ������
	D3DXHANDLE				m_hvDir;		// ��������
	D3DXHANDLE				m_hvCenter;		// ��������
	D3DXHANDLE				m_hvScale;		// DOF
	D3DXHANDLE				m_htSrcTex;		// �ؽ�ó
	D3DXHANDLE				m_htBlurTex;	// �ؽ�ó

	CD3DMesh				*m_pMesh;
	CD3DMesh				*m_pMeshBg;
			
	// ��ȯ���
	D3DXMATRIX				m_mWorld;
	D3DXMATRIX				m_mView;
	D3DXMATRIX				m_mProj;

	D3DXVECTOR3				m_LighPos;		// ������ġ

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

