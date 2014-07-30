// -------------------------------------------------------------
// 반영반사광
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------

float4x4 mWVP;

float4 vLightDir;	// 광원방향
float4 vColor;		// 광원*메시 색
float3 vEyePos;		// 카메라위치(로컬좌표계)

// -------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
    float3 N			: TEXCOORD0;
    float3 X			: TEXCOORD1;
};
// -------------------------------------------------------------
// 장면렌더
// -------------------------------------------------------------
VS_OUTPUT VS(
      float4 Pos    : POSITION,          // 로컬위치좌표
      float4 Normal : NORMAL            // 법선벡터
){
	VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
	
	// 좌표변환
	Out.Pos = mul(Pos, mWVP);
	
	// 확산광+환경광
	float amb = -vLightDir.w;	// 환경광의 강도
	float3 L = -vLightDir; // 로컬좌표계에서의 광원벡터
	Out.Color = vColor * max(amb, dot(Normal, -vLightDir));
	
	// 반영반사용 벡터
	Out.N = Normal.xyz;
	Out.X = Pos.xyz;
	
	return Out;
}
// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
	float3 L = -vLightDir.xyz;				// 광원벡터
	float3 N = normalize(In.N);				// 법선벡터
	float3 V = normalize(vEyePos - In.X);	// 시선벡터
	float3 H = normalize(L + V);			// 하프벡터
	
	// 계산에 필요한 여러가지 각도
	float NV = dot(N,V);
	float NH = dot(N,H);
	float VH = dot(V,H);
	float NL = dot(N,L);
	float LH = dot(L,H);
	
	// Beckmann분포함수
	const float m = 0.35f;// 거친정도
	float NH2 = NH*NH;
	float D = exp(-(1-NH2)/(NH2*m*m))/(4*m*m*NH2*NH2);
	
	// 기하감쇠율
	float G = min(1,min(2*NH*NV/VH, 2*NH*NL/VH));
	
	// 프레넬
	float n = 20.0f;// 복소굴절률의 실수부
	float g = sqrt(n*n+LH*LH-1);
	float gpc = g+LH;
	float gnc = g-LH;
	float cgpc = LH*gpc-1;
	float cgnc = LH*gnc+1;
	float F = 0.5f*gnc*gnc*(1+cgpc*cgpc/(cgnc*cgnc))/(gpc*gpc);
	
	// 금속의 색
	float4 ks = {2.0f*0.486f, 2.0f*0.433f, 2.0f*0.185f, 1.0f};
	
    return In.Color						// 확산광+환경광
			 + ks * max(0,F*D*G/NV);	// 반영반사광
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
