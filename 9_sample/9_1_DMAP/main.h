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
	BOOL	 m_bDMap;	// ���������� �����Ѱ�?
	FLOAT							m_degree;	// ������ ũ��
	CD3DMesh						*m_pMesh;	// ��
	LPDIRECT3DTEXTURE9				m_pDispMap; // D-MAP
	LPDIRECT3DVERTEXDECLARATION9	m_pDecl;	// ��������
	LPD3DXEFFECT					m_pEffect;	// ���̴�

	D3DXMATRIX						m_mWorld;	// �������
	D3DXMATRIX						m_mView;	// �����
	D3DXMATRIX						m_mProj;	// �������
	D3DXVECTOR4						m_LighPos;	// ������ ����

	BOOL                    m_bLoadingApp;      // �ε���?
    CD3DFont*               m_pFont;            // ��Ʈ
    UserInput               m_UserInput;        // �Էµ�����

    FLOAT                   m_fWorldRotX;       // X��ȸ��
    FLOAT                   m_fWorldRotY;       // Y��ȸ��

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
