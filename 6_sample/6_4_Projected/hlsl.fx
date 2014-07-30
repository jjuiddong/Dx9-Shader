// -------------------------------------------------------------
// 투영텍스처 그림자
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------
float4x4 mWVP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWVPT;		// 로컬에서 텍스처공간으로의 좌표변환
float4	 vLightPos;	// 광원의 위치

// -------------------------------------------------------------
// 텍스처
// -------------------------------------------------------------
texture DecaleMap;
sampler DecaleMapSamp = sampler_state
{
    Texture = <DecaleMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

texture ShadowMap;
sampler ShadowMapSamp = sampler_state
{
    Texture = <ShadowMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};


// -------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// -------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 TexDecale: TEXCOORD0;
	float4 TexShadow: TEXCOORD1;
};
// -------------------------------------------------------------
// 정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS (
	  float4 Pos	: POSITION          // 정점위치
	, float4 Normal	: NORMAL            // 법선벡터
	, float2 Tex	: TEXCOORD0			// 텍스처좌표
){
	VS_OUTPUT Out;        // 출력데이터
	
	// 위치변환
	Out.Pos = mul( Pos, mWVP );
	
	Out.Color = max( dot(normalize(vLightPos.xyz-Pos.xyz), Normal), 0);
	
	// 텍스처좌표
	Out.TexDecale = Tex;
	
	// 텍스처좌표
	Out.TexShadow = mul( Pos, mWVPT );
	
	return Out;
}
// -------------------------------------------------------------
// 픽셀셰이더
// -------------------------------------------------------------
float4 PS ( VS_OUTPUT In) : COLOR
{
	float4 decale = tex2D( DecaleMapSamp, In.TexDecale );
	float4 shadow = tex2Dproj( ShadowMapSamp, In.TexShadow );
	
	return decale * (saturate(In.Color-0.5f*shadow)+0.3f);
}
// -------------------------------------------------------------
// 테크닉
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}
