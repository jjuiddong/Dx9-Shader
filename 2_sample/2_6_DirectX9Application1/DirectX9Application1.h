//-----------------------------------------------------------------------------
// File: DirectX9Application1.h
//
// Desc: DirectX AppWizard에의해 생성된 윈도우 어플리케이션
//-----------------------------------------------------------------------------
#pragma once




//-----------------------------------------------------------------------------
// 정의 & 상수
//-----------------------------------------------------------------------------
// TODO: "DirectX AppWizard Apps"를 당신 회사명과 이름으로 바꾸세요
#define DXAPP_KEY        TEXT("Software\\DirectX AppWizard Apps\\DirectX9Application1")

// 현재의 입력데이터를 보관할 구조체
struct UserInput
{
    // TODO: 필요한 것들 추가
    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;
};




//-----------------------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: 어플리케이션 클래스
//
//
//-----------------------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
    BOOL                    m_bLoadingApp;          // 로드중?
    CD3DFont*               m_pFont;                // 폰트
    ID3DXMesh*              m_pD3DXMesh;            // 주전자 메시
    UserInput               m_UserInput;            // 입력데이터

    FLOAT                   m_fWorldRotX;           // X축회전
    FLOAT                   m_fWorldRotY;           // Y축회전

protected:
    // 단한번 초기화
    virtual HRESULT OneTimeSceneInit();
    // LPDIRECT3DDEVICE9 가 변경되었을때 불리는 초기화처리
    virtual HRESULT InitDeviceObjects();
    // 화면크기가 변경되었을때 불리는 초기화처리
    virtual HRESULT RestoreDeviceObjects();
    // RestoreDeviceObjects()에 대응하는 해제처리
    virtual HRESULT InvalidateDeviceObjects();
    // InitDeviceObjects()에 대응하는 해제처리
    virtual HRESULT DeleteDeviceObjects();
    // 화면 렌더링
    virtual HRESULT Render();
    // 키 입력처리나 움직임 제어
    virtual HRESULT FrameMove();
    // 가장마지막에 불리는 해제처리
    virtual HRESULT FinalCleanup();
    // 지원되는 기능검사
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT );

    // 디바이스 정보출력을 위한 디버그용 메시지함수
    HRESULT RenderText();
    
    // 키입력 갱신
    void    UpdateInput( UserInput* pUserInput );
public:
    // 각종 이벤트 처리
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    
    CMyD3DApplication();           // 생성자
    virtual ~CMyD3DApplication();  // 소멸자
};

