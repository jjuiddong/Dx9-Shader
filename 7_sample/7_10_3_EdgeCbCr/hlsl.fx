// ------------------------------------------------------------
// 윤곽추출
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// ------------------------------------------------------------
// 전역변수
// ------------------------------------------------------------
float4x4 mWVP;
float4   vCol;
float4	 vLightDir;	// 광원방향

// ------------------------------------------------------------
// 텍스처
// ------------------------------------------------------------
texture SrcTex;
sampler SrcSamp = sampler_state
{
    Texture = <SrcTex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// ------------------------------------------------------------
sampler FloorSamp = sampler_state
{
    Texture = <SrcTex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Wrap;
    AddressV = Wrap;
};
// ------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// ------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
	float2 Tex0			: TEXCOORD0;
};
// ------------------------------------------------------------
struct EDGE_INPUT
{
    float4 Pos			: POSITION;
	float2 Tex0			: TEXCOORD0;
	float2 Tex1			: TEXCOORD1;
	float2 Tex2			: TEXCOORD2;
	float2 Tex3			: TEXCOORD3;
};

// ------------------------------------------------------------
// 정점셰이더(조명없음)
// ------------------------------------------------------------
VS_OUTPUT VS_pass0 (
      float4 Pos    : POSITION           // 모델정점
     ,float4 Normal : NORMAL             // 법선벡터
     ,float4 Tex0   : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
	// 위치좌표
	Out.Pos = mul( Pos, mWVP );
	
	// 텍스처좌표
    Out.Tex0 = Tex0;

    return Out;
}
// ------------------------------------------------------------
// 픽셀셰이더(조명없음)
// ------------------------------------------------------------
float4 PS_pass0 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	
	// 색
	Out = tex2D( FloorSamp, In.Tex0 );
	
	return Out;
}
// ------------------------------------------------------------
// 정점셰이더(조명있음)
// ------------------------------------------------------------
VS_OUTPUT VS_pass1 (
      float4 Pos    : POSITION           // 모델정점
    , float4 Normal : NORMAL             // 법선벡터
     ,float4 Tex0   : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
	// 위치좌표
	Out.Pos = mul( Pos, mWVP );
	
	// 색
	float diffuse = max(dot(vLightDir.xyz, Normal.xyz), 0);//확산광
	float ambient = vLightDir.w;						   //환경광
	Out.Color = vCol * ( diffuse + ambient );
	
	// 텍스처좌표
    Out.Tex0 = Tex0;

    return Out;
}

// ------------------------------------------------------------
// 픽셀셰이더(조명있음)
// ------------------------------------------------------------
float4 PS_pass1 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	
	// 색
	Out = In.Color * tex2D( FloorSamp, In.Tex0 );
	
	return Out;
}
// ------------------------------------------------------------
// 픽셀셰이더(윤곽추출)
// ------------------------------------------------------------
float4 PS_Edge (EDGE_INPUT In) : COLOR
{
	float3 d0 = tex2D( SrcSamp, In.Tex0 ).rgb
	           -tex2D( SrcSamp, In.Tex1 ).rgb;
	float3 d1 = tex2D( SrcSamp, In.Tex2 ).rgb
	           -tex2D( SrcSamp, In.Tex3 ).rgb;
	
	// 렌더링타겟2:CbCr
	float3x3 RGB2CrCb = {
		{ 0,         0,         0      },// Y(=0)
		{ 0.50000, - 0.41869, - 0.08131},// Cr
		{-0.16874, - 0.33126, + 0.50000},// Cb
	};
	d0 = mul( RGB2CrCb, d0 );
	d1 = mul( RGB2CrCb, d1 );
	
	float diff = dot(d0, d0)+dot(d1,d1);
	
	return 1.0 - 100.0 * diff;
}

// ------------------------------------------------------------
// 테크닉
// ------------------------------------------------------------
technique TShader
{
    pass P0 // 조명계산없음
    {
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_1_1 PS_pass0();
    }
    pass P1 // 조명계산있음
    {
        VertexShader = compile vs_1_1 VS_pass1();
        PixelShader  = compile ps_1_1 PS_pass1();
    }
    pass P2 // 엣지
    {
        // 셰이더
        PixelShader  = compile ps_2_0 PS_Edge();
	}
}
