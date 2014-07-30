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
float time;

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
    AddressV = Wrap;
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
	Out.Col = max(0,dot(Normal, LightDir.xyz)) + LightDir.w;

	//위치를 적당히 변환한다
	Out.Tex = 0.5f*Pos+0.5;

	return Out;
}
// -------------------------------------------------------------
// 한 옥타브의 노이즈 함수
// -------------------------------------------------------------
float noise( float gap, float3 pos )
{
	float3 x = gap * pos;
	float3 ix = floor(x)/32.0f;
	float3 fx = frac(x);
	
	float4 x0, x1;

	x0.x = tex3D( Samp, ix + float3(0.f/32.f, 0.f/32.f, 0.f/32.f) ).x;
	x0.y = tex3D( Samp, ix + float3(1.f/32.f, 0.f/32.f, 0.f/32.f) ).x;
	x0.z = tex3D( Samp, ix + float3(0.f/32.f, 1.f/32.f, 0.f/32.f) ).x;
	x0.w = tex3D( Samp, ix + float3(1.f/32.f, 1.f/32.f, 0.f/32.f) ).x;
	
	x1.x = tex3D( Samp, ix + float3(0.f/32.f, 0.f/32.f, 1.f/32.f) ).x;
	x1.y = tex3D( Samp, ix + float3(1.f/32.f, 0.f/32.f, 1.f/32.f) ).x;
	x1.z = tex3D( Samp, ix + float3(0.f/32.f, 1.f/32.f, 1.f/32.f) ).x;
	x1.w = tex3D( Samp, ix + float3(1.f/32.f, 1.f/32.f, 1.f/32.f) ).x;
	
	x0    = lerp(x0.xyzw, x1.xyzw, fx.z);
	x0.xy = lerp(x0.xy,   x0.zw,   fx.y);
	return  lerp(x0.x,    x0.y,    fx.x);
}
// -------------------------------------------------------------
// 픽셀셰이더
// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
	float4 color = {167.f/256.f, 105.f/256.f, 61.f/256.f, 0};

	float n = 0.5000f * noise( 12.f, In.Tex )
			+ 0.2500f * noise( 24.f, In.Tex );

	return In.Col * tex2D( WoodSamp, n+time );
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
