//-------------------------------------------------------------
// File: main.h
//
// Desc: �Ÿ�����
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
    BOOL bA;
    BOOL bS;
};




//-------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: ���ø����̼� Ŭ����
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	CD3DMesh						*m_pMesh;	// ��
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;	// ��������
	LPD3DXEFFECT					m_pEffect;	// ����Ʈ
	D3DXHANDLE						m_hmWVP;	// ����~�������
	D3DXHANDLE						m_hvCol;	// �޽û�
	D3DXHANDLE						m_hvDir;	// ��������
	D3DXHANDLE						m_hvFog;	// ������ near/far
	D3DXHANDLE						m_hvEye;	// ���� ��ġ

	D3DXMATRIX						m_mWorld;	// �������
	D3DXMATRIX						m_mView;	// �����
	D3DXMATRIX						m_mProj;	// �������
	D3DXVECTOR4						m_LightDir;	// ��������
	D3DXVECTOR3						m_Eye;		// ���� ��ġ

	FLOAT						m_near;			// ���׽���
	FLOAT						m_far;			// ��������
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
