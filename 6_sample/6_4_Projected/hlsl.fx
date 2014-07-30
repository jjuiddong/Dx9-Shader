// -------------------------------------------------------------
// �����ؽ�ó �׸���
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 mWVP;		// ���ÿ��� �������������� ��ǥ��ȯ
float4x4 mWVPT;		// ���ÿ��� �ؽ�ó���������� ��ǥ��ȯ
float4	 vLightPos;	// ������ ��ġ

// -------------------------------------------------------------
// �ؽ�ó
// -------------------------------------------------------------
texture DecaleMap;
sampler DecaleMapSamp = sampler_state
{
    Texture = <DecaleMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};

texture ShadowMap;
sampler ShadowMapSamp = sampler_state
{
    Texture = <ShadowMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};


// -------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos		: POSITION;
	float4 Color	: COLOR0;
	float2 TexDecale: TEXCOORD0;
	float4 TexShadow: TEXCOORD1;
};
// -------------------------------------------------------------
// �������̴�
// -------------------------------------------------------------
VS_OUTPUT VS (
	  float4 Pos	: POSITION          // ������ġ
	, float4 Normal	: NORMAL            // ��������
	, float2 Tex	: TEXCOORD0			// �ؽ�ó��ǥ
){
	VS_OUTPUT Out;        // ��µ�����
	
	// ��ġ��ȯ
	Out.Pos = mul( Pos, mWVP );
	
	Out.Color = max( dot(normalize(vLightPos.xyz-Pos.xyz), Normal), 0);
	
	// �ؽ�ó��ǥ
	Out.TexDecale = Tex;
	
	// �ؽ�ó��ǥ
	Out.TexShadow = mul( Pos, mWVPT );
	
	return Out;
}
// -------------------------------------------------------------
// �ȼ����̴�
// -------------------------------------------------------------
float4 PS ( VS_OUTPUT In) : COLOR
{
	float4 decale = tex2D( DecaleMapSamp, In.TexDecale );
	float4 shadow = tex2Dproj( ShadowMapSamp, In.TexShadow );
	
	return decale * (saturate(In.Color-0.5f*shadow)+0.3f);
}
// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // ���̴�
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}
