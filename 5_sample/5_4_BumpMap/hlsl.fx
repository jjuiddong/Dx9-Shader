// -------------------------------------------------------------
// 범프맵
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------

float4x4 mWVP;		// 좌표변환 행렬

float4 vLightDir;	// 광원방향
float4 vColor;		// 광원*메시 색
float3 vEyePos;		// 카메라위치(로컬좌표계)

// -------------------------------------------------------------
// 텍스처
// -------------------------------------------------------------
// 디컬 텍스처
texture DecaleTex;
sampler DecaleSamp = sampler_state
{
    Texture = <DecaleTex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
// 법선맵
texture NormalMap;
sampler NormalSamp = sampler_state
{
    Texture = <NormalMap>;
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
    float4 Color		: COLOR0;		// 정점색
    float2 Tex			: TEXCOORD0;	// 디컬텍스처 좌표
    float3 L			: TEXCOORD1;	// 광원벡터
    float3 E			: TEXCOORD2;	// 법선벡터
};
// -------------------------------------------------------------
// 장면렌더
// -------------------------------------------------------------
VS_OUTPUT VS(
      float4 Pos      : POSITION,          // 로컬위치좌표
      float3 Normal   : NORMAL,            // 법선벡터
      float3 Tangent  : TANGENT0,          // 접선벡터
      float2 Texcoord : TEXCOORD0          // 텍스처좌표
){
	VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
	
	// 좌표변환
	Out.Pos = mul(Pos, mWVP);
	
	// 메시 색
	Out.Color = vColor;
	
	// 디컬용 텍스처좌표
	Out.Tex = Texcoord;

	// 좌표계변환 기저
	float3 N = Normal;
	float3 T = Tangent;
	float3 B = cross(N,T);

	// 반영반사용 벡터
	float3 E = vEyePos - Pos.xyz;	// 시선벡터
	Out.E.x = dot(E,T);
	Out.E.y = dot(E,B);
	Out.E.z = dot(E,N);

	float3 L = -vLightDir.xyz;		// 광원벡터
	Out.L.x = dot(L,T);
	Out.L.y = dot(L,B);
	Out.L.z = dot(L,N);
	
	return Out;
}
// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
	float3 N = 2.0f*tex2D( NormalSamp, In.Tex ).xyz-1.0;// 법선맵으로부터 법선
	float3 L = normalize(In.L);						// 광원벡터
	float3 R = reflect(-normalize(In.E), N);		// 반사벡터
	float amb = -vLightDir.w;						// 환경광의 강도
	
    return In.Color * tex2D( DecaleSamp, In.Tex )	// 확산광과 환경광에
			   * (max(0, dot(N, L))+amb)			// 정점색과 텍스처색을 합성한다
			 + 0.3f * pow(max(0,dot(R, L)), 8);		// Phong반영반사광
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
