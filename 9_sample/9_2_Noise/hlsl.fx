// -------------------------------------------------------------
// Perlin Noise
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------
float4x4 mWVP;		// 좌표변환 행렬
float4 LightDir;		// 광원벡터

// -------------------------------------------------------------
// 텍스처
// -------------------------------------------------------------
// 난수 텍스처
texture Tex;
sampler Samp = sampler_state
{
    Texture = <Tex>;
    MinFilter = POINT;
    MagFilter = POINT;
    MipFilter = NONE;

    AddressU = Wrap;
    AddressV = Wrap;
};
texture WoodTex;
sampler WoodSamp = sampler_state
{
    Texture = <WoodTex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Wrap;
    AddressV = Clamp;
};

// -------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Col			: COLOR0;
    float3 Tex			: TEXCOORD0;	// 디컬 텍스처좌표
};
// -------------------------------------------------------------
// 좌표변환
// -------------------------------------------------------------
float2 Q(float3 v)
{
	float2 Out = (float2)0;
	
	float pv = 7.0f/8.0f;
	float ar = 1.0/(2.0*(1.000000001+v.y));

	Out.x = pv * ar * v.x + 0.5;
	Out.y = pv * ar * v.z + 0.5;

	return Out;
}

// -------------------------------------------------------------
// 장면렌더
// -------------------------------------------------------------
VS_OUTPUT VS(
      float4 Pos      : POSITION,         // 로컬위치좌표
      float3 Normal   : NORMAL            // 법선벡터
){
	VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
	
	// 좌표변환
	Out.Pos = mul(Pos, mWVP);
	
	// 조명
	Out.Col = max(0,dot(Normal, LightDir.xyz)) - LightDir.w;

	//위치를 적당히 변환한다
	Out.Tex.xy = Q(normalize(Pos));

	return Out;
}
// -------------------------------------------------------------
// 한 옥타브의 노이즈 함수
// -------------------------------------------------------------
float noise( float gap, float3 pos )
{
	float2 x = gap * pos;
	float2 ix = floor(x)/128.0f;
	float2 fx = frac(x);

	float x00 = tex2D( Samp, ix + float2(0.f/128.f, 0.f/128.f) ).x;
	float x10 = tex2D( Samp, ix + float2(1.f/128.f, 0.f/128.f) ).x;
	float x01 = tex2D( Samp, ix + float2(0.f/128.f, 1.f/128.f) ).x;
	float x11 = tex2D( Samp, ix + float2(1.f/128.f, 1.f/128.f) ).x;
	
	return lerp(lerp(x00, x01, fx.y), lerp(x10, x11, fx.y), fx.x);
}
// -------------------------------------------------------------
// 픽셀셰이더
// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
	float4 color = {167.f/256.f, 105.f/256.f, 61.f/256.f, 0};

	float n = 0.5000f * noise(  8.f, In.Tex )
			+ 0.2500f * noise( 16.f, In.Tex )
			+ 0.1250f * noise( 32.f, In.Tex )
			+ 0.0625f * noise( 64.f, In.Tex );

	return In.Col * tex2D( WoodSamp, float2(0.5, n) );
//	return color * (0.8f*fmod(10*n,1)+0.2);
}




// -------------------------------------------------------------
// 테크닉
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}
