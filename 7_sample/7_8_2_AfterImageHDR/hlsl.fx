// ------------------------------------------------------------
// 잔상
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------
float4x4 mWVP;
float3 vEyePos;
float3 vLightDir;
float4 vCol;

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
texture CurrentMap;
sampler CurrentSamp = sampler_state
{
    Texture = <CurrentMap>;
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
    float4 Color		: COLOR0;
	float3 Normal		: TEXCOORD0;
	float3 Eye			: TEXCOORD1;
};
// -------------------------------------------------------------
struct OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
	float3 Normal		: TEXCOORD0;
	float3 Eye			: TEXCOORD1;
};
// -------------------------------------------------------------
OUTPUT VS(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL             // 모델법선
){
	OUTPUT Out = (OUTPUT)0;        // 출력데이터
	float4  uv;
	
	// 좌표변환
	Out.Pos = mul(Pos, mWVP);
	
	// 색
	Out.Color = vCol * (0.7*max( dot(vLightDir, Normal), 0)+0.3);
	
	Out.Normal = Normal;		// 법선
	Out.Eye    = Pos - vEyePos;	// 시선
	
	return Out;
}
// -------------------------------------------------------------
float4 PS(OUTPUT In) : COLOR
{   
    float3 e = normalize(In.Eye);	// 시선벡터
    float3 n = normalize(In.Normal);// 법선벡터
    float3 r = reflect(e,n);		// 반사벡터
    float3 RGB2Lum = {0.299, 0.587, 0.114};
	
	float power = pow(max(0,dot(r,vLightDir)), 32); // 퐁
	float4 SpecCol = float4(10,13,15,0);			// 반영반사색
	
	float4 Out = In.Color + SpecCol * power;
	
	// 밝기 강도를 쓴다
	Out.a = 0.01*(exp(2.0f*dot(RGB2Lum,Out.rgb))-1.0);

    return Out;
}
// -------------------------------------------------------------

// ------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// ------------------------------------------------------------
struct VS_OUTPUT_AFTERIMAGE
{
    float4 Pos			: POSITION;
	float2 Tex0			: TEXCOORD0;
	float2 Tex1			: TEXCOORD1;
};

// ------------------------------------------------------------
// 픽셀셰이더
// ------------------------------------------------------------
float4 PS_AFTERIMAGE (VS_OUTPUT_AFTERIMAGE In) : COLOR
{   
    float4 Last = tex2D( SrcSamp,     In.Tex0 );
    float4 Now  = tex2D( CurrentSamp, In.Tex1 );
	
	float weight = Last.a-0.001f;
	
    return (1.0f-weight) * Now + weight * Last;
}
// ------------------------------------------------------------
// 테크닉
// ------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS();
    }
    pass P1
    {
        // 셰이더
        PixelShader  = compile ps_2_0 PS_AFTERIMAGE();
    }
}
