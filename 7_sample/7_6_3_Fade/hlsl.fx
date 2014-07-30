// ------------------------------------------------------------
// ���Ǿ�����
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// ------------------------------------------------------------
// ��������
// ------------------------------------------------------------
float t;

// ------------------------------------------------------------
// �ؽ�ó
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
// �������̴����� �ȼ����̴��� �ѱ�� ������
// ------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos            : POSITION;
    float2 Tex            : TEXCOORD0;
};

// ------------------------------------------------------------
// �������̴�
// ------------------------------------------------------------
VS_OUTPUT VS (
      float4 Pos    : POSITION,          // ������
      float4 Tex    : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
    // ��ġ��ǥ
    Out.Pos = Pos;
    // �ؽ�ó��ǥ
    Out.Tex = Tex;
    
    return Out;
}

// ------------------------------------------------------------
// �ȼ����̴�
// ------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
    float4 Out = (float4)0;
    float3 YCbCr;
    
    const float3 RGB2Y  = {0.29900, 0.58700, 0.11400};
    const float3 Cb2RGB = { 0.00000f, -0.34414f, 1.77200f};
    const float3 Cr2RGB = {+1.40200f, -0.71414f, 0.00000f};

    float3 Color = tex2D( SrcSamp, In.Tex ).xyz; // ������
    
    YCbCr.x = dot(Color, RGB2Y);    // Y
    YCbCr.y = -0.2f;				// Cb
    YCbCr.z =  0.1f;				// Cr
    
    float3 Sepia = YCbCr.x
				 + mul(Cb2RGB, YCbCr.y)
				 + mul(Cr2RGB, YCbCr.z);// YCbCr�� RGB��
    
    Out.rgb = lerp( Color, Sepia, t );  // �������� ����
    
    return Out;
}

// ------------------------------------------------------------
// ��ũ��
// ------------------------------------------------------------
technique TShader
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_1_1 PS();
    }
}
