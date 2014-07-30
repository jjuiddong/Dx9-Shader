// ------------------------------------------------------------
// 16박스필터
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// ------------------------------------------------------------
// 전역변수
// ------------------------------------------------------------
float WIDTH;
float HEIGHT;

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
// 정점셰이더에서 픽셀셰이더로 넘기는 데이터
// ------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
	float2 Tex0			: TEXCOORD0;
	float2 Tex1			: TEXCOORD1;
	float2 Tex2			: TEXCOORD2;
	float2 Tex3			: TEXCOORD3;
};

// ------------------------------------------------------------
// 정점셰이더
// ------------------------------------------------------------
VS_OUTPUT VS (
      float4 Pos    : POSITION           // 모델정점
     ,float4 Tex    : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 위치좌표
    Out.Pos = Pos;
    
    Out.Tex0 = Tex + float2(-1.0f/WIDTH, -1.0f/HEIGHT);
    Out.Tex1 = Tex + float2(+1.0f/WIDTH, -1.0f/HEIGHT);
    Out.Tex2 = Tex + float2(-1.0f/WIDTH, +1.0f/HEIGHT);
    Out.Tex3 = Tex + float2(+1.0f/WIDTH, +1.0f/HEIGHT);
    
    return Out;
}

// ------------------------------------------------------------
// 픽셀셰이더
// ------------------------------------------------------------
PixelShader PS = asm
{
    ps_1_1

    def c0, 0.5, 0.5, 0.5, 0.5

    tex t0
    tex t1
    tex t2
    tex t3

    lrp r0, c0, t0, t1 ; r0 = (t0+t1)/2
    lrp r1, c0, t2, t3 ; r1 = (t2+t3)/2
    lrp r0, c0, r0, r1 ; out = (t0+t1+t2+t3)/4
};

// ------------------------------------------------------------
// 9콘 필터 샘플링
// ------------------------------------------------------------
VS_OUTPUT VS9 (
      float4 Pos    : POSITION           // 모델정점
     ,float4 Tex   : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 위치좌표
    Out.Pos = Pos;
    
    Out.Tex0 = Tex + float2( 0.0f/WIDTH,  0.0f/HEIGHT);
    Out.Tex1 = Tex + float2(+1.0f/WIDTH,  0.0f/HEIGHT);
    Out.Tex2 = Tex + float2( 0.0f/WIDTH, +1.0f/HEIGHT);
    Out.Tex3 = Tex + float2(+1.0f/WIDTH, +1.0f/HEIGHT);
    
    return Out;
}

// ------------------------------------------------------------
// 테크닉
// ------------------------------------------------------------
technique TShader
{
    pass P0// 16박스필터 샘플링
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS();
        PixelShader  = <PS>;
        
        // 샘플러
		Sampler[0] = (SrcSamp);
		Sampler[1] = (SrcSamp);
		Sampler[2] = (SrcSamp);
		Sampler[3] = (SrcSamp);
    }
    pass P1// 9콘필터 샘플링
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS9();
        PixelShader  = <PS>;
        
        // 샘플러
		Sampler[0] = (SrcSamp);
		Sampler[1] = (SrcSamp);
		Sampler[2] = (SrcSamp);
		Sampler[3] = (SrcSamp);
    }
}
