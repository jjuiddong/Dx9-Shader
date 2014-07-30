//-------------------------------------------------------------
// File: main.cpp
//
// Desc: ũ�ν�����
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
#include "framework/DXUtil.h"
#include "framework/D3DEnumeration.h"
#include "framework/D3DSettings.h"
#include "framework/D3DApp.h"
#include "framework/D3DFont.h"
#include "framework/D3DFile.h"
#include "framework/D3DUtil.h"
#include "resource.h"
#include "main.h"

// �����ũ��
#define RS   m_pd3dDevice->SetRenderState
#define TSS  m_pd3dDevice->SetTextureStageState
#define SAMP m_pd3dDevice->SetSamplerState


//-------------------------------------------------------------
// ��������
//-------------------------------------------------------------
D3DVERTEXELEMENT9 decl[] =
{
	{0,  0, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL,	0},
	{0, 24, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT,	0},
	{0, 36, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};

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
	m_pTexScene					= NULL;
	m_pSurfScene				= NULL;
	m_pTexSceneScaled			= NULL;
	m_pSurfSceneScaled			= NULL;
	m_pTexBrightPass			= NULL;
	m_pSurfBrightPass			= NULL;
	m_pTexStarSource			= NULL;
	m_pSurfStarSource			= NULL;
	ZeroMemory( m_apTexStar,  sizeof(m_apTexStar)  );
	ZeroMemory( m_apSurfStar, sizeof(m_apSurfStar) );

	m_pMesh						= new CD3DMesh();
	m_pMeshBg					= new CD3DMesh();
	m_pDecl						= NULL;
	m_pNormalMap				= NULL;

	m_pEffect					= NULL;
	m_hTechnique  				= NULL;
	m_hmWVP  					= NULL;
	m_hvLightDir  				= NULL;
	m_hvColor	  				= NULL;
	m_hvEyePos					= NULL;
	m_htDecaleTex  				= NULL;
	m_htNormalMap				= NULL;

	m_fWorldRotX                = -D3DX_PI/10;
    m_fWorldRotY                = D3DX_PI/2;
	m_fViewZoom				    = 5.0f;

	m_dwCreationWidth           = 640;
    m_dwCreationHeight          = 480;
    m_strWindowTitle            = TEXT( "main" );
    m_d3dEnumeration.AppUsesDepthBuffer   = TRUE;
	m_bStartFullscreen			= false;
	m_bShowCursorWhenFullscreen	= false;

    m_pFont                     = new CD3DFont( _T("Arial"), 12, D3DFONT_BOLD );
    m_bLoadingApp               = TRUE;

    ZeroMemory( &m_UserInput, sizeof(m_UserInput) );
}



//-----------------------------------------------------------------------------
// Name: DrawFullScreenQuad
// Desc: �簢���� ��üȭ�鿡 �����Ѵ�
//-----------------------------------------------------------------------------
void CMyD3DApplication::DrawFullScreenQuad(float fLeftU, float fTopV, float fRightU, float fBottomV)
{
    D3DSURFACE_DESC desc;
    PDIRECT3DSURFACE9 pSurf;

    // ������Ÿ�� ����(���� ����)�� ����
    m_pd3dDevice->GetRenderTarget(0, &pSurf);
    pSurf->GetDesc(&desc);
    pSurf->Release();
    FLOAT w = (FLOAT)desc.Width;
    FLOAT h = (FLOAT)desc.Height;

	typedef struct{
		float p[4]; // ��ġ��ǥ
		float t[2]; // �ؽ�ó��ǥ
	} ScreenVertex;
	ScreenVertex svQuad[4] = {
		// x       y      z     w       u       v
		{0-0.5f, 0-0.5f, 0.5f, 1.0f, fLeftU,  fTopV,},
		{w-0.5f, 0-0.5f, 0.5f, 1.0f, fRightU, fTopV,},
		{0-0.5f, h-0.5f, 0.5f, 1.0f, fLeftU,  fBottomV,},
		{w-0.5f, h-0.5f, 0.5f, 1.0f, fRightU, fBottomV,},
	};

    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);	// ���̹��� ���̱�
    m_pd3dDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
    m_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, svQuad, sizeof(ScreenVertex));
    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);	// ���̹��� ��Ȱ
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
HRESULT CMyD3DApplication::ConfirmDevice( D3DCAPS9* pCaps,
                     DWORD dwBehavior,    D3DFORMAT Format )
{
    UNREFERENCED_PARAMETER( Format );
    

	// �ȼ����̴� ����üũ
    if( pCaps->PixelShaderVersion < D3DPS_VERSION(2,0) )
		return E_FAIL;

	// �������̴� ������ ���������� ����Ʈ���� ó��
    if( pCaps->VertexShaderVersion < D3DVS_VERSION(1,1)
    &&  0==(dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING) )
			return E_FAIL;

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
    LPDIRECT3DTEXTURE9	pHeightTexture;
    D3DSURFACE_DESC desc;

	// ������ ����
    D3DUtil_CreateTexture( m_pd3dDevice,// ���̸� �б�
		_T("height.bmp"), &pHeightTexture );
    pHeightTexture->GetLevelDesc(0,&desc);// �ؽ�ó���� ���
    D3DXCreateTexture(m_pd3dDevice, desc.Width, desc.Height, 0, 0, 
        D3DFMT_X8R8G8B8, D3DPOOL_MANAGED, &m_pNormalMap);// �ؽ�ó ����
    D3DXComputeNormalMap(m_pNormalMap,	// ������ ����
        pHeightTexture, NULL, 0, D3DX_CHANNEL_RED, 5.0f);
    SAFE_RELEASE( pHeightTexture );		// ������� �ʴ� ���ҽ� ��ȯ

	// �������� ������Ʈ ����
	if( FAILED( hr = m_pd3dDevice->CreateVertexDeclaration(
										decl, &m_pDecl )))
		return DXTRACE_ERR ("CreateVertexDeclaration", hr);
	
	// ������ �б�
	if(FAILED(hr=m_pMesh  ->Create( m_pd3dDevice, _T("t-pot.x"))))
        return DXTRACE_ERR( "Load Object", hr );
        
	// ���� �б�
	if(FAILED(hr=m_pMeshBg->Create( m_pd3dDevice, _T("map.x"))))
        return DXTRACE_ERR( "Load Ground", hr );
        
	// ���̴� �б�
    LPD3DXBUFFER pErr=NULL;
    if( FAILED( hr = D3DXCreateEffectFromFile(
                m_pd3dDevice, "hlsl.fx", NULL, NULL, 
                0 , NULL, &m_pEffect, &pErr ))){
        // ���̴� �н� ����
        MessageBox( NULL, (LPCTSTR)pErr->GetBufferPointer()
                    , "ERROR", MB_OK);
    }else{
		m_hTechnique = m_pEffect->GetTechniqueByName( "TShader" );
		m_hmWVP      = m_pEffect->GetParameterByName( NULL, "mWVP" );
		m_hvLightDir = m_pEffect->GetParameterByName( NULL, "vLightDir" );
		m_hvColor    = m_pEffect->GetParameterByName( NULL, "vColor" );
		m_hvEyePos   = m_pEffect->GetParameterByName( NULL, "vEyePos" );
		m_htDecaleTex= m_pEffect->GetParameterByName( NULL, "DecaleTex" );
		m_htNormalMap= m_pEffect->GetParameterByName( NULL, "NormalMap" );
    }
    SAFE_RELEASE(pErr);

	// ��Ʈ
    m_pFont->InitDeviceObjects( m_pd3dDevice );

    return S_OK;
}

//-------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc: ȭ��ũ�Ⱑ �������� ȣ���
//       Ȯ���� �޸𸮴� InvalidateDeviceObjects()���� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RestoreDeviceObjects()
{
    HRESULT hr;

	//---------------------------------------------------------
	// ����� ������ HDR ���� ������Ÿ��
	//---------------------------------------------------------
    hr = m_pd3dDevice->CreateTexture(
				m_d3dsdBackBuffer.Width, m_d3dsdBackBuffer.Height, 
                1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F, 
                D3DPOOL_DEFAULT, &m_pTexScene, NULL);
    if( FAILED(hr) ) return hr;
	hr = m_pTexScene->GetSurfaceLevel( 0, &m_pSurfScene );
    if( FAILED(hr) ) return hr;

	//---------------------------------------------------------
	// ��ҹ���
	//---------------------------------------------------------
	// ��ҹ����� �⺻ũ��(4�� ���)
    m_dwCropWidth  = m_d3dsdBackBuffer.Width  - m_d3dsdBackBuffer.Width  % 4;
    m_dwCropHeight = m_d3dsdBackBuffer.Height - m_d3dsdBackBuffer.Height % 4;

    hr = m_pd3dDevice->CreateTexture(
				m_dwCropWidth / 4, m_dwCropHeight / 4, 
                1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F,
				D3DPOOL_DEFAULT, &m_pTexSceneScaled, NULL);
    if( FAILED(hr) ) return hr;
	hr = m_pTexSceneScaled->GetSurfaceLevel( 0, &m_pSurfSceneScaled );
    if( FAILED(hr) ) return hr;

	//---------------------------------------------------------
	// �ֵ�����
	//---------------------------------------------------------
    hr = m_pd3dDevice->CreateTexture(
				m_dwCropWidth / 4 + 2, m_dwCropHeight / 4 + 2, 
                1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
				D3DPOOL_DEFAULT, &m_pTexBrightPass, NULL);
    if( FAILED(hr) ) return hr;
	hr = m_pTexBrightPass->GetSurfaceLevel( 0, &m_pSurfBrightPass );
    if( FAILED(hr) ) return hr;
	// �ֺ��θ� �˰�ĥ�Ѵ�
	m_pd3dDevice->ColorFill( m_pSurfBrightPass, NULL
								, D3DCOLOR_ARGB(0, 0, 0, 0) );

	//---------------------------------------------------------
	// ������ �ʵ��� �帴�ϰ� ������ �ؽ�ó
	//---------------------------------------------------------
    hr = m_pd3dDevice->CreateTexture(
				m_dwCropWidth / 4 + 2, m_dwCropHeight / 4 + 2, 
                1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
				D3DPOOL_DEFAULT, &m_pTexStarSource, NULL);
    if( FAILED(hr) ) return hr;
	hr = m_pTexStarSource->GetSurfaceLevel( 0, &m_pSurfStarSource );
    if( FAILED(hr) ) return hr;
	// �ֺ��θ� �˰�ĥ�Ѵ�
	m_pd3dDevice->ColorFill( m_pSurfStarSource, NULL
								, D3DCOLOR_ARGB(0, 0, 0, 0) );

	//---------------------------------------------------------
	// �����ؽ�ó
	//---------------------------------------------------------
    for(int i=0; i < NUM_STAR_TEXTURES; i++ ) {
        hr = m_pd3dDevice->CreateTexture(
			    m_dwCropWidth /4, m_dwCropHeight / 4,
                1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F,
				D3DPOOL_DEFAULT, &m_apTexStar[i], NULL );
        if( FAILED(hr) ) return hr;
		hr = m_apTexStar[i]->GetSurfaceLevel( 0, &m_apSurfStar[i] );
		if( FAILED(hr) ) return hr;
    }

	// ����Ʈ
	if(m_pEffect) m_pEffect->OnResetDevice();

	// �޽�
	m_pMeshBg->RestoreDeviceObjects( m_pd3dDevice );

	//---------------------------------------------------------
	// �޽����� ó��
	//---------------------------------------------------------
	if( m_pMesh && m_pMesh->GetSysMemMesh() ){
		LPD3DXMESH pMesh;

		m_pMesh->GetSysMemMesh()->CloneMesh(
			m_pMesh->GetSysMemMesh()->GetOptions(), decl,
			m_pd3dDevice, &pMesh );
		D3DXComputeNormals( pMesh, NULL );
		D3DXComputeTangent( pMesh, 0, 0, 0, TRUE, NULL );

		SAFE_RELEASE(m_pMesh->m_pLocalMesh);
		m_pMesh->m_pLocalMesh = pMesh;
	}

    // ������ ���¼���
    RS( D3DRS_ZENABLE,        TRUE );
	RS( D3DRS_LIGHTING, FALSE );
    
	// �����
    D3DXVECTOR3 vFromPt   = D3DXVECTOR3( 0.0f, 0.0f, -5.0f );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, &vFromPt, &vLookatPt, &vUpVec );

    // �������
    FLOAT fAspect = ((FLOAT)m_d3dsdBackBuffer.Width) / m_d3dsdBackBuffer.Height;
    D3DXMatrixPerspectiveFovLH( &m_mProj, D3DX_PI/4, fAspect, 1.0f, 100.0f );

    // ��Ʈ
    m_pFont->RestoreDeviceObjects();

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
    D3DXMATRIX matRotY;
    D3DXMATRIX matRotX;
	D3DXMATRIX mCamera;

    if( m_UserInput.bRotateLeft && !m_UserInput.bRotateRight )
        m_fWorldRotY += m_fElapsedTime;
    else
    if( m_UserInput.bRotateRight && !m_UserInput.bRotateLeft )
        m_fWorldRotY -= m_fElapsedTime;

    if( m_UserInput.bRotateUp && !m_UserInput.bRotateDown )
        m_fWorldRotX += m_fElapsedTime;
    else
    if( m_UserInput.bRotateDown && !m_UserInput.bRotateUp )
        m_fWorldRotX -= m_fElapsedTime;

    D3DXMatrixRotationX( &matRotX, m_fWorldRotX );
    D3DXMatrixRotationY( &matRotY, m_fWorldRotY );

    D3DXMatrixMultiply( &mCamera, &matRotY, &matRotX );
	
	//---------------------------------------------------------
	// ����� ����
	//---------------------------------------------------------
	// ��
    if( m_UserInput.bZoomIn && !m_UserInput.bZoomOut )
        m_fViewZoom += m_fElapsedTime;
    else if( m_UserInput.bZoomOut && !m_UserInput.bZoomIn )
        m_fViewZoom -= m_fElapsedTime;

    m_vFromPt   = D3DXVECTOR4( 0.0f, 0.f, -m_fViewZoom,1 );
    D3DXVECTOR3 vLookatPt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec    = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &m_mView, (D3DXVECTOR3*)&m_vFromPt, &vLookatPt, &vUpVec );
	
	m_mView = mCamera * m_mView;

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
    
	pUserInput->bZoomIn      = ( m_bActive && (GetAsyncKeyState( 'Z'     )  & 0x8000) == 0x8000 );
    pUserInput->bZoomOut     = ( m_bActive && (GetAsyncKeyState( 'X'      ) & 0x8000) == 0x8000 );
}


//-------------------------------------------------------------
// Name: RenderScene()
// Desc: ȭ�� ����
//-------------------------------------------------------------
void CMyD3DApplication::RenderScene()
{
    D3DXMATRIX m, mT, mR, mView, mProj;
	D3DXMATRIX mWorld;
	D3DXVECTOR4 v, light_pos, eye_pos;
	DWORD i;

	//-----------------------------------------------------
	// ��������
	//-----------------------------------------------------
	// �������
	D3DXMatrixScaling( &m,  3.0f, 3.0f, 3.0f );
	D3DXMatrixRotationY( &mR,  D3DX_PI );
	D3DXMatrixTranslation( &mT, 0.0f,-2.0f ,0.0f );
	mWorld = m * mR * mT;

	// ��ļ���
	m_pd3dDevice->SetTransform( D3DTS_WORLD,  &mWorld);
	m_pd3dDevice->SetTransform( D3DTS_VIEW,  &m_mView );
	m_pd3dDevice->SetTransform( D3DTS_PROJECTION,  &m_mProj );

	TSS( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	TSS( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
    SAMP( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
    SAMP( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	m_pMeshBg->Render( m_pd3dDevice );

	if( m_pEffect != NULL ) 
	{
		//-------------------------------------------------
		// ���̴� ����
		//-------------------------------------------------
		m_pEffect->SetTechnique( m_hTechnique );
		m_pEffect->Begin( NULL, 0 );
		m_pEffect->Pass( 0 );
		
		//-------------------------------------------------
		// ������ ����
		//-------------------------------------------------

		// �������(ȸ��)
		D3DXMatrixRotationY( &mWorld,  m_fTime );

		// ����-������ȯ���
		m = mWorld * m_mView * m_mProj;
		m_pEffect->SetMatrix( m_hmWVP, &m );

		// ��������(������ǥ��)
		light_pos = D3DXVECTOR4( -0.577f, -0.577f, -0.577f,0);
		D3DXMatrixInverse( &m, NULL, &mWorld);
		D3DXVec4Transform( &v, &light_pos, &m );
		D3DXVec3Normalize( (D3DXVECTOR3 *)&v, (D3DXVECTOR3 *)&v );
		v.w = -0.5f;		// ȯ�汤���
		m_pEffect->SetVector( m_hvLightDir, &v );
		
		// ����(������ǥ��)
		m = mWorld * m_mView;
		D3DXMatrixInverse( &m, NULL, &m);
		v = D3DXVECTOR4( 0, 0, 0, 1);
		D3DXVec4Transform( &v, &v, &m );
		m_pEffect->SetVector( m_hvEyePos, &v );

		// ������
		m_pEffect->SetTexture( m_htNormalMap, m_pNormalMap );
		// ��������
		m_pd3dDevice->SetVertexDeclaration( m_pDecl );

		D3DMATERIAL9 *pMtrl = m_pMesh->m_pMaterials;
		for( i=0; i<m_pMesh->m_dwNumMaterials; i++ ) {
			v.x = pMtrl->Diffuse.r;
			v.y = pMtrl->Diffuse.g;
			v.z = pMtrl->Diffuse.b;
			m_pEffect->SetVector( m_hvColor, &v );
			m_pEffect->SetTexture( m_htDecaleTex, m_pMesh->m_pTextures[i] );
			m_pMesh->m_pLocalMesh->DrawSubset( i );  // ����
			pMtrl++;
		}

		m_pEffect->End();
	}
}


//-----------------------------------------------------------------------------
// Name: GetTextureCoords()
// Desc: �Է��ؽ�ó�� ����ؽ�ó���� �ؽ�ó��ǥ ���
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::GetTextureCoords( PDIRECT3DTEXTURE9 pTexSrc, RECT* pRectSrc, 
                          PDIRECT3DTEXTURE9 pTexDest, RECT* pRectDest, CoordRect* pCoords )
{
    D3DSURFACE_DESC desc;

    if( pTexSrc == NULL || pTexDest == NULL || pCoords == NULL )
		return E_INVALIDARG;

    // �⺻������ �ؽ�ó��ǥ�� �״�� ���
    pCoords->u0 = 0.0f;
    pCoords->v0 = 0.0f;
    pCoords->u1 = 1.0f; 
    pCoords->v1 = 1.0f;

    // �Է¿��� ǥ�鿡 ���� ����
    if( pRectSrc != NULL ) {
        pTexSrc->GetLevelDesc( 0, &desc );// �ؽ�ó������ ��´�
        // ���ۿ���
        pCoords->u0 += pRectSrc->left                   / desc.Width;
        pCoords->v0 += pRectSrc->top                    / desc.Height;
        pCoords->u1 -= (desc.Width  - pRectSrc->right)  / desc.Width;
        pCoords->v1 -= (desc.Height - pRectSrc->bottom) / desc.Height;
    }

    // ����� ǥ�鿡 ���� ����
    if( pRectDest != NULL ) {
        pTexDest->GetLevelDesc( 0, &desc );// �ؽ�ó������ ��´�
        // ����� ���¿� ���� �ؽ�ó��ǥ�� �����
        pCoords->u0 -= pRectDest->left                   / desc.Width;
        pCoords->v0 -= pRectDest->top                    / desc.Height;
        pCoords->u1 += (desc.Width  - pRectDest->right)  / desc.Width;
        pCoords->v1 += (desc.Height - pRectDest->bottom) / desc.Height;
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: Scene_To_SceneScaled()
// Desc: m_pTexScene�� 1/4�� �ؼ� m_pTexSceneScale�� �ִ´�
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::Scene_To_SceneScaled()
{
    // �ʰ��� �κп� �߽ɺκ��� �����Ѵ�
    CoordRect coords;
    RECT rectSrc;
    rectSrc.left   = (m_d3dsdBackBuffer.Width  - m_dwCropWidth ) / 2;
    rectSrc.top    = (m_d3dsdBackBuffer.Height - m_dwCropHeight) / 2;
    rectSrc.right  = rectSrc.left + m_dwCropWidth;
    rectSrc.bottom = rectSrc.top  + m_dwCropHeight;
    // ������Ÿ�ٿ� ���߿� �ؽ�ó��ǥ ���
    GetTextureCoords( m_pTexScene, &rectSrc, m_pTexSceneScaled, NULL, &coords );

    // �ֺ� 16�ؼ��� ���ø������� �ؼ�
	// 0.5�� �߽ɿ� ���߱����� ����
    int index=0;
    D3DXVECTOR2 offsets[MAX_SAMPLES];

    for( int y=0; y < 4; y++ ) {
        for( int x=0; x < 4; x++ ) {
            offsets[ index ].x = (x - 1.5f) / m_d3dsdBackBuffer.Width;
            offsets[ index ].y = (y - 1.5f) / m_d3dsdBackBuffer.Height;
            index++;
        }
    }
	m_pEffect->SetValue("g_avSampleOffsets", offsets, sizeof(offsets));
    
	// 16�ؼ��� ���ø��ؼ� �� ��հ��� ��ҹ��ۿ� ����ϳ�
    m_pd3dDevice->SetRenderTarget( 0, m_pSurfSceneScaled );
    m_pEffect->SetTechnique("DownScale4x4");
	m_pEffect->Begin(NULL, 0);
	m_pEffect->Pass(0);
    m_pd3dDevice->SetTexture( 0, m_pTexScene );
	DrawFullScreenQuad( coords.u0, coords.v0, coords.u1, coords.v1 );
	m_pEffect->End();

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: SceneScaled_To_BrightPass
// Desc: ��ҹ��ۿ� �����Ҷ� ���� �κи� �����Ѵ�
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::SceneScaled_To_BrightPass()
{
    // ����� ���� �����κ��� �������� ũ�� ����
    D3DSURFACE_DESC desc;
	m_pTexBrightPass->GetLevelDesc( 0, &desc );
	RECT rectDest = {0,0,desc.Width,desc.Height};
    InflateRect( &rectDest, -1, -1 );// �o�͐�̑傫�������菬��������
    m_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
    m_pd3dDevice->SetScissorRect( &rectDest );
	
	// ��üȭ�� ����
    m_pd3dDevice->SetRenderTarget( 0, m_pSurfBrightPass );
    m_pEffect->SetTechnique("BrightPassFilter");
    m_pEffect->Begin(NULL, 0);
    m_pEffect->Pass(0);
    m_pd3dDevice->SetTexture( 0, m_pTexSceneScaled );
	DrawFullScreenQuad( 0.0f, 0.0f, 1.0f, 1.0f );
    m_pEffect->End();

    m_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetGaussBlur5x5
// Desc: ���� ���϶� ���콺�� �����Ⱑ �ǵ��� ��� ���
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::GetGaussBlur5x5(
	DWORD dwD3DTexWidth, DWORD dwD3DTexHeight,
	D3DXVECTOR2* avTexCoordOffset, D3DXVECTOR4* avSampleWeight)
{
    float tu = 1.0f / (float)dwD3DTexWidth ;
    float tv = 1.0f / (float)dwD3DTexHeight ;

    float totalWeight = 0.0f;
    int index=0;
    for( int x = -2; x <= 2; x++ ) {
        for( int y = -2; y <= 2; y++ ) {
			// ����� ���� �κ��� �Ұ�
            if( 2 < abs(x) + abs(y) ) continue;

            avTexCoordOffset[index] = D3DXVECTOR2( x * tu, y * tv );
			float fx = (FLOAT)x;
			float fy = (FLOAT)y;
			avSampleWeight[index].x = avSampleWeight[index].y =
			avSampleWeight[index].z = avSampleWeight[index].w
				= expf( -(fx*fx + fy*fy)/(2*1.0f) );
            totalWeight += avSampleWeight[index].x;

            index++;
        }
    }

	// ����ġ �հ�� 1.0
    for( int i=0; i < index; i++ ) avSampleWeight[i] *= 1.0f/totalWeight;

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: BrightPass_To_StarSource
// Desc: ��ҹ��ۿ� �����Ҷ� ��������� �����ϱ� ���ؼ�
//       ���콺�� �����⸦ �Ѵ�
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::BrightPass_To_StarSource()
{
    // ����� ���� ������ ���� �������� ũ�� ����
    D3DSURFACE_DESC desc;
	m_pTexStarSource->GetLevelDesc( 0, &desc );
	RECT rectDest = {0,0,desc.Width,desc.Height};
    InflateRect( &rectDest, -1, -1 );// ����Ұ��� ũ�⸦ 1��ŭ ���δ�
    m_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, TRUE );
    m_pd3dDevice->SetScissorRect( &rectDest );

    // �ؽ�ó��ǥ ����
    CoordRect coords;
    GetTextureCoords( m_pTexBrightPass, NULL, m_pTexStarSource,
					&rectDest, &coords );

    // ���� �̹����� ũ��κ��� ���콺��������� ���
    D3DXVECTOR2 offsets[MAX_SAMPLES];
    D3DXVECTOR4 weights[MAX_SAMPLES];
    m_pTexBrightPass->GetLevelDesc( 0, &desc );
    GetGaussBlur5x5( desc.Width, desc.Height, offsets, weights );
    m_pEffect->SetValue("g_avSampleOffsets", offsets, sizeof(offsets));
    m_pEffect->SetValue("g_avSampleWeights", weights, sizeof(weights));
    
    // ���콺 ������
    m_pEffect->SetTechnique("GaussBlur5x5");
    m_pd3dDevice->SetRenderTarget( 0, m_pSurfStarSource );
    m_pEffect->Begin(NULL, 0);
    m_pEffect->Pass(0);
    m_pd3dDevice->SetTexture( 0, m_pTexBrightPass );
	DrawFullScreenQuad( coords.u0, coords.v0, coords.u1, coords.v1 );
    m_pEffect->End();

    m_pd3dDevice->SetRenderState( D3DRS_SCISSORTESTENABLE, FALSE );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RenderStar()
// Desc: ���������
//-----------------------------------------------------------------------------
HRESULT CMyD3DApplication::RenderStar()
{
    // ����Ʈ���� ����ϴ� ��� ����
    static const s_maxPasses = 3;
    static const int nSamples = 8;
	// ������
    static const D3DXCOLOR s_colorWhite(0.63f, 0.63f, 0.63f, 0.0f) ;
	static const D3DXCOLOR s_ChromaticAberrationColor[8] = {
		D3DXCOLOR(0.5f, 0.5f, 0.5f,  0.0f),	// ��
		D3DXCOLOR(0.8f, 0.3f, 0.3f,  0.0f), // ��
		D3DXCOLOR(1.0f, 0.2f, 0.2f,  0.0f),	// ��
		D3DXCOLOR(0.5f, 0.2f, 0.6f,  0.0f), // ��
		D3DXCOLOR(0.2f, 0.2f, 1.0f,  0.0f),	// ��
		D3DXCOLOR(0.2f, 0.3f, 0.7f,  0.0f), // ��
		D3DXCOLOR(0.2f, 0.6f, 0.2f,  0.0f),	// ��
		D3DXCOLOR(0.3f, 0.5f, 0.3f,  0.0f), // ��
	} ;

    static D3DXVECTOR4 s_aaColor[s_maxPasses][nSamples];

    for (int p = 0 ; p < s_maxPasses ; p ++) {
		// �߽ɿ����� �Ÿ������� �������� �����
        float ratio = (float)(p + 1) / (float)s_maxPasses ;
        // ���� ���ø��ؼ� ������ ���� �����
        for (int s = 0 ; s < nSamples ; s ++) {
            D3DXCOLOR chromaticAberrColor ;
            D3DXColorLerp(&chromaticAberrColor,
				&( s_ChromaticAberrationColor[s] ),
                &s_colorWhite, ratio) ;
			// ��ü���� �� ��ȭ�� ����
            D3DXColorLerp( (D3DXCOLOR*)&( s_aaColor[p][s] ),
                &s_colorWhite, &chromaticAberrColor, 0.7f ) ;
        }
    }

    float radOffset = m_fWorldRotY/5;// ������ ���� ȸ��

    // ����ȭ���� ���� ���� ����
    D3DSURFACE_DESC desc;
    m_pSurfStarSource->GetDesc( &desc );
    float srcW = (FLOAT) desc.Width;
    float srcH = (FLOAT) desc.Height;
	
	int nStarLines = 6;// ������ �ٱ� ����
    for (int d = 0 ; d < nStarLines ; d ++) {    // ���⿡ ���� ����
        PDIRECT3DTEXTURE9 pTexSource = m_pTexStarSource;
        float rad = radOffset + 2*d*D3DX_PI/(FLOAT)nStarLines;// ����
        float sn = sinf(rad);
        float cs = cosf(rad);
	    D3DXVECTOR2 vtStepUV = D3DXVECTOR2(0.3f * sn / srcW,
										   0.3f * cs / srcH);
        
        float attnPowScale = (atanf(D3DX_PI/4) + 0.1f) *
                       (160.0f + 120.0f) / (srcW + srcH);

        int iWorkTexture = 0;
        for (int p = 0 ; p < s_maxPasses; p++) {
			// �������� �� ����
		    PDIRECT3DSURFACE9 pSurfDest = NULL;
            if (p == s_maxPasses - 1) {
                // ���� �н��� ������ ���ۿ� Ȯ���Ѵ�
                pSurfDest = m_apSurfStar[d+2];
            } else {
                pSurfDest = m_apSurfStar[iWorkTexture];
            }
            m_pd3dDevice->SetRenderTarget( 0, pSurfDest );

            //�ؽ�ó��ǥ�� �ռ��Ҷ��� ����ġ�� ���
			D3DXVECTOR4 avSampleWeights[MAX_SAMPLES];
			D3DXVECTOR2 avSampleOffsets[MAX_SAMPLES];

            for (int i = 0 ; i < nSamples ; i++) {
                // ������ ����ġ
                float lum = powf( 0.95f, attnPowScale * i );
                avSampleWeights[i] = s_aaColor[s_maxPasses-1-p][i]
                                * lum * (p+1.0f) * 0.5f ;
                                
                // �ؽ�ó��ǥ�� �ø� ��
                avSampleOffsets[i].x = vtStepUV.x * i ;
                avSampleOffsets[i].y = vtStepUV.y * i ;
                if ( 0.9f <= fabs(avSampleOffsets[i].x) ||
                     0.9f <= fabs(avSampleOffsets[i].y) ) {
                    avSampleOffsets[i].x = 0.0f ;
                    avSampleOffsets[i].y = 0.0f ;
                    avSampleWeights[i] *= 0.0f ;
                }
            }
            m_pEffect->SetValue("g_avSampleOffsets", avSampleOffsets, sizeof(avSampleOffsets));
            m_pEffect->SetVectorArray("g_avSampleWeights", avSampleWeights, nSamples);
            
            // ��üȭ�� ����
            m_pEffect->SetTechnique("Star");
            m_pEffect->Begin(NULL, 0);
            m_pEffect->Pass(0);
            m_pd3dDevice->SetTexture( 0, pTexSource );
            DrawFullScreenQuad(0.0f, 0.0f, 1.0f, 1.0f);
			m_pEffect->End();

            // ���� �н��� ���� �Ķ���� ����
            vtStepUV *= nSamples;
            attnPowScale *= nSamples;

            // �������� ����� ���� �ؽ�ó�� �Ѵ�
            pTexSource = m_apTexStar[iWorkTexture];
			
			iWorkTexture ^= 1;
        }
    }


    // ��� ������ �ռ�
    m_pd3dDevice->SetRenderTarget( 0, m_apSurfStar[0] );
  
	m_pEffect->SetTechnique("MergeTextures");
    m_pEffect->Begin(NULL, 0);
    m_pEffect->Pass(0);
	m_pd3dDevice->SetTexture( 0, m_apTexStar[0+2] );
	m_pd3dDevice->SetTexture( 1, m_apTexStar[1+2] );
	m_pd3dDevice->SetTexture( 2, m_apTexStar[2+2] );
	m_pd3dDevice->SetTexture( 3, m_apTexStar[3+2] );
	m_pd3dDevice->SetTexture( 4, m_apTexStar[4+2] );
	m_pd3dDevice->SetTexture( 5, m_apTexStar[5+2] );

	DrawFullScreenQuad(0.0f, 0.0f, 1.0f, 1.0f);

    m_pEffect->End();

    return S_OK;
}
//-------------------------------------------------------------
// Name: Render()
// Desc: ȭ�� ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::Render()
{

    PDIRECT3DSURFACE9 pBackBuffer;
    
    // HDR������ Ÿ�� ����
    m_pd3dDevice->GetRenderTarget(0, &pBackBuffer);// ���
    m_pd3dDevice->SetRenderTarget(0, m_pSurfScene);// ������ ������ ��ȯ

	//---------------------------------------------------------
	// ����
	//---------------------------------------------------------
    if( SUCCEEDED( m_pd3dDevice->BeginScene() ) )
    {
		// ������Ÿ�� Ŭ����
		m_pd3dDevice->Clear(0L, NULL
						, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER
						, 0x0060c0, 1.0f, 0L);

		// ������ ���� ȭ�鿡 ����
		this->RenderScene();
		
        this->Scene_To_SceneScaled();     // ��ҹ��ۿ� ����� ����
        this->SceneScaled_To_BrightPass();// �����κ� ����
        this->BrightPass_To_StarSource(); // ������ ���� ������
        this->RenderStar();               // ���� ����

		// ������Ÿ�� ����
        m_pd3dDevice->SetRenderTarget(0, pBackBuffer);
	    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

		// �������� LDR�� �ٿ���Ѽ� ����
		float w = (float)this->m_d3dsdBackBuffer.Width ;
		float h = (float)this->m_d3dsdBackBuffer.Height;
		typedef struct{float x,y,z,w,u,v;}TVERTEX;
		TVERTEX VertexFinal[4] = {
			//x  y  z rhw    tu        tv
			{ 0, 0, 0, 1, 0+0.5f/w, 0+0.5f/h,},
			{ w, 0, 0, 1, 1+0.5f/w, 0+0.5f/h,},
			{ w, h, 0, 1, 1+0.5f/w, 1+0.5f/h,},
			{ 0, h, 0, 1, 0+0.5f/w, 1+0.5f/h,},
		};
		m_pd3dDevice->SetFVF(D3DFVF_XYZRHW| D3DFVF_TEX1);
		m_pd3dDevice->SetTexture( 0, m_pTexScene );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
		                  , 2, VertexFinal, sizeof( TVERTEX ) );

		// ������ �����ռ�
		m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE) ;
		m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE) ;
        m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE) ;
		m_pd3dDevice->SetTexture( 0, m_apTexStar[0] );
		m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
		                  , 2, VertexFinal, sizeof( TVERTEX ) );
        m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE) ;


#if 0	// ����׿� �ؽ�ó ���
		for(int i=0;i<4;i++){
			TVERTEX VertexFinal2[4] = {
				//x  y  z rhw    tu        tv
				{ 0,     0+h*(FLOAT)i/8, 0, 1, 0+0.5f/w, 0+0.5f/h,},
				{ w/8,   0+h*(FLOAT)i/8, 0, 1, 1+0.5f/w, 0+0.5f/h,},
				{ w/8, h/8+h*(FLOAT)i/8, 0, 1, 1+0.5f/w, 1+0.5f/h,},
				{ 0,   h/8+h*(FLOAT)i/8, 0, 1, 0+0.5f/w, 1+0.5f/h,},
			};
			m_pd3dDevice->SetFVF(D3DFVF_XYZRHW| D3DFVF_TEX1);
			switch(i){
			case 0:m_pd3dDevice->SetTexture( 0, m_pTexSceneScaled );break;
			case 1:m_pd3dDevice->SetTexture( 0, m_pTexBrightPass );break;
			case 2:m_pd3dDevice->SetTexture( 0, m_pTexStarSource );break;
			case 3:m_pd3dDevice->SetTexture( 0, m_apTexStar[0] );break;
			}
			m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLEFAN
								, 2, VertexFinal2, sizeof( TVERTEX ) );
		}
#endif

		// �����·� ����
	    m_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
        
		RenderText();				// ���� ���

        m_pd3dDevice->EndScene();	// ��������
    }

    // ǥ�� ����
    SAFE_RELEASE(pBackBuffer);

    return S_OK;
}




//-------------------------------------------------------------
// Name: RenderText()
// Desc: ���¿� ���� ���
//-------------------------------------------------------------
HRESULT CMyD3DApplication::RenderText()
{
    D3DCOLOR fontColor        = D3DCOLOR_ARGB(255,255,255,0);
    TCHAR szMsg[MAX_PATH] = TEXT("");

    FLOAT fNextLine = 40.0f; // ��³���

    // ���۹��
    fNextLine = (FLOAT) m_d3dsdBackBuffer.Height; 
    lstrcpy( szMsg, TEXT("Press 'F2' to configure display") );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );

    lstrcpy( szMsg, m_strDeviceStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
    lstrcpy( szMsg, m_strFrameStats );
    fNextLine -= 20.0f;
    m_pFont->DrawText( 2, fNextLine, fontColor, szMsg );
	
	return S_OK;
}




//-------------------------------------------------------------
// Name: MsgProc()
// Desc: WndProc �������̵�
//-------------------------------------------------------------
LRESULT CMyD3DApplication::MsgProc( HWND hWnd, UINT msg,
                                 WPARAM wParam, LPARAM lParam )
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
                DrawText( hDC, strMsg, -1, &rct
                		, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
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
	SAFE_RELEASE(m_pSurfScene);
	SAFE_RELEASE(m_pTexScene);
	SAFE_RELEASE(m_pSurfSceneScaled);
	SAFE_RELEASE(m_pTexSceneScaled);
	SAFE_RELEASE(m_pSurfBrightPass);
	SAFE_RELEASE(m_pTexBrightPass);
	SAFE_RELEASE(m_pSurfStarSource);
	SAFE_RELEASE(m_pTexStarSource);
    for(int i=0; i < NUM_STAR_TEXTURES; i++ ) {
        SAFE_RELEASE(m_apSurfStar[i]);
        SAFE_RELEASE(m_apTexStar[i]);
    }

	m_pMesh  ->InvalidateDeviceObjects(); // �޽�
	m_pMeshBg->InvalidateDeviceObjects();

    m_pFont->InvalidateDeviceObjects();	// ��Ʈ

	// ���̴�
    if( m_pEffect != NULL ) m_pEffect->OnLostDevice();

    return S_OK;
}




//-------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: InitDeviceObjects() ���� ������ ������Ʈ ����
//-------------------------------------------------------------
HRESULT CMyD3DApplication::DeleteDeviceObjects()
{
	SAFE_RELEASE( m_pEffect );      // ���̴�
	SAFE_RELEASE( m_pDecl );		// ��������
	SAFE_RELEASE( m_pNormalMap );
	
	// �޽�
	m_pMesh  ->Destroy();
	m_pMeshBg->Destroy();

    // ��Ʈ
    m_pFont->DeleteDeviceObjects();

    return S_OK;
}




//-------------------------------------------------------------
// Name: FinalCleanup()
// Desc: ���������� ȣ���
//-------------------------------------------------------------
HRESULT CMyD3DApplication::FinalCleanup()
{
    SAFE_DELETE( m_pMeshBg ); // �޽�
	SAFE_DELETE( m_pMesh );

    SAFE_DELETE( m_pFont );	// ��Ʈ

    return S_OK;
}




