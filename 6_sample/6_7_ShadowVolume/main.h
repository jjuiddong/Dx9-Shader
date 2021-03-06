//-------------------------------------------------------------
// File: main.h
//
// Desc: 볼륨그림자
//-------------------------------------------------------------
#pragma once
#include "CShadowVolume.h"



//-------------------------------------------------------------
// 정의 & 상수
//-------------------------------------------------------------
// 입력데이터를 보존할 구조체
struct UserInput
{
    BOOL bRotateUp;
    BOOL bRotateDown;
    BOOL bRotateLeft;
    BOOL bRotateRight;
};



//-------------------------------------------------------------
// 전체화면렌더 폴리곤
//-------------------------------------------------------------
class CBigSquare {
private:
	typedef struct {
		D3DXVECTOR4 p;
		D3DCOLOR	color;
	} SHADOWVERTEX;
	static const DWORD FVF;

	LPDIRECT3DVERTEXBUFFER9 m_pVB;
public:
	CBigSquare();
	HRESULT		Create( LPDIRECT3DDEVICE9 pd3dDevice );
	void		RestoreDeviceObjects( FLOAT sx, FLOAT sy );
	void		Destroy();
	void		Render( LPDIRECT3DDEVICE9 pd3dDevice );
};




//-------------------------------------------------------------
// Name: class CMyD3DApplication
// Desc: 어플리케이션 클래스
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	CBigSquare*				m_pBigSquare;	// 그림자 렌더용 전체화면 폴리곤
	CD3DMesh*				m_pMeshBG;		// 배경 모델
	CD3DMesh*				m_pMeshBox;		// 상자 모델
	CShadowVolume*			m_pShadowBox;	// 상자 모델 그림자

	// 긘긃�[�_
	LPD3DXEFFECT			m_pEffect;		// 이펙트
	D3DXHANDLE				m_hmWVP;		// 로컬~투영행렬
	D3DXHANDLE				m_hvPos;		// 광원위치

	BOOL					m_bLoadingApp;	// 로드중?
    CD3DFont*				m_pFont;		// 폰트
    UserInput				m_UserInput;	// 입력데이터
	
	D3DXMATRIX				m_mView;		// 뷰행렬
	D3DXMATRIX				m_mProj;		// 투영행렬
	D3DXVECTOR3				m_LighPos;		// 광원위치

    FLOAT					m_fWorldRotX;	// X축회전
    FLOAT					m_fWorldRotY;	// Y축회전

protected:
    virtual HRESULT OneTimeSceneInit();
    virtual HRESULT InitDeviceObjects();
    virtual HRESULT RestoreDeviceObjects();
    virtual HRESULT InvalidateDeviceObjects();
    virtual HRESULT DeleteDeviceObjects();
    virtual HRESULT Render();
    virtual HRESULT FrameMove();
    virtual HRESULT FinalCleanup();
    virtual HRESULT ConfirmDevice( D3DCAPS9*, DWORD, D3DFORMAT, D3DFORMAT );

    HRESULT RenderText();

    void    UpdateInput( UserInput* pUserInput );
public:
    LRESULT MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
    CMyD3DApplication();
    virtual ~CMyD3DApplication();
};

