//-------------------------------------------------------------
// File: main.h
//
// Desc: �ؽ�ó ������
//-------------------------------------------------------------
#pragma once



//-------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: ���ø����̼� Ŭ����
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    // �� �������̴�
    LPDIRECT3DVERTEXSHADER9 m_pVertexShader;     // �� �������̴�
    LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;  // �� ��������

	LPDIRECT3DVERTEXBUFFER9 m_pVB;		// ������������

    // �ϊ��s��
    D3DXMATRIXA16  m_matView;			// �����
    D3DXMATRIXA16  m_matProj;			// �������

	BOOL					m_bLoadingApp;	// �ε���?
    CD3DFont*				m_pFont;		// ��Ʈ

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

public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

