// main.cpp : 콘솔어플리케이션의 진입점
//

#include "stdafx.h"
#include "main.h"
#define MAX_LOADSTRING 100

// ----------------------------------------------------------------------------
// 외부함수
// ----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd );
VOID Render();
VOID Cleanup();

// 전역변수
HINSTANCE hInst;								// 인스턴스
TCHAR szTitle[MAX_LOADSTRING];					// 타이틀바 텍스트
TCHAR szWindowClass[MAX_LOADSTRING];			// 윈도우 클래스 명

// 현재 코드모듈에서 사용중인 함수 선언
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// ----------------------------------------------------------------------------
// 어플리케이션 기본루프
// ----------------------------------------------------------------------------
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// 전역문자열 초기화
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MAIN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	
	ZeroMemory( &msg, sizeof(msg) );
	
	// 어플리케이션 초기화
	if (InitInstance (hInstance, nCmdShow)){
		
		hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MAIN);
		
		// 메인 메시지 루프
		ZeroMemory( &msg, sizeof(msg) );
		while( msg.message!=WM_QUIT ) {
			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			} else {
				Render();// 렌더
			}
		}
	}
	
	Cleanup();// DirectX 사용 해제
	
	return (int) msg.wParam;
}



//
//  함수 : MyRegisterClass()
//
//  목적 : 윈도우 클래스 등록
//
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_MAIN);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_MAIN;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   함수 : InitInstance(HANDLE, int)
//
//   목적 : 인스턴스 핸들을 보존하고, 윈도우 생성
//
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 전역 인스턴스 핸들 보존

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 310, 330, NULL, NULL, hInstance, NULL);

   if (!hWnd) return FALSE; // 윈도우 생성 실패

   if(FAILED(InitD3D( hWnd ))) return FALSE;    // Direct X 초기화

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  함수 : WndProc(HWND, unsigned, WORD, LONG)
//
//  목적 : 메인윈도우의 메시지 처리
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND://  	- 어플리케이션 메뉴 처리
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// 선택된 메뉴 해석
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT://   메인윈도우 렌더
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:// 중지 메시지 출력하고 복귀
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// 버전정보 대화상자의 메시지 핸들러
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:// 초기화
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{// OK버튼이나 취소버튼이 눌린경우
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
