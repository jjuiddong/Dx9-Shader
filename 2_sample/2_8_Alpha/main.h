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
	LPDIRECT3DTEXTURE9		m_pEarthTexture;// ���� �ؽ�ó
	LPDIRECT3DTEXTURE9		m_pCloudTexture;// �� �ؽ�ó
	LPDIRECT3DVERTEXBUFFER9 m_pVB;			// �������� ����


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

