// ------------------------------------------------------------
// 윤곽추출
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// ------------------------------------------------------------
// 전역변수
// ------------------------------------------------------------
float4x4 mWVP;
float4   vCol;
float4	 vLightDir;	// 광원방향

// ------------------------------------------------------------
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// ------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
	float2 Tex0			: TEXCOORD0;
};

// ------------------------------------------------------------
// 정점셰이더(조명있음)
// ------------------------------------------------------------
VS_OUTPUT VS (
      float4 Pos    : POSITION           // 모델정점
    , float4 Normal : NORMAL             // 법선벡터
     ,float4 Tex0   : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
	// 위치좌표
	Out.Pos = mul( Pos, mWVP );
	
	// 색
	float diffuse = max(dot(vLightDir.xyz, Normal.xyz), 0);//환경광
	float ambient = vLightDir.w;						   //확산광
	Out.Color = vCol * ( diffuse + ambient );
	
	// 텍스처좌표
    Out.Tex0 = Tex0;

    return Out;
}

// ------------------------------------------------------------
// 정점셰이더(윤곽추출)
// ------------------------------------------------------------
VS_OUTPUT VS_Edge (
      float4 Pos    : POSITION           // 모델정점
    , float4 Normal : NORMAL             // 법선벡터
     ,float4 Tex0   : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    float4 pos = mul( Pos, mWVP );

    float4 n = mul( Normal, mWVP );
    n.zw=0;
    
	// 위치좌표
	Out.Pos = pos + 0.02f*n;
	
	// 색
	Out.Color = 0;
	
    return Out;
}

// ------------------------------------------------------------
// 테크닉
// ------------------------------------------------------------
technique TShader
{
    pass P0 // 조명계산있음
    {
        VertexShader = compile vs_1_1 VS();
    }
    pass P1 // 윤곽추출
    {
        VertexShader = compile vs_1_1 VS_Edge();
    }
}
