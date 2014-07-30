// -------------------------------------------------------------
// 모자이크
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// 텍스처
// -------------------------------------------------------------
texture tMask;
sampler MaskSamp = sampler_state
{
    Texture = <tMask>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
texture tSrc;
sampler SrcSamp = sampler_state
{
    Texture = <tSrc>;
    MinFilter = POINT;
    MagFilter = POINT;
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
    float2 Tex0           : TEXCOORD0;// 마스크 텍스처
    float2 Tex1           : TEXCOORD1;// 렌더링이미지
};

// -------------------------------------------------------------
// 픽셀셰이더
// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
    float4 Color;
    const float grids = 30.0f;	// 모자이크를 위한 화면분할수
    
    float2 tex_coord = floor( grids * In.Tex1 + 0.5f)/grids;
    Color.rgb = tex2D( SrcSamp, tex_coord );

    Color.a = tex2D( MaskSamp, In.Tex0 );// 투명도
    
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
        PixelShader  = compile ps_2_0 PS();
    }
}
