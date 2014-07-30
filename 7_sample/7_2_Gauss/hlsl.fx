// -------------------------------------------------------------
// 가우스필터
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// 전역변수
// -------------------------------------------------------------

float  MAP_WIDTH;
float  MAP_HEIGHT;
float  weight[8];
float2 offsetX;
float2 offsetY;

// -------------------------------------------------------------
// 텍스처
// -------------------------------------------------------------
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
struct VS_OUTPUT
{
    float4 Pos            : POSITION;
    float2 Tex0            : TEXCOORD0;
    float2 Tex1            : TEXCOORD1;
    float2 Tex2            : TEXCOORD2;
    float2 Tex3            : TEXCOORD3;
    float2 Tex4            : TEXCOORD4;
    float2 Tex5            : TEXCOORD5;
    float2 Tex6            : TEXCOORD6;
    float2 Tex7            : TEXCOORD7;
};

// -------------------------------------------------------------
// X뭉개기
// -------------------------------------------------------------

// -------------------------------------------------------------
// 정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_pass1 (
      float4 Pos    : POSITION,          // 모델정점
      float4 Tex    : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 위치좌표
    Out.Pos = Pos;
    
    Out.Tex0 = Tex + float2( - 1.0f/MAP_WIDTH, 0.0f );
    Out.Tex1 = Tex + float2( - 3.0f/MAP_WIDTH, 0.0f );
    Out.Tex2 = Tex + float2( - 5.0f/MAP_WIDTH, 0.0f );
    Out.Tex3 = Tex + float2( - 7.0f/MAP_WIDTH, 0.0f );
    Out.Tex4 = Tex + float2( - 9.0f/MAP_WIDTH, 0.0f );
    Out.Tex5 = Tex + float2( -11.0f/MAP_WIDTH, 0.0f );
    Out.Tex6 = Tex + float2( -13.0f/MAP_WIDTH, 0.0f );
    Out.Tex7 = Tex + float2( -15.0f/MAP_WIDTH, 0.0f );
    
    return Out;
}

// -------------------------------------------------------------
// 픽셀셰이더
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT In) : COLOR
{   
    float4 Color;
    
    Color  = weight[0] * (tex2D( SrcSamp, In.Tex0 )
                        + tex2D( SrcSamp, In.Tex7 + offsetX ));
    Color += weight[1] * (tex2D( SrcSamp, In.Tex1 )
                        + tex2D( SrcSamp, In.Tex6 + offsetX ));
    Color += weight[2] * (tex2D( SrcSamp, In.Tex2 )
                        + tex2D( SrcSamp, In.Tex5 + offsetX ));
    Color += weight[3] * (tex2D( SrcSamp, In.Tex3 )
                        + tex2D( SrcSamp, In.Tex4 + offsetX ));
    Color += weight[4] * (tex2D( SrcSamp, In.Tex4 )
                        + tex2D( SrcSamp, In.Tex3 + offsetX ));
    Color += weight[5] * (tex2D( SrcSamp, In.Tex5 )
                        + tex2D( SrcSamp, In.Tex2 + offsetX ));
    Color += weight[6] * (tex2D( SrcSamp, In.Tex6 )
                        + tex2D( SrcSamp, In.Tex1 + offsetX ));
    Color += weight[7] * (tex2D( SrcSamp, In.Tex7 )
                        + tex2D( SrcSamp, In.Tex0 + offsetX ));
    
    return Color;
}
// -------------------------------------------------------------
// Y뭉개기
// -------------------------------------------------------------

// -------------------------------------------------------------
// 정점셰이더
// -------------------------------------------------------------
VS_OUTPUT VS_pass2 (
      float4 Pos    : POSITION,          // 모델정점
      float4 Tex    : TEXCOORD0	         // 텍스처좌표
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // 출력데이터
    
    // 위치좌표
    Out.Pos = Pos;
    
    Out.Tex0 = Tex + float2( 0.0f, - 1.0f/MAP_HEIGHT );
    Out.Tex1 = Tex + float2( 0.0f, - 3.0f/MAP_HEIGHT );
    Out.Tex2 = Tex + float2( 0.0f, - 5.0f/MAP_HEIGHT );
    Out.Tex3 = Tex + float2( 0.0f, - 7.0f/MAP_HEIGHT );
    Out.Tex4 = Tex + float2( 0.0f, - 9.0f/MAP_HEIGHT );
    Out.Tex5 = Tex + float2( 0.0f, -11.0f/MAP_HEIGHT );
    Out.Tex6 = Tex + float2( 0.0f, -13.0f/MAP_HEIGHT );
    Out.Tex7 = Tex + float2( 0.0f, -15.0f/MAP_HEIGHT );
    
    return Out;
}

// -------------------------------------------------------------
// 픽셀셰이더
// -------------------------------------------------------------
float4 PS_pass2(VS_OUTPUT In) : COLOR
{   
    float4 Color;
    
    Color  = weight[0] * (tex2D( SrcSamp, In.Tex0 )
                        + tex2D( SrcSamp, In.Tex7 + offsetY ));
    Color += weight[1] * (tex2D( SrcSamp, In.Tex1 )
                        + tex2D( SrcSamp, In.Tex6 + offsetY ));
    Color += weight[2] * (tex2D( SrcSamp, In.Tex2 )
                        + tex2D( SrcSamp, In.Tex5 + offsetY ));
    Color += weight[3] * (tex2D( SrcSamp, In.Tex3 )
                        + tex2D( SrcSamp, In.Tex4 + offsetY ));
    Color += weight[4] * (tex2D( SrcSamp, In.Tex4 )
                        + tex2D( SrcSamp, In.Tex3 + offsetY ));
    Color += weight[5] * (tex2D( SrcSamp, In.Tex5 )
                        + tex2D( SrcSamp, In.Tex2 + offsetY ));
    Color += weight[6] * (tex2D( SrcSamp, In.Tex6 )
                        + tex2D( SrcSamp, In.Tex1 + offsetY ));
    Color += weight[7] * (tex2D( SrcSamp, In.Tex7 )
                        + tex2D( SrcSamp, In.Tex0 + offsetY ));
    
    return Color;
}

// -------------------------------------------------------------
// 테크닉
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS_pass1();
        PixelShader  = compile ps_2_0 PS_pass1();
    }
    pass P1
    {
        // 셰이더
        VertexShader = compile vs_1_1 VS_pass2();
        PixelShader  = compile ps_2_0 PS_pass2();
    }
}
