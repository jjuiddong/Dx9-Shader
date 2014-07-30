// ------------------------------------------------------------
// ��������
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// ------------------------------------------------------------
// ��������
// ------------------------------------------------------------
float4x4 mWVP;
float4   vCol;
float4	 vLightDir;	// ��������

// ------------------------------------------------------------
// �ؽ�ó
// ------------------------------------------------------------
texture SrcTex;
sampler SrcSamp = sampler_state
{
    Texture = <SrcTex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// ------------------------------------------------------------
sampler FloorSamp = sampler_state
{
    Texture = <SrcTex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Wrap;
    AddressV = Wrap;
};
// ------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// ------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
	float2 Tex0			: TEXCOORD0;
};
// ------------------------------------------------------------
struct EDGE_INPUT
{
    float4 Pos			: POSITION;
	float2 Tex0			: TEXCOORD0;
	float2 Tex1			: TEXCOORD1;
	float2 Tex2			: TEXCOORD2;
	float2 Tex3			: TEXCOORD3;
};

// ------------------------------------------------------------
// �������̴�(�������)
// ------------------------------------------------------------
VS_OUTPUT VS_pass0 (
      float4 Pos    : POSITION           // ������
     ,float4 Normal : NORMAL             // ��������
     ,float4 Tex0   : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
	// ��ġ��ǥ
	Out.Pos = mul( Pos, mWVP );
	
	// �ؽ�ó��ǥ
    Out.Tex0 = Tex0;

    return Out;
}
// ------------------------------------------------------------
// �ȼ����̴�(�������)
// ------------------------------------------------------------
float4 PS_pass0 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	
	// ��
	Out = tex2D( FloorSamp, In.Tex0 );
	
	return Out;
}
// ------------------------------------------------------------
// �������̴�(��������)
// ------------------------------------------------------------
VS_OUTPUT VS_pass1 (
      float4 Pos    : POSITION           // ������
    , float4 Normal : NORMAL             // ��������
     ,float4 Tex0   : TEXCOORD0	         // �ؽ�ó��ǥ
){
    VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
    
	// ��ġ��ǥ
	Out.Pos = mul( Pos, mWVP );
	
	// ��
	float diffuse = max(dot(vLightDir.xyz, Normal.xyz), 0);//Ȯ�걤
	float ambient = vLightDir.w;						   //ȯ�汤
	Out.Color = vCol * ( diffuse + ambient );
	
	// �ؽ�ó��ǥ
    Out.Tex0 = Tex0;

    return Out;
}

// ------------------------------------------------------------
// �ȼ����̴�(��������)
// ------------------------------------------------------------
float4 PS_pass1 (VS_OUTPUT In) : COLOR
{
	float4 Out;
	
	// ��
	Out = In.Color * tex2D( FloorSamp, In.Tex0 );
	
	return Out;
}
// ------------------------------------------------------------
// �ȼ����̴�(��������)
// ------------------------------------------------------------
float4 PS_Edge (EDGE_INPUT In) : COLOR
{
	float3 d0 = tex2D( SrcSamp, In.Tex0 ).rgb
	           -tex2D( SrcSamp, In.Tex1 ).rgb;
	float3 d1 = tex2D( SrcSamp, In.Tex2 ).rgb
	           -tex2D( SrcSamp, In.Tex3 ).rgb;
	
	// ������Ÿ��2:CbCr
	float3x3 RGB2CrCb = {
		{ 0,         0,         0      },// Y(=0)
		{ 0.50000, - 0.41869, - 0.08131},// Cr
		{-0.16874, - 0.33126, + 0.50000},// Cb
	};
	d0 = mul( RGB2CrCb, d0 );
	d1 = mul( RGB2CrCb, d1 );
	
	float diff = dot(d0, d0)+dot(d1,d1);
	
	return 1.0 - 100.0 * diff;
}

// ------------------------------------------------------------
// ��ũ��
// ------------------------------------------------------------
technique TShader
{
    pass P0 // ���������
    {
        VertexShader = compile vs_1_1 VS_pass0();
        PixelShader  = compile ps_1_1 PS_pass0();
    }
    pass P1 // ����������
    {
        VertexShader = compile vs_1_1 VS_pass1();
        PixelShader  = compile ps_1_1 PS_pass1();
    }
    pass P2 // ����
    {
        // ���̴�
        PixelShader  = compile ps_2_0 PS_Edge();
	}
}
