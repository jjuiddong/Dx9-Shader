// main.cpp : �ܼ־��ø����̼��� ������
//

#include "stdafx.h"
#include "main.h"
#define MAX_LOADSTRING 100

// ----------------------------------------------------------------------------
// �ܺ��Լ�
// ----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd );
VOID Render();
VOID Cleanup();

// ��������
HINSTANCE hInst;								// �ν��Ͻ�
TCHAR szTitle[MAX_LOADSTRING];					// Ÿ��Ʋ�� �ؽ�Ʈ
TCHAR szWindowClass[MAX_LOADSTRING];			// ������ Ŭ���� ��

// ���� �ڵ��⿡�� ������� �Լ� ����
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

// ----------------------------------------------------------------------------
// ���ø����̼� �⺻����
// ----------------------------------------------------------------------------
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	// �������ڿ� �ʱ�ȭ
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MAIN, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);
	
	ZeroMemory( &msg, sizeof(msg) );
	
	// ���ø����̼� �ʱ�ȭ
	if (InitInstance (hInstance, nCmdShow)){
		
		hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MAIN);
		
		// ���� �޽��� ����
		ZeroMemory( &msg, sizeof(msg) );
		while( msg.message!=WM_QUIT ) {
			if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) ) {
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			} else {
				Render();// ����
			}
		}
	}
	
	Cleanup();// DirectX ��� ����
	
	return (int) msg.wParam;
}



//
//  �Լ� : MyRegisterClass()
//
//  ���� : ������ Ŭ���� ���
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
//   �Լ� : InitInstance(HANDLE, int)
//
//   ���� : �ν��Ͻ� �ڵ��� �����ϰ�, ������ ����
//
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // ���� �ν��Ͻ� �ڵ� ����

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 310, 330, NULL, NULL, hInstance, NULL);

   if (!hWnd) return FALSE; // ������ ���� ����

   if(FAILED(InitD3D( hWnd ))) return FALSE;    // Direct X �ʱ�ȭ

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �Լ� : WndProc(HWND, unsigned, WORD, LONG)
//
//  ���� : ������������ �޽��� ó��
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND://  	- ���ø����̼� �޴� ó��
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// ���õ� �޴� �ؼ�
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
	case WM_PAINT://   ���������� ����
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:// ���� �޽��� ����ϰ� ����
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// �������� ��ȭ������ �޽��� �ڵ鷯
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:// �ʱ�ȭ
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{// OK��ư�̳� ��ҹ�ư�� �������
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
