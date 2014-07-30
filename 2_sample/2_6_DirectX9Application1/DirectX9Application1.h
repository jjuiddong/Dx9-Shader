//-----------------------------------------------------------------------------
// File: DirectX9Application1.h
//
// Desc: DirectX AppWizard������ ������ ������ ���ø����̼�
//-----------------------------------------------------------------------------
#pragma once




//-----------------------------------------------------------------------------
// ���� & ���
//-----------------------------------------------------------------------------
// TODO: "DirectX AppWizard Apps"�� ��� ȸ���� �̸����� �ٲټ���
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\DirectX9Application1")

// ������ �Էµ����͸� ������ ����ü
struct UserInput
{
    // TODO: �ʿ��� �͵� �߰�
    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;
};




//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: ���ø����̼� Ŭ����
//
//
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    BOOL                    m_bLoadingApp;          // �ε���?
    CD3DFont*               m_pFont;                // ��Ʈ
    ID3DXMesh*              m_pD3DXMesh;            // ������ �޽�
    UserInput               m_UserInput;            // �Էµ�����

    FLOAT                   m_fWorldRotX;           // X��ȸ��
    FLOAT                   m_fWorldRotY;           // Y��ȸ��

protected:
    // ���ѹ� �ʱ�ȭ
    virtual HRESULT OneTimeSceneInit();
    // LPDIRECT3DDEVICE9 �� ����Ǿ����� �Ҹ��� �ʱ�ȭó��
    virtual HRESULT InitDeviceObjects();
    // ȭ��ũ�Ⱑ ����Ǿ����� �Ҹ��� �ʱ�ȭó��
    virtual HRESULT RestoreDeviceObjects();
    // RestoreDeviceObjects()�� �����ϴ� ����ó��
    virtual HRESULT InvalidateDeviceObjects();
    // InitDeviceObjects()�� �����ϴ� ����ó��
    virtual HRESULT DeleteDeviceObjects();
    // ȭ�� ������
    virtual HRESULT Render();
    // Ű �Է�ó���� ������ ����
    virtual HRESULT FrameMove();
    // ���帶������ �Ҹ��� ����ó��
    virtual HRESULT FinalCleanup();
    // �����Ǵ� ��ɰ˻�
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );

    // ����̽� ��������� ���� ����׿� �޽����Լ�
    HRESULT RenderText();
    
    // Ű�Է� ����
    void    UpdateInput( UserInput* pUserInput );
public:
    // ���� �̺�Ʈ ó��
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    
    CMyD3DApplication();           // ������
    virtual ~CMyD3DApplication();  // �Ҹ���
};

