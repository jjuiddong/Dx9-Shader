// ------------------------------------------------------------
// 중앙값필터
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

static const float TEX_SIZE = 256.0;

// ------------------------------------------------------------
// 텍스처
// ------------------------------------------------------------
texture SrcMap;
sampler SrcSamp = sampler_state
{
    Texture = <SrcMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// ------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// ------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
	float2 Tex			: TEXCOORD0;
};

// ------------------------------------------------------------
// 정점셰이더
// ------------------------------------------------------------
VS_OUTPUT VS (
      float4 Pos    : POSITION,          // 모델정점
      float4 Tex    : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 위치좌표
    Out.Pos = Pos;
    
    Out.Tex = Tex + float2( 0.5/TEX_SIZE, 0.5/TEX_SIZE);
    
    return Out;
}

// ------------------------------------------------------------
// 픽셀셰이더
// ------------------------------------------------------------
float4 PS (VS_OUTPUT In) : COLOR
{   
    float4 Out = (float4)0;
	const float3 RGB2Y = {0.299, 0.587, 0.114};
	
	float2 s1 = float2( 0.0/TEX_SIZE, 1.0/TEX_SIZE);
	float2 s2 = float2( 1.0/TEX_SIZE, 0.0/TEX_SIZE);
	
	// 텍스처를 약간 늘려서 샘플링
	float3 col0 = tex2D( SrcSamp, In.Tex );
	float3 col1 = tex2D( SrcSamp, In.Tex + s1 );
	float3 col2 = tex2D( SrcSamp, In.Tex - s1 );
	float3 col3 = tex2D( SrcSamp, In.Tex + s2 );
	float3 col4 = tex2D( SrcSamp, In.Tex - s2 );
	
	// 색강도를 구한다
	float b0 = dot( col0, RGB2Y );
	float b1 = dot( col1, RGB2Y );
	float b2 = dot( col2, RGB2Y );
	float b3 = dot( col3, RGB2Y );
	float b4 = dot( col4, RGB2Y );

	// 색순서를 구한다 ([4-값]의 순서로 나열)
	float flag0 = ((b0< b1)?1.0:0.0) + ((b0< b2)?1.0:0.0) + ((b0< b3)?1.0:0.0) + ((b0< b4)?1.0:0.0);
	float flag1 = ((b1<=b0)?1.0:0.0) + ((b1< b2)?1.0:0.0) + ((b1< b3)?1.0:0.0) + ((b1< b4)?1.0:0.0);
	float flag2 = ((b2<=b0)?1.0:0.0) + ((b2<=b1)?1.0:0.0) + ((b2< b3)?1.0:0.0) + ((b2< b4)?1.0:0.0);
	float flag3 = ((b3<=b0)?1.0:0.0) + ((b3<=b1)?1.0:0.0) + ((b3<=b2)?1.0:0.0) + ((b3< b4)?1.0:0.0);

	// 중심색을 중앙값으로 사용
	Out.xyz =  ( flag0 ==2.0 ) ? col0 : 
	          (( flag1 ==2.0 ) ? col1 :
	          (( flag2 ==2.0 ) ? col2 :
	          (( flag3 ==2.0 ) ? col3 : col4 )));

    return Out;
}
// ------------------------------------------------------------
// 테크닉
// ------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // 셰이더
        VertexShader = compile vs_1_1  VS();
        PixelShader  = compile ps_2_0 PS();
        
		Sampler[0] = (SrcSamp);
    }
}
