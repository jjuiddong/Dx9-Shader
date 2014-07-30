// -------------------------------------------------------------
// ������
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------

float4x4 mWVP;		// ��ǥ��ȯ ���

float4 vLightDir;	// ��������
float4 vColor;		// ����*�޽� ��
float3 vEyePos;		// ī�޶���ġ(������ǥ��)

// -------------------------------------------------------------
// �ؽ�ó
// -------------------------------------------------------------
// ���� �ؽ�ó
texture DecaleTex;
sampler DecaleSamp = sampler_state
{
    Texture = <DecaleTex>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Clamp;
    AddressV = Clamp;
};
// -------------------------------------------------------------
// ������
texture NormalMap;
sampler NormalSamp = sampler_state
{
    Texture = <NormalMap>;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
    MipFilter = NONE;

    AddressU = Wrap;
    AddressV = Wrap;
};

// -------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;		// ������
    float2 Tex			: TEXCOORD0;	// �����ؽ�ó ��ǥ
    float3 L			: TEXCOORD1;	// ��������
    float3 E			: TEXCOORD2;	// ��������
};
// -------------------------------------------------------------
// ��鷻��
// -------------------------------------------------------------
VS_OUTPUT VS(
      float4 Pos      : POSITION,          // ������ġ��ǥ
      float3 Normal   : NORMAL,            // ��������
      float3 Tangent  : TANGENT0,          // ��������
      float2 Texcoord : TEXCOORD0          // �ؽ�ó��ǥ
){
	VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
	
	// ��ǥ��ȯ
	Out.Pos = mul(Pos, mWVP);
	
	// �޽� ��
	Out.Color = vColor;
	
	// ���ÿ� �ؽ�ó��ǥ
	Out.Tex = Texcoord;

	// ��ǥ�躯ȯ ����
	float3 N = Normal;
	float3 T = Tangent;
	float3 B = cross(N,T);

	// �ݿ��ݻ�� ����
	float3 E = vEyePos - Pos.xyz;	// �ü�����
	Out.E.x = dot(E,T);
	Out.E.y = dot(E,B);
	Out.E.z = dot(E,N);

	float3 L = -vLightDir.xyz;		// ��������
	Out.L.x = dot(L,T);
	Out.L.y = dot(L,B);
	Out.L.z = dot(L,N);
	
	return Out;
}
// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
	float3 N = 2.0f*tex2D( NormalSamp, In.Tex ).xyz-1.0;// ���������κ��� ����
	float3 L = normalize(In.L);						// ��������
	float3 R = reflect(-normalize(In.E), N);		// �ݻ纤��
	float amb = -vLightDir.w;						// ȯ�汤�� ����
	
    return In.Color * tex2D( DecaleSamp, In.Tex )	// Ȯ�걤�� ȯ�汤��
			   * (max(0, dot(N, L))+amb)			// �������� �ؽ�ó���� �ռ��Ѵ�
			 + 0.3f * pow(max(0,dot(R, L)), 8);		// Phong�ݿ��ݻ籤
}

// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS();
    }
}
