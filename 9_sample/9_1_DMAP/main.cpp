//-------------------------------------------------------------
// File: main.cpp
//
// Desc: ��������
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
// ��������
//-------------------------------------------------------------
D3DVERTEXELEMENT9 decl[] =
{
	{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
	{0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 24, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_LOOKUP,  D3DDECLUSAGE_SAMPLE,	0},
	D3DDECL_END()
};


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
	m_bDMap						= TRUE;
	m_degree					= 1.f;
	m_pMesh						= new CD3DMesh();
	m_pDispMap					= NULL;
	m_pDecl						= NULL;
	m_pEffect					= NULL;

    m_fWorldRotX                = 0;
	m_fWorldRotY                = -1;

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
    
	// ���̴� üũ
	if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1) &&
	  !(dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING ) )
		return E_FAIL;	// �������̴�

	if( pCaps->PixelShaderVersion  < D3DPS_VERSION(1,1) )
		return E_FAIL;	// �ȼ����̴�
	
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
    D3DFORMAT adptFormat = m_d3dSettings.DisplayMode().Format;

	// ���������� ������ ���ǵ��� Ȯ��
    m_bDMap = 
		// �ϵ���� ����ó��
		(m_pd3dDevice->GetSoftwareVertexProcessing()==FALSE) && 
		// �������� �����ϴ°� üũ
        (m_d3dCaps.DevCaps2 & D3DDEVCAPS2_DMAPNPATCH ) &&
		// �ؽ�ó���� Ȯ��
        SUCCEEDED( m_pD3D->CheckDeviceFormat(
								m_d3dCaps.AdapterOrdinal,
                                m_d3dCaps.DeviceType,
                                adptFormat,
                                D3DUSAGE_DMAP,
                                D3DRTYPE_TEXTURE,
                                D3DFMT_L8 ) );

    // �޽� �б�
	if( FAILED( hr=m_pMesh->Create( m_pd3dDevice, "earth.x" )))
        return DXTRACE_ERR( "Load Mesh", hr );

	
	// ���̴� �б�
    if( FAILED( hr = D3DXCreateEffectFromFile(
						m_pd3dDevice, "hlsl.fx", NULL, NULL, 
						0, NULL, &m_pEffect, NULL ) ) )
		return DXTRACE_ERR( "CreateEffectFromFile", hr );

	if(m_bDMap){
		// �������� ������Ʈ ����
		if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration(
											decl, &m_pDecl )))
			return DXTRACE_ERR ("CreateVertexDeclaration", hr);
		
		// ������ �б�
		if(FAILED(hr = D3DXCreateTextureFromFileEx(
								  m_pd3dDevice		// ����̽�
								, "earthbump.bmp"	// ���ϸ�
								, D3DX_DEFAULT		// ��
								, D3DX_DEFAULT		// ����
								, D3DX_DEFAULT		// �� ����
								, D3DUSAGE_DMAP		// ����
								, D3DFMT_L8			// ����
								, D3DPOOL_MANAGED	// �޸�Ŭ����
								, D3DX_DEFAULT		// ���͸�
								, D3DX_DEFAULT		// ���͸�
								, 0					// ����
								, NULL				// �ҽ��̹���
								, NULL				// 256�� �ȷ�Ʈ
								, &m_pDispMap
								)))
			return DXTRACE_ERR( "Load Texture", hr );

		// ���÷� ������Ʈ ����
		m_pd3dDevice->SetSamplerState( D3DDMAPSAMPLER,
						D3DSAMP_ADDRESSU,  D3DTADDRESS_CLAMP );
		m_pd3dDevice->SetSamplerState( D3DDMAPSAMPLER,
						D3DSAMP_ADDRESSV,  D3DTADDRESS_CLAMP );
		m_pd3dDevice->SetSamplerState( D3DDMAPSAMPLER,
						D3DSAMP_MIPFILTER, D3DTEXF_NONE );
		m_pd3dDevice->SetSamplerState( D3DDMAPSAMPLER,
						D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
		m_pd3dDevice->SetSamplerState( D3DDMAPSAMPLER,
						D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	}

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
	#define TSS  m_pd3dDevice->SetTextureStageState
	#define SAMP m_pd3dDevice->SetSamplerState

    // ������ ���¼���
    RS  ( D3DRS_ZENABLE,        TRUE );
    RS  ( D3DRS_LIGHTING,       FALSE );

    // �ؽ�ó ���¼���
    TSS ( 0, D3DTSS_COLOROP,    D3DTOP_MODULATE );
    TSS ( 0, D3DTSS_COLORARG1,  D3DTA_TEXTURE );
    TSS ( 0, D3DTSS_COLORARG2,  D3DTA_DIFFUSE );
    TSS ( 0, D3DTSS_ALPHAOP,    D3DTOP_MODULATE );
    TSS ( 0, D3DTSS_ALPHAARG1,  D3DTA_TEXTURE );
    TSS ( 0, D3DTSS_ALPHAARG2,  D3DTA_DIFFUSE );
	SAMP( 0, D3DSAMP_ADDRESSU,  D3DTADDRESS_WRAP );
	SAMP( 0, D3DSAMP_ADDRESSV,  D3DTADDRESS_WRAP );
    SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );


    // �������
    D3DXMatrixIdentity( &m_mWorld );
    m_pd3dDevice->SetTransform( D3DTS_WORLD,  &m_mWorld );

	// �����
    D3DXVECTOR3 vFrom   = D3DXVECTOR3( 0.0f, 0.0f, -3.5f );
    D3DXVECTOR3 vLookat = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUp     = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFrom, &vLookat, &vUp );

    // �������
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width )
				  / ((FLOAT)m_d3dsdBackBuffer.Height);
    D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect
								, 0.1f, 10.0f );

	// ������ġ ����
	m_LighPos = D3DXVECTOR4(-0.6f, 0.6f, -0.6f, 0.3f);

	
	if(m_bDMap){
		//-----------------------------------------------------
		// �������� ����Ҷ��� ����ó��
		//-----------------------------------------------------
		if( m_pMesh->m_pSysMemMesh ){
			if( FAILED( m_pMesh->m_pSysMemMesh->CloneMesh(
						0L|D3DXMESH_NPATCHES, decl,
						m_pd3dDevice,
						&m_pMesh->m_pLocalMesh )))
				return E_FAIL;
			D3DXComputeNormals( m_pMesh->m_pLocalMesh, NULL );
		}
	}else{
		m_pMesh->RestoreDeviceObjects(m_pd3dDevice);
	}
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
		m_degree += 0.01f;
	else if(m_UserInput.bX && !m_UserInput.bZ)
		m_degree -= 0.01f;

	// ��İ���
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
						0x000000ff, 1.0f, 0L );

    if( SUCCEEDED( m_pd3dDevice->BeginScene()))	// ���� ����
    {
		if(m_bDMap && m_pEffect != NULL)
		{	// �������� �Ѵ�
			//-------------------------------------------------
			// ���̴� ����
			//-------------------------------------------------
			hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
			m_pEffect->SetTechnique( hTechnique );
			m_pEffect->Begin( NULL, 0 );
			m_pEffect->Pass( 0 );
			
			//-------------------------------------------------
			// ���̴� ��� ����
			//-------------------------------------------------
			// ��ǥ��ȯ
			m = m_mWorld * m_mView * m_mProj;
		    D3DXMatrixTranspose( &m, &m );
			m_pd3dDevice->SetVertexShaderConstantF( 0,(float*)&m, 4);
			// ������ ũ��
			v = D3DXVECTOR4(m_degree,0.0f,0.0f,0.0f);
			m_pd3dDevice->SetVertexShaderConstantF( 4,(float*)&v, 1);
			// ������ ���
			v = D3DXVECTOR4(0.0f,0.5f,1.0f,2.0f);
			m_pd3dDevice->SetVertexShaderConstantF(12,(float*)&v, 1);
			// ����
			D3DXMatrixInverse( &m, NULL, &m_mWorld);
			D3DXVec4Transform( &v, &m_LighPos, &m );
			D3DXVec4Normalize( &v, &v );v.w = 0.3f;
			m_pd3dDevice->SetVertexShaderConstantF(13,(float*)&v,1 );


			//-------------------------------------------------
			// ������ ����
			//-------------------------------------------------
			m_pd3dDevice->SetTexture(D3DDMAPSAMPLER, m_pDispMap);
			m_pd3dDevice->SetNPatchMode(4);// N�p�b�`�̃��[�h
			m_pd3dDevice->SetVertexDeclaration( m_pDecl );

			//-------------------------------------------------
			// ����
			//-------------------------------------------------
			m_pMesh->Render( m_pd3dDevice );

			//-------------------------------------------------
			// ��������
			//-------------------------------------------------
			m_pd3dDevice->SetNPatchMode(0);
			m_pd3dDevice->SetVertexShader( NULL );
			m_pEffect->End();
		}else{
			// �������� ���� ����
		    m_pd3dDevice->SetTransform( D3DTS_WORLD, &m_mWorld );
			m_pd3dDevice->SetTransform( D3DTS_VIEW, &m_mView );
			m_pd3dDevice->SetTransform( D3DTS_PROJECTION, &m_mProj );

			m_pd3dDevice->SetTexture(0,m_pMesh->m_pTextures[0]);
			m_pMesh->Render( m_pd3dDevice );
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
    sprintf( szMsg, "Displacement: %f", m_degree );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Use arrow keys to rotate object") );
    fNextLine -= 20.0f; m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, TEXT("Press 'z' or 'x' to change degree of the displacement") );
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
	SAFE_RELEASE( m_pDispMap );		// ������

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




