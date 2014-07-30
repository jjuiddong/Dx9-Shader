// -------------------------------------------------------------
// 그림자 맵
// 
// Copyright (c) 2002-2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------
float4x4 mWVP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWLP;		// 로컬에서 투영공간으로의 좌표변환
float4x4 mWLPB;		// 텍스처좌표계로의 투영
float4   vCol;		// 메시색
float4	 vLightDir;	// 광원방향

// -------------------------------------------------------------
// 텍스처
// -------------------------------------------------------------
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
// -------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
	float4 Diffuse		: COLOR0;
	float4 Ambient		: COLOR1;
	float4 ShadowMapUV	: TEXCOORD0;
	float4 Depth		: TEXCOORD1;
	float2 DecaleTex	: TEXCOORD2;
};

// -------------------------------------------------------------
// 1패스:정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL	         // 모델법선
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 좌표변환
	float4 pos = mul( Pos, mWLP );
	
    // 위치좌표
    Out.Pos = pos;
    
    // 카메라좌표계에서의 깊이를 텍스처에 넣는다
    Out.ShadowMapUV = pos;

    return Out;
}
// -------------------------------------------------------------
// 1패스:픽셀셰이더
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{   
    float4 Out;
    
    Out = In.ShadowMapUV.z / In.ShadowMapUV.w;
    
    return Out;
}
// -------------------------------------------------------------
// 정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL,	         // 모델법선
      float2 Tex    : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
	float4	uv;
	
	// 좌표변환
    Out.Pos = mul(Pos, mWVP);
	// 색
	Out.Diffuse = vCol * max( dot(vLightDir, Normal), 0);// 확산광
	Out.Ambient = vCol * 0.3f;							 // 환경광
	
	// 그림자 맵
	Out.ShadowMapUV = mul(Pos, mWLPB);
	
	// 비교를 위한 깊이값
	Out.Depth       = mul(Pos, mWLP);
		
	// 디컬 텍스처
	Out.DecaleTex   = Tex;
		
    return Out;
}
// -------------------------------------------------------------
// 2패스:픽셀셰이더(텍스처있음)
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT In) : COLOR
{   
    float4 Color;
	float  shadow = tex2Dproj( ShadowMapSamp, In.ShadowMapUV ).x;
	float4 decale = tex2D( DecaleMapSamp, In.DecaleTex );
    
    Color = In.Ambient
		 + ((shadow * In.Depth.w < In.Depth.z-0.03f) ? 0 : In.Diffuse);

    return Color * decale;
}  
// -------------------------------------------------------------
// 2패스:픽셀셰이더(텍스처없음)
// -------------------------------------------------------------
float4 PS_pass2(VS_OUTPUT In) : COLOR
{   
    float4 Color;
	float  shadow = tex2Dproj( ShadowMapSamp, In.ShadowMapUV ).x;
    
    Color = In.Ambient
		 + ((shadow * In.Depth.w < In.Depth.z-0.03f) ? 0 : In.Diffuse);

    return Color;
}  
// -------------------------------------------------------------
// 테크닉
// -------------------------------------------------------------
technique TShader
{
    pass P0// 그림자맵 생성
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_2_0 PS_pass0();
    }
    pass P1// 텍스처 있음
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS_pass1();
    }
    pass P2// 텍스처 없음
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS_pass2();
    }
}
