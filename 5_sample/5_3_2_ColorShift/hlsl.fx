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
    float3 X			: TEXCOORD2;
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
	const float m = 0.2f;// 거친정도
	float NH2 = NH*NH;
	float D = exp(-(1-NH2)/(NH2*m*m))/(4*m*m*NH2*NH2);
	
	// 기하감쇠율
	float G = min(1,min(2*NH*NV/VH, 2*NH*NL/VH));
	
	// 프레넬
	float n = 3.0f;// 복소굴절률의 실수부
	float g = sqrt(n*n+LH*LH-1);
	float gpc = g+LH;
	float gnc = g-LH;
	float cgpc = LH*gpc-1;
	float cgnc = LH*gnc+1;
	float F = 0.5f*gnc*gnc*(1+cgpc*cgpc/(cgnc*cgnc))/(gpc*gpc);
	float F0 = ((n-1)*(n-1))/((n+1)*(n+1));
	
	// 금속의 색
	float4 light_color = {0.296, 0.304, 1.000f, 1.0f};// 입사광 색
	float4 c0 = {0.486f * light_color.x        
			   , 0.433f * light_color.y           // 정면에서light_color색의 
			   , 0.185f * light_color.z, 1.0f};   // 빛이 닿았을때의 반영반사색
	float4 color = c0 + (light_color-c0)*max(0,F-F0)/(1-F0);// 컬리쉬프트

    return In.Color						// 확산광+환경광
			 + color * max(0,F*D*G/NV);	// 반영반사광
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
