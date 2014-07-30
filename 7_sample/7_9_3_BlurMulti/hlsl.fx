// ------------------------------------------------------------
// 모션블러
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------
float4x4 mWV;
float4x4 mLastWV;
float4x4 mVP;
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
// -------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// -------------------------------------------------------------
struct OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
	float3 Normal		: TEXCOORD0;
	float3 Eye			: TEXCOORD1;
};
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
	float4 Tex			: TEXCOORD0;
	float4 Velocity		: TEXCOORD1;
};
// -------------------------------------------------------------
OUTPUT VS(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL             // 모델법선
){
	OUTPUT Out = (OUTPUT)0;        // 출력데이터
	
	// 좌표변환
	Out.Pos = mul(mul(Pos, mWV), mVP);		// 중간좌표사용
	
	// 색
	Out.Color = vCol * (0.7*max( dot(vLightDir, Normal), 0)// 확산색
						+0.3);// 환경색
	
	Out.Normal = Normal;		// 색
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
	
	// 오브젝트가 존재하는 곳의 알파를 1로
	Out.a = 1;
	
    return Out;
}
// -------------------------------------------------------------
VS_OUTPUT VS_Blur(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL             // 모델법선
){
	VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
	
	// 좌표변환
	float4 x1 = mul(Pos, mWV);			// 지금 뷰좌표
	float4 x0 = mul(Pos, mLastWV);		// 1프레임 직전 뷰좌표
	float4 v = x1-x0;					// 속도
	float3 n = mul(Normal, mWV);		// 뷰좌표계에서의 법선
	
	bool bFront = (0<=dot(n, v.xyz));	// 속도방향으로 향하고 있는가?
	float4 x = bFront ? x1 : x0;		// 방향에따라 위치결정
	
	Out.Pos = mul(x, mVP);				// 투영공간으로
	
	// 투영공간에서 텍스처공간으로 변환
	Out.Tex.x =  Out.Pos.x + Out.Pos.w;
	Out.Tex.y = -Out.Pos.y + Out.Pos.w;
	Out.Tex.w = 2.0f*Out.Pos.w;
	
	// 텍스처좌표로 속도를 구한다
	float4 s0 = mul(x0, mVP); s0 /= s0.w;
	float4 s1 = mul(x1, mVP); s1 /= s1.w;
	Out.Velocity = s1 - s0;
	Out.Velocity.x *= +0.5f;
	Out.Velocity.y *= -0.5f;
	
	return Out;
}
// ------------------------------------------------------------
// 픽셀셰이더
// ------------------------------------------------------------
float4 PS_Blur (VS_OUTPUT In) : COLOR
{   
    float4 Out = 0;
	const int   SAMPLES = 26;
	const float samples = SAMPLES;
	
	for(int i=0;i<SAMPLES;i++){
		float t = (float)(i+1)/samples;
		Out += tex2D( SrcSamp, In.Tex/In.Tex.w + t*In.Velocity );
	}
	Out /= samples;
	
    return Out;
}

// ------------------------------------------------------------
// 테크닉
// ------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // 일반렌더
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS();
    }
    pass P1
    {
        // 모션블러
        VertexShader = compile vs_1_1 VS_Blur();
        PixelShader  = compile ps_2_0 PS_Blur();
    }
}
