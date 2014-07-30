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

// -------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
};
// -------------------------------------------------------------
struct OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
};
// -------------------------------------------------------------
OUTPUT VS(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL             // 모델법선
){
	OUTPUT Out = (OUTPUT)0;        // 출력데이터
	
	// 좌표변환
	Out.Pos = mul(mul(Pos, mWV), mVP);
	
	// 색
	Out.Color = vCol * (0.7*max( dot(vLightDir, Normal), 0)// 둮랼륡
						+0.3);// 듏떕륡
	
	return Out;
}
// -------------------------------------------------------------
OUTPUT VS_Blur(
      float4 Pos    : POSITION,          // 모델정점
      float3 Normal : NORMAL             // 모델법선
){
	OUTPUT Out = (OUTPUT)0;        // 출력데이터
	
	// 좌표변환
	float4 x1 = mul(Pos, mWV);			// 지금 뷰좌표
	float4 x0 = mul(Pos, mLastWV);		// 1프레임 직전 뷰좌표
	float4 v = x1-x0;					// 속도
	float3 n = mul(Normal, mWV);		// 뷰좌표계에서의 법선
	
	bool bFront = (0<=dot(n, v.xyz));	// 속도방향으로 향하고 있는가?
	float4 x = bFront ? x1 : x0;		// 방향에따라 위치결정
	
	Out.Pos = mul(x, mVP);				// 투영공간으로
	
	// 색
	Out.Color = vCol * (0.7*max( dot(vLightDir, Normal), 0)// 확산광
						+0.3);// 환경광
	
	// 진행방향으로 향하고 있으면 불투명, 반대이면 투명
	Out.Color.a = bFront ? 1 : 0;
	
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
    }
    pass P1
    {
        // 모션블러
        VertexShader = compile vs_1_1 VS_Blur();
    }
}
