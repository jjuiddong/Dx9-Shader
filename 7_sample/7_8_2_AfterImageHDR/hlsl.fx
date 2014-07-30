// ------------------------------------------------------------
// �ܻ�
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 mWVP;
float3 vEyePos;
float3 vLightDir;
float4 vCol;

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
texture CurrentMap;
sampler CurrentSamp = sampler_state
{
    Texture = <CurrentMap>;
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
    float4 Color		: COLOR0;
	float3 Normal		: TEXCOORD0;
	float3 Eye			: TEXCOORD1;
};
// -------------------------------------------------------------
struct OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
	float3 Normal		: TEXCOORD0;
	float3 Eye			: TEXCOORD1;
};
// -------------------------------------------------------------
OUTPUT VS(
      float4 Pos    : POSITION,          // ������
      float3 Normal : NORMAL             // �𵨹���
){
	OUTPUT Out = (OUTPUT)0;        // ��µ�����
	float4  uv;
	
	// ��ǥ��ȯ
	Out.Pos = mul(Pos, mWVP);
	
	// ��
	Out.Color = vCol * (0.7*max( dot(vLightDir, Normal), 0)+0.3);
	
	Out.Normal = Normal;		// ����
	Out.Eye    = Pos - vEyePos;	// �ü�
	
	return Out;
}
// -------------------------------------------------------------
float4 PS(OUTPUT In) : COLOR
{   
    float3 e = normalize(In.Eye);	// �ü�����
    float3 n = normalize(In.Normal);// ��������
    float3 r = reflect(e,n);		// �ݻ纤��
    float3 RGB2Lum = {0.299, 0.587, 0.114};
	
	float power = pow(max(0,dot(r,vLightDir)), 32); // ��
	float4 SpecCol = float4(10,13,15,0);			// �ݿ��ݻ��
	
	float4 Out = In.Color + SpecCol * power;
	
	// ��� ������ ����
	Out.a = 0.01*(exp(2.0f*dot(RGB2Lum,Out.rgb))-1.0);

    return Out;
}
// -------------------------------------------------------------

// ------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// ------------------------------------------------------------
struct VS_OUTPUT_AFTERIMAGE
{
    float4 Pos			: POSITION;
	float2 Tex0			: TEXCOORD0;
	float2 Tex1			: TEXCOORD1;
};

// ------------------------------------------------------------
// �ȼ����̴�
// ------------------------------------------------------------
float4 PS_AFTERIMAGE (VS_OUTPUT_AFTERIMAGE In) : COLOR
{   
    float4 Last = tex2D( SrcSamp,     In.Tex0 );
    float4 Now  = tex2D( CurrentSamp, In.Tex1 );
	
	float weight = Last.a-0.001f;
	
    return (1.0f-weight) * Now + weight * Last;
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
        PixelShader  = compile ps_2_0 PS();
    }
    pass P1
    {
        // ���̴�
        PixelShader  = compile ps_2_0 PS_AFTERIMAGE();
    }
}
