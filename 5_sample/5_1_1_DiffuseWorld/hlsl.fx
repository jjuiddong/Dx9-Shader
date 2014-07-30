// -------------------------------------------------------------
// 확산광
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------

float4x4 mWVP;
float4x4 mWIT;

float3 vLightDir;							// 광원의 방향

// 광원밝기
float4 I_a = { 0.3f, 0.3f, 0.3f, 0.0f };    // ambient
float4 I_d = { 0.7f, 0.7f, 0.7f, 0.0f };    // diffuse

// 반사율
float4 k_a = { 1.0f, 1.0f, 1.0f, 1.0f };    // ambient
float4 k_d = { 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse

// -------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
};
// -------------------------------------------------------------
// 장면정보
// -------------------------------------------------------------
VS_OUTPUT VS(
      float4 Pos    : POSITION,          // 로컬위치좌표
      float3 Normal : NORMAL            // 법선벡터
){
	VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
	
	// 좌표변환
	Out.Pos = mul(Pos, mWVP);
	
	// 정점색
	float3 L = -vLightDir;
	float3 N = normalize(mul(Normal, (float3x3)mWIT)); // 월드좌표계에서의 법선

	Out.Color = I_a * k_a					   // 환경광
	          + I_d * k_d * max(0, dot(N, L)); // 확산광
	
	return Out;
}
// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
    return In.Color;
}

// -------------------------------------------------------------
// 테크닉
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_1_1 PS();
    }
}
