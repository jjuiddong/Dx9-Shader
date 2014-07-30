// ------------------------------------------------------------
// �ܻ�
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
    float4 Pos			: POSITION;
	float2 Tex			: TEXCOORD0;
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
    
    Out.Tex = Tex;
    
    return Out;
}

// ------------------------------------------------------------
// �ȼ����̴�
// ------------------------------------------------------------
float4 PS (VS_OUTPUT In) : COLOR
{   
    float4 Out = tex2D( SrcSamp, In.Tex );
	
	Out.a = 0.9f;
	
	
    return Out;
}
// ------------------------------------------------------------
// ��ũ��
// ------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // ���̴�
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_1_1 PS();
        
		Sampler[0] = (SrcSamp);
		
        // ����������Ʈ
        AlphaBlendEnable = True;
        SrcBlend = SrcAlpha;
        DestBlend = InvSrcAlpha;
    }
}
