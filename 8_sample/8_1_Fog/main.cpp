//-------------------------------------------------------------
// File: main.cpp
//
// Desc: ���� ����
//-------------------------------------------------------------
#define STRICT
#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <tchar.h>
#include "DXUtil.h"
#include "D3DEnumeration.h"
#include "D3DSettings.h"
#include "D3DApp.h"
#include "D3DFont.h"
#include "D3DFile.h"
#include "D3DUtil.h"
#include "resource.h"
#include "main.h"


//-------------------------------------------------------------
// ��������
//-------------------------------------------------------------
CMyD3DApplication* g_pApp  = NULL;
HINSTANCE          g_hInst = NULL;


//-------------------------------------------------------------
// Name: WinMain()
// Desc: �����Լ�
//-------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    CMyD3DApplication d3dApp;

    g_pApp  = &d3dApp;
    g_hInst = hInst;

    InitCommonControls();
    if( FAILED( d3dApp.Create( hInst ) ) )
        return 0;

    return d3dApp.Run();
}




//-------------------------------------------------------------
// Name: CMyD3DApplication()
// Desc: ���ø����̼� ������
//-------------------------------------------------------------
CMyD3DApplication::CMyD3DApplication()
{
	m_pMesh						= new CD3DMesh();
	m_pDecl						= NULL;
	m_pEffect					= NULL;
	m_hmWVP						= NULL;
	m_hvCol						= NULL;
	m_hvDir						= NULL;
	m_hvFog						= NULL;

	m_near						=  2.0f;
	m_far						= 15.0f;
    m_fWorldRotX                = -0.5f;
	m_fWorldRotY                = D3DX_PI;

	m_dwCreationWidth           = 500;
    m_dwCreationHeight          = 375;
    m_strWindowTitle            = TEXT( "main" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;
    
	ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
}




//-------------------------------------------------------------
// Name: ~CMyD3DApplication()
// Desc: �Ҹ���
//-------------------------------------------------------------
CMyD3DApplication::~CMyD3DApplication()
{
}




//-------------------------------------------------------------
// Name: OneTimeSceneInit()
// Desc: �� �ѹ��� �ʱ�ȭ
//       ������ �ʱ�ȭ�� IDirect3D9�ʱ�ȭ�� ������
//       �׷��� LPDIRECT3DDEVICE9�ʱ�ȭ�� ������ ���� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::OneTimeSceneInit()
{
    // �ε� �޽��� ���
    SendMessage( m_hWnd, WM_PAINT, 0, 0 );

    m_bLoadingApp = FALSE;

    return S_OK;
}




//-------------------------------------------------------------
// Name: ConfirmDevice()
// Desc: �ʱ�ȭ�� ȣ���. �ʿ��� �ɷ�(caps)üũ
//-------------------------------------------------------------
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps
						, DWORD dwBehavior, D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    UNREFERENCED_PARAMETER( dwBehavior );
    UNREFERENCED_PARAMETER( pCaps );
    
	// ���̴�üũ
	if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) &&
	  !(dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) )
		return E_FAIL;	// �������̴�

	return S_OK;
}


//-------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: ����̽��� ���������� �ʱ�ȭ
//       �����ӹ��� ���˰� ����̽� ������ ���ѵڿ� ȣ��
//       ���⼭ Ȯ���� �޸𸮴� DeleteDeviceObjects()���� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::InitDeviceObjects()
{
    HRESULT hr;

    // �޽��б�
	if( FAILED( hr=m_pMesh->Create( m_pd3dDevice, "map.x" )))
        return DXTRACE_ERR( "Load Mesh", hr );
	m_pMesh->SetFVF( m_pd3dDevice, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 );
	
	// �������� ������Ʈ ����
	D3DVERTEXELEMENT9 decl[] =
	{
		{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
		{0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration(
										decl, &m_pDecl )))
		return DXTRACE_ERR ("CreateVertexDeclaration", hr);

	// ���̴��б�
    if( FAILED( hr = D3DXCreateEffectFromFile(
						m_pd3dDevice, "hlsl.fx", NULL, NULL, 
						0, NULL, &m_pEffect, NULL ) ) )
		return DXTRACE_ERR( "CreateEffectFromFile", hr );
	m_hmWVP = m_pEffect->GetParameterByName( NULL, "mWVP" );
	m_hvCol = m_pEffect->GetParameterByName( NULL, "vCol" );
	m_hvDir = m_pEffect->GetParameterByName( NULL, "vLightDir" );
	m_hvFog = m_pEffect->GetParameterByName( NULL, "vFog" );

    m_pFont->InitDeviceObjects( m_pd3dDevice );// ��Ʈ
    
	return S_OK;
}

//-------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: ȭ��ũ�Ⱑ �������� ȣ���
//       Ȯ���� �޸𸮴� InvalidateDeviceObjects()���� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
	// �����ũ��
	#define RS   m_pd3dDevice->SetRenderState
	#define SAMP m_pd3dDevice->SetSamplerState

    // ������ ���¼���
    RS  ( D3DRS_ZENABLE,        TRUE );
    RS  ( D3DRS_LIGHTING,       FALSE );

    // �ؽ�ó ����
	SAMP( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
	SAMP( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );
    SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );


    // �������
    D3DXMatrixIdentity( &m_mWorld );

	// �����
    D3DXVECTOR3 vFrom   = D3DXVECTOR3( 0.0f, 0.0f, -10.0f );
    D3DXVECTOR3 vLookat = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFrom, &vLookat, &vUp );

    // �������
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width )
				  / ((FLOAT)m_d3dsdBackBuffer.Height);
    D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect
								, 1.0f, 100.0f );

	// �������� ����
	m_LightDir = D3DXVECTOR4(-0.6f, 0.6f, -0.6f, 0.3f);

	
	m_pMesh->RestoreDeviceObjects(m_pd3dDevice);
	if( m_pEffect!=NULL ) m_pEffect->OnResetDevice();// ���̴�

    m_pFont->RestoreDeviceObjects();	// ��Ʈ

    return S_OK;
}




//-------------------------------------------------------------
// Name: FrameMove()
// Desc: �� �����Ӹ��� ȣ���. �ִϸ��̼� ó���� ���
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FrameMove()
{
	UpdateInput( &m_UserInput ); // �Էµ����� ����

	//---------------------------------------------------------
	// �Է¿� ���� ��ǥ�踦 �����Ѵ�
	//---------------------------------------------------------
	// ȸ��
    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;
	// ��
	if(m_UserInput.bZ && !m_UserInput.bX)
		m_near += 0.01f;
	else if(m_UserInput.bX && !m_UserInput.bZ)
		m_near -= 0.01f;

	// ��
	if(m_UserInput.bA && !m_UserInput.bS)
		m_far += 0.01f;
	else if(m_UserInput.bS && !m_UserInput.bA)
		m_far -= 0.01f;

	//---------------------------------------------------------
	// ��İ���
	//---------------------------------------------------------
 	D3DXMATRIX matRotX, matRotY;
    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );
    D3DXMatrixMultiply( &m_mWorld, &matRotY, &matRotX );

    return S_OK;
}




//-------------------------------------------------------------
// Name: UpdateInput()
// Desc: �Էµ����� ����
//-------------------------------------------------------------
void CMyD3DApplication::UpdateInput( UserInput* pUserInput )
{
    pUserInput->bRotateUp    = ( m_bActive && (GetAsyncKeyState( VK_UP )    & 0x8000) == 0x8000 );
    pUserInput->bRotateDown  = ( m_bActive && (GetAsyncKeyState( VK_DOWN )  & 0x8000) == 0x8000 );
    pUserInput->bRotateLeft  = ( m_bActive && (GetAsyncKeyState( VK_LEFT )  & 0x8000) == 0x8000 );
    pUserInput->bRotateRight = ( m_bActive && (GetAsyncKeyState( VK_RIGHT ) & 0x8000) == 0x8000 );
	pUserInput->bZ = ( m_bActive && (GetAsyncKeyState( 'Z' ) & 0x8000) == 0x8000 );
	pUserInput->bX = ( m_bActive && (GetAsyncKeyState( 'X' ) & 0x8000) == 0x8000 );
	pUserInput->bA = ( m_bActive && (GetAsyncKeyState( 'A' ) & 0x8000) == 0x8000 );
	pUserInput->bS = ( m_bActive && (GetAsyncKeyState( 'S' ) & 0x8000) == 0x8000 );
}




//-------------------------------------------------------------
// Name: Render()
// Desc: ȭ�� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{
    D3DXHANDLE hTechnique;
	D3DXMATRIX m;
	D3DXVECTOR4 v;

	// ȭ�� Ŭ����
    m_pd3dDevice->Clear( 0L, NULL,
						D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
						0xffffff, 1.0f, 0L );

    if( SUCCEEDED( m_pd3dDevice->BeginScene()))	// ��������
    {
		if(m_pEffect != NULL)
		{
			//-------------------------------------------------
			// ���̴� ����
			//-------------------------------------------------
			hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
			m_pEffect->SetTechnique( hTechnique );
			m_pEffect->Begin( NULL, 0 );
			m_pEffect->Pass( 0 );
			m_pd3dDevice->SetVertexDeclaration( m_pDecl );
			
			//-------------------------------------------------
			// ���̴� ��� ����
			//-------------------------------------------------
			// ��ǥ��ȯ
			m = m_mWorld * m_mView * m_mProj;
			if( m_hmWVP != NULL ) m_pEffect->SetMatrix( m_hmWVP, &m );

			// ����
			v.x = m_far/(m_far-m_near);
			v.y = -1.0f/(m_far-m_near);
			if( m_hvFog != NULL ) m_pEffect->SetVector( m_hvFog, &v );

			// ����
			D3DXMatrixInverse( &m, NULL, &m_mWorld);
			D3DXVec4Transform( &v, &m_LightDir, &m );
			D3DXVec4Normalize( &v, &v );v.w = 0.3f;
			if( m_hvDir != NULL ) m_pEffect->SetVector( m_hvDir, &v );

			//-------------------------------------------------
			// ����
			//-------------------------------------------------
			D3DMATERIAL9 *pMtrl = m_pMesh->m_pMaterials;
			for( DWORD i=0; i<m_pMesh->m_dwNumMaterials; i++ ) {
				v.x = pMtrl->Diffuse.r;
				v.y = pMtrl->Diffuse.g;
				v.z = pMtrl->Diffuse.b;
				v.w = pMtrl->Diffuse.a;
				if( m_hvCol != NULL ) m_pEffect->SetVector( m_hvCol, &v );
				m_pd3dDevice->SetTexture( 0, m_pMesh->m_pTextures[i] );
				m_pMesh->m_pLocalMesh->DrawSubset( i );  // �`��
				pMtrl++;
			}

			m_pEffect->End();
		}

		RenderText();				// ���� ���

        m_pd3dDevice->EndScene();	// ��������
    }

    return S_OK;
}




//-------------------------------------------------------------
// Name: RenderText()
// Desc: ���¿� ������ ȭ�鿡 ���
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    FLOAT fNextLine = 40.0f; 

    // ���÷��� ���� ���
    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    // ���۹� & �μ� ���
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
	sprintf( szMsg, "Near: %f  Far: %f", m_near, m_far );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Use arrow keys to rotate object") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Press 'z'-'x' or 'a'-'s' to change the parameters of the fog") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    
	return S_OK;
}




//-------------------------------------------------------------
// Name: MsgProc()
// Desc: WndProc �������̵�
//-------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd,
					UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_PAINT:
        {
            if( m_bLoadingApp )
            {
                // �ε���
                HDC hDC = GetDC( hWnd );
                TCHAR strMsg[MAX_PATH];
                wsprintf(strMsg, TEXT("Loading... Please wait"));
                RECT rct;
                GetClientRect( hWnd, &rct );
                DrawText( hDC, strMsg, -1, &rct,
						DT_CENTER|DT_VCENTER|DT_SINGLELINE );
                ReleaseDC( hWnd, hDC );
            }
            break;
        }

    }

    return CD3DApplication::MsgProc( hWnd, msg, wParam, lParam );
}




//-------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: RestoreDeviceObjects() ���� ������ ������Ʈ ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::InvalidateDeviceObjects()
{
	m_pMesh->InvalidateDeviceObjects();				// �޽�
    if(m_pEffect!=NULL) m_pEffect->OnLostDevice();	// ���̴�

    m_pFont->InvalidateDeviceObjects();	// ��Ʈ

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() ���� ������ ������Ʈ ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	m_pMesh->Destroy();				// �޽�
	SAFE_RELEASE( m_pEffect );		// ���̴�
	SAFE_RELEASE( m_pDecl );		// ��������

    m_pFont->DeleteDeviceObjects();	// ��Ʈ

	return S_OK;
}




//-------------------------------------------------------------
// Name: FinalCleanup()
// Desc: ���������� ȣ���
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    SAFE_DELETE( m_pMesh );	// �޽�

    SAFE_DELETE( m_pFont );	// ��Ʈ

    return S_OK;
}




