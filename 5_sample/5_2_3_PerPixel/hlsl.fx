// -------------------------------------------------------------
// �ݿ��ݻ籤
// 
// Copyright (c) 2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------

float4x4 mWVP;

float4 vLightDir;	// ��������
float4 vColor;		// ����*�޽� ��
float3 vEyePos;		// ī�޶���ġ(������ǥ��)

// -------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
    float3 N			: TEXCOORD0;
    float3 Eye			: TEXCOORD1;
};
// -------------------------------------------------------------
// ��鷻��
// -------------------------------------------------------------
VS_OUTPUT VS(
      float4 Pos    : POSITION,          // ������ġ��ǥ
      float4 Normal : NORMAL            // ��������
){
	VS_OUTPUT Out = (VS_OUTPUT)0;        // ��µ�����
	
	// ��ǥ��ȯ
	Out.Pos = mul(Pos, mWVP);
	
	// Ȯ�걤+ȯ�汤
	float amb = -vLightDir.w;	// ȯ�汤�� ����
	float3 L = -vLightDir; // ������ǥ�迡���� ��������
	Out.Color = vColor * max(amb, dot(Normal, -vLightDir));
	
	// �ݿ��ݻ�� ����
	Out.N   = Normal.xyz;
	Out.Eye = vEyePos - Pos.xyz;
	
	return Out;
}
// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
	float3 L = -vLightDir.xyz;
	float3 H = normalize(L + normalize(In.Eye));	// ��������
	float3 N = normalize(In.N);
	
    return In.Color							// Ȯ�걤+ȯ�汤
			 + pow(max(0,dot(N, H)), 10);	// �ݿ��ݻ籤
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
