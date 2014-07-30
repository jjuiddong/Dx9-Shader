// -------------------------------------------------------------
// �׸��� ��
// 
// Copyright (c) 2002-2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 mWVP;		// ���ÿ��� �������������� ��ǥ��ȯ
float4x4 mWLP;		// ���ÿ��� �������������� ��ǥ��ȯ
float4x4 mWLPB;		// �ؽ�ó��ǥ����� ����
float4   vCol;		// �޽û�
float4	 vLightDir;	// ��������

// -------------------------------------------------------------
// �ؽ�ó
// -------------------------------------------------------------
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
// -------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
	float4 Diffuse		: COLOR0;
	float4 Ambient		: COLOR1;
	float4 ShadowMapUV	: TEXCOORD0;
	float4 Depth		: TEXCOORD1;
	float2 DecaleTex	: TEXCOORD2;
};

// -------------------------------------------------------------
// 1�н�:�������̴�
// -------------------------------------------------------------
VS_OUTPUT VS_pass0(
      float4 Pos    : POSITION,          // ������
      float3 Normal : NORMAL	         // �𵨹���
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
    // ��ǥ��ȯ
	float4 pos = mul( Pos, mWLP );
	
    // ��ġ��ǥ
    Out.Pos = pos;
    
    // ī�޶���ǥ�迡���� ���̸� �ؽ�ó�� �ִ´�
    Out.ShadowMapUV = pos;

    return Out;
}
// -------------------------------------------------------------
// 1�н�:�ȼ����̴�
// -------------------------------------------------------------
float4 PS_pass0(VS_OUTPUT In) : COLOR
{   
    float4 Out;
    
    Out = In.ShadowMapUV.z / In.ShadowMapUV.w;
    
    return Out;
}
// -------------------------------------------------------------
// �������̴�
// -------------------------------------------------------------
VS_OUTPUT VS(
      float4 Pos    : POSITION,          // ������
      float3 Normal : NORMAL,	         // �𵨹���
      float2 Tex    : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
	float4	uv;
	
	// ��ǥ��ȯ
    Out.Pos = mul(Pos, mWVP);
	// ��
	Out.Diffuse = vCol * max( dot(vLightDir, Normal), 0);// Ȯ�걤
	Out.Ambient = vCol * 0.3f;							 // ȯ�汤
	
	// �׸��� ��
	Out.ShadowMapUV = mul(Pos, mWLPB);
	
	// �񱳸� ���� ���̰�
	Out.Depth       = mul(Pos, mWLP);
		
	// ���� �ؽ�ó
	Out.DecaleTex   = Tex;
		
    return Out;
}
// -------------------------------------------------------------
// 2�н�:�ȼ����̴�(�ؽ�ó����)
// -------------------------------------------------------------
float4 PS_pass1(VS_OUTPUT In) : COLOR
{   
    float4 Color;
	float  shadow = tex2Dproj( ShadowMapSamp, In.ShadowMapUV ).x;
	float4 decale = tex2D( DecaleMapSamp, In.DecaleTex );
    
    Color = In.Ambient
		 + ((shadow * In.Depth.w < In.Depth.z-0.03f) ? 0 : In.Diffuse);

    return Color * decale;
}  
// -------------------------------------------------------------
// 2�н�:�ȼ����̴�(�ؽ�ó����)
// -------------------------------------------------------------
float4 PS_pass2(VS_OUTPUT In) : COLOR
{   
    float4 Color;
	float  shadow = tex2Dproj( ShadowMapSamp, In.ShadowMapUV ).x;
    
    Color = In.Ambient
		 + ((shadow * In.Depth.w < In.Depth.z-0.03f) ? 0 : In.Diffuse);

    return Color;
}  
// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TShader
{
    pass P0// �׸��ڸ� ����
    {
        // ���̴�
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_2_0 PS_pass0();
    }
    pass P1// �ؽ�ó ����
    {
        // ���̴�
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS_pass1();
    }
    pass P2// �ؽ�ó ����
    {
        // ���̴�
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS_pass2();
    }
}
