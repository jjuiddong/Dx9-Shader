//-------------------------------------------------------------
// File: main.h
//
// Desc: �����׸���
//-------------------------------------------------------------
#pragma once
#include "CShadowVolume.h"



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
};



//-------------------------------------------------------------
// ��üȭ�鷻�� ������
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
// Desc: ���ø����̼� Ŭ����
//-------------------------------------------------------------
class CMyD3DApplication : public CD3DApplication
{
	CBigSquare*				m_pBigSquare;	// �׸��� ������ ��üȭ�� ������
	CD3DMesh*				m_pMeshBG;		// ��� ��
	CD3DMesh*				m_pMeshBox;		// ���� ��
	CShadowVolume*			m_pShadowBox;	// ���� �� �׸���

	// �V�F�[�_
	LPD3DXEFFECT			m_pEffect;		// ����Ʈ
	D3DXHANDLE				m_hmWVP;		// ����~�������
	D3DXHANDLE				m_hvPos;		// ������ġ

	BOOL					m_bLoadingApp;	// �ε���?
    CD3DFont*				m_pFont;		// ��Ʈ
    UserInput				m_UserInput;	// �Էµ�����
	
	D3DXMATRIX				m_mView;		// �����
	D3DXMATRIX				m_mProj;		// �������
	D3DXVECTOR3				m_LighPos;		// ������ġ

    FLOAT					m_fWorldRotX;	// X��ȸ��
    FLOAT					m_fWorldRotY;	// Y��ȸ��

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

