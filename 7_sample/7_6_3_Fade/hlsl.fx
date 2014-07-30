// ------------------------------------------------------------
// 세피아필터
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// ------------------------------------------------------------
// 전역변수
// ------------------------------------------------------------
float t;

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
    float4 Pos            : POSITION;
    float2 Tex            : TEXCOORD0;
};

// ------------------------------------------------------------
// 정점셰이더
// ------------------------------------------------------------
VS_OUTPUT VS (
      float4 Pos    : POSITION,          // 모델정점
      float4 Tex    : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 위치좌표
    Out.Pos = Pos;
    // 텍스처좌표
    Out.Tex = Tex;
    
    return Out;
}

// ------------------------------------------------------------
// 픽셀셰이더
// ------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
    float4 Out = (float4)0;
    float3 YCbCr;
    
    const float3 RGB2Y  = {0.29900, 0.58700, 0.11400};
    const float3 Cb2RGB = { 0.00000f, -0.34414f, 1.77200f};
    const float3 Cr2RGB = {+1.40200f, -0.71414f, 0.00000f};

    float3 Color = tex2D( SrcSamp, In.Tex ).xyz; // 원본색
    
    YCbCr.x = dot(Color, RGB2Y);    // Y
    YCbCr.y = -0.2f;				// Cb
    YCbCr.z =  0.1f;				// Cr
    
    float3 Sepia = YCbCr.x
				 + mul(Cb2RGB, YCbCr.y)
				 + mul(Cr2RGB, YCbCr.z);// YCbCr을 RGB로
    
    Out.rgb = lerp( Color, Sepia, t );  // 원본색과 보간
    
    return Out;
}

// ------------------------------------------------------------
// 테크닉
// ------------------------------------------------------------
technique TShader
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_1_1 PS();
    }
}
