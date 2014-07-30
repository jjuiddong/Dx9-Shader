// ------------------------------------------------------------
// ��Ǻ�
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// ------------------------------------------------------------

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 mWV;
float4x4 mLastWV;
float4x4 mVP;
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
// -------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
	float3 Normal		: TEXCOORD0;
	float3 Eye			: TEXCOORD1;
};
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
	float4 Tex			: TEXCOORD0;
	float4 Velocity		: TEXCOORD1;
};
// -------------------------------------------------------------
OUTPUT VS(
      float4 Pos    : POSITION,          // ������
      float3 Normal : NORMAL             // �𵨹���
){
	OUTPUT Out = (OUTPUT)0;        // ��µ�����
	
	// ��ǥ��ȯ
	Out.Pos = mul(mul(Pos, mWV), mVP);		// �߰���ǥ���
	
	// ��
	Out.Color = vCol * (0.7*max( dot(vLightDir, Normal), 0)// Ȯ���
						+0.3);// ȯ���
	
	Out.Normal = Normal;		// ��
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
	
	// ������Ʈ�� �����ϴ� ���� ���ĸ� 1��
	Out.a = 1;
	
    return Out;
}
// -------------------------------------------------------------
VS_OUTPUT VS_Blur(
      float4 Pos    : POSITION,          // ������
      float3 Normal : NORMAL             // �𵨹���
){
	VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
	
	// ��ǥ��ȯ
	float4 x1 = mul(Pos, mWV);			// ���� ����ǥ
	float4 x0 = mul(Pos, mLastWV);		// 1������ ���� ����ǥ
	float4 v = x1-x0;					// �ӵ�
	float3 n = mul(Normal, mWV);		// ����ǥ�迡���� ����
	
	bool bFront = (0<=dot(n, v.xyz));	// �ӵ��������� ���ϰ� �ִ°�?
	float4 x = bFront ? x1 : x0;		// ���⿡���� ��ġ����
	
	Out.Pos = mul(x, mVP);				// ������������
	
	// ������������ �ؽ�ó�������� ��ȯ
	Out.Tex.x =  Out.Pos.x + Out.Pos.w;
	Out.Tex.y = -Out.Pos.y + Out.Pos.w;
	Out.Tex.w = 2.0f*Out.Pos.w;
	
	// �ؽ�ó��ǥ�� �ӵ��� ���Ѵ�
	float4 s0 = mul(x0, mVP); s0 /= s0.w;
	float4 s1 = mul(x1, mVP); s1 /= s1.w;
	Out.Velocity = s1 - s0;
	Out.Velocity.x *= +0.5f;
	Out.Velocity.y *= -0.5f;
	
	return Out;
}
// ------------------------------------------------------------
// �ȼ����̴�
// ------------------------------------------------------------
float4 PS_Blur (VS_OUTPUT In) : COLOR
{   
    float4 Out = 0;
	const int   SAMPLES = 26;
	const float samples = SAMPLES;
	
	for(int i=0;i<SAMPLES;i++){
		float t = (float)(i+1)/samples;
		Out += tex2D( SrcSamp, In.Tex/In.Tex.w + t*In.Velocity );
	}
	Out /= samples;
	
    return Out;
}

// ------------------------------------------------------------
// ��ũ��
// ------------------------------------------------------------
technique TShader
{
    pass P0
    {
        // �Ϲݷ���
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS();
    }
    pass P1
    {
        // ��Ǻ�
        VertexShader = compile vs_1_1 VS_Blur();
        PixelShader  = compile ps_2_0 PS_Blur();
    }
}
