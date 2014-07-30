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
	
	// ������
	float amb = -vLightDir.w;	// ȯ�汤�� ����
	
	float3 N = Normal;
	float3 L = -vLightDir; // ������ǥ�迡���� ��������
	float3 eye = normalize(vEyePos - Pos.xyz);
	float3 H = normalize(L+eye);	// ��������
	
	Out.Color = vColor * max(amb, dot(Normal, -vLightDir))
				+ pow(max(0,dot(N, H)), 10);
	
	
	return Out;
}
// -------------------------------------------------------------
float4 PS(VS_OUTPUT In) : COLOR
{   
    return In.Color;
}

// -------------------------------------------------------------
// ��ũ��
// -------------------------------------------------------------
technique TShader
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_1_1 PS();
    }
}
