// -------------------------------------------------------------
// ������ũ
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// �ؽ�ó
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
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos            : POSITION;
    float2 Tex0           : TEXCOORD0;// ����ũ �ؽ�ó
    float2 Tex1           : TEXCOORD1;// �������̹���
};

// -------------------------------------------------------------
// �ȼ����̴�
// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
    float4 Color;
    const float grids = 30.0f;	// ������ũ�� ���� ȭ����Ҽ�
    
    float2 tex_coord = floor( grids * In.Tex1 + 0.5f)/grids;
    Color.rgb = tex2D( SrcSamp, tex_coord );

    Color.a = tex2D( MaskSamp, In.Tex0 );// ����
    
    return Color;
}

// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // ���̴�
        PixelShader  = compile ps_2_0 PS();
    }
}
