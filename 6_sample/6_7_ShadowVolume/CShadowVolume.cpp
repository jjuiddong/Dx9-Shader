
#include <assert.h>
#include "D3DFile.h"
#include "DXUtil.h"
#include "CShadowVolume.h"

// ---------------------------------------------------------------------------
// 静的オブジェクト
// ---------------------------------------------------------------------------
const DWORD CShadowVolume::FVF = D3DFVF_XYZ | D3DFVF_NORMAL;

// ---------------------------------------------------------------------------
CShadowVolume::CShadowVolume()
{
    m_pVertices = NULL;
	m_dwNumFaces = 0;
}
// ---------------------------------------------------------------------------
CShadowVolume::~CShadowVolume()
{
}
// ---------------------------------------------------------------------------
// 描画
// ---------------------------------------------------------------------------
VOID CShadowVolume::Render( LPDIRECT3DDEVICE9 pd3dDevice )
{
	pd3dDevice->SetFVF( CShadowVolume::FVF );

	pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLELIST, m_dwNumFaces,
								m_pVertices, sizeof(SHADOW_VOLUME_VERTEX) );
}
// ---------------------------------------------------------------------------
// 生成
// ---------------------------------------------------------------------------
HRESULT CShadowVolume::Create( LPD3DXMESH pMesh )
{
	HRESULT ret = S_OK;
    WORD*       pIndices;
	DWORD i,j,k,l, face;

	// 出力用のメモリの確保
	struct MESHVERTEX { D3DXVECTOR3 p, n; };
	SHADOW_VOLUME_VERTEX* pVertices;
	DWORD dwNumFaces    = pMesh->GetNumFaces();

	m_dwNumFaces = 4*dwNumFaces;
	m_pVertices = new SHADOW_VOLUME_VERTEX[3*m_dwNumFaces];

	// バッファを専有
    pMesh->LockVertexBuffer( 0L, (LPVOID*)&pVertices );
    pMesh->LockIndexBuffer ( 0L, (LPVOID*)&pIndices );
    
	// 法線保存用
    D3DXVECTOR3 *vNormal = new D3DXVECTOR3[dwNumFaces];
	if(NULL==vNormal){
		m_dwNumFaces = 0;
        ret = E_OUTOFMEMORY;
		goto end;
	}
	// 通常データ製作
    for( i=0; i<dwNumFaces; i++ )
	{
        D3DXVECTOR3 v0 = pVertices[pIndices[3*i+0]].p;
        D3DXVECTOR3 v1 = pVertices[pIndices[3*i+1]].p;
        D3DXVECTOR3 v2 = pVertices[pIndices[3*i+2]].p;

        D3DXVECTOR3 vCross1(v1-v0);
        D3DXVECTOR3 vCross2(v2-v1);
        D3DXVec3Cross( &vNormal[i], &vCross1, &vCross2 );
		D3DXVec3Normalize( &vNormal[i], &vNormal[i] );
		
		m_pVertices[3*i+0].p = v0;
		m_pVertices[3*i+1].p = v1;
		m_pVertices[3*i+2].p = v2;
		m_pVertices[3*i+0].n = vNormal[i];
		m_pVertices[3*i+1].n = vNormal[i];
		m_pVertices[3*i+2].n = vNormal[i];
    }
	
	// 稜線に挟み込むデータ製作
	face = dwNumFaces;
	for( i=0  ; i<dwNumFaces; i++ ){
	for( j=i+1; j<dwNumFaces; j++ ){
		DWORD id[2][2];
		DWORD cnt=0;
		for(k=0;k<3;k++){
		for(l=0;l<3;l++){
			D3DXVECTOR3 dv;
			D3DXVec3Subtract( &dv,	&pVertices[pIndices[3*i+k]].p,
									&pVertices[pIndices[3*j+l]].p);
			if( D3DXVec3LengthSq( &dv ) < 0.001f ){
				// 頂点の位置が同じデータを検索
				id[cnt][0] = 3*i+k;
				id[cnt][1] = 3*j+l;
				cnt++;
			}
		}
		}
		if(2==cnt){
			// 共有稜線があった
			if(id[1][0]-id[0][0]!=1){
				// ポリゴンの表向きを調整するための順番ずらし
				DWORD tmp = id[0][0];
				id[0][0] = id[1][0];
				id[1][0] = tmp;
				tmp = id[0][1];
				id[0][1] = id[1][1];
				id[1][1] = tmp;
			}
			// 稜線にポリゴンを埋め込む
			m_pVertices[3*face+0].p = pVertices[pIndices[id[1][0]]].p;
			m_pVertices[3*face+2].p = pVertices[pIndices[id[0][1]]].p;
			m_pVertices[3*face+1].p = pVertices[pIndices[id[0][0]]].p;
			m_pVertices[3*face+0].n = vNormal[i];
			m_pVertices[3*face+2].n = vNormal[j];
			m_pVertices[3*face+1].n = vNormal[i];
			face++;
			m_pVertices[3*face+0].p = pVertices[pIndices[id[1][0]]].p;
			m_pVertices[3*face+2].p = pVertices[pIndices[id[1][1]]].p;
			m_pVertices[3*face+1].p = pVertices[pIndices[id[0][1]]].p;
			m_pVertices[3*face+0].n = vNormal[i];
			m_pVertices[3*face+2].n = vNormal[j];
			m_pVertices[3*face+1].n = vNormal[j];
			face++;
		}
	}
	}
	assert(face == m_dwNumFaces);

	delete[] vNormal;
end:
	// バッファの専有を解除
    pMesh->UnlockVertexBuffer();
    pMesh->UnlockIndexBuffer();
	
	return ret;
}
// ---------------------------------------------------------------------------
// 破棄
// ---------------------------------------------------------------------------
VOID CShadowVolume::Destroy()
{
    SAFE_DELETE_ARRAY( m_pVertices );
	m_dwNumFaces = 0;
}
// ---------------------------------------------------------------------------
