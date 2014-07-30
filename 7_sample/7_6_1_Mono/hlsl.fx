// ------------------------------------------------------------
// ���Ǿ�����
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

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
    
    const float3 RGB2Y = {0.29900, 0.58700, 0.11400};

    float3 Color = tex2D( SrcSamp, In.Tex ).xyz; // ������
    
    Out.rgb = dot(Color, RGB2Y);    // Y�� �״�� ������
    
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
