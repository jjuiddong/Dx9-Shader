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

// -------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
};
// -------------------------------------------------------------
struct OUTPUT
{
    float4 Pos			: POSITION;
    float4 Color		: COLOR0;
};
// -------------------------------------------------------------
OUTPUT VS(
      float4 Pos    : POSITION,          // ������
      float3 Normal : NORMAL             // �𵨹���
){
	OUTPUT Out = (OUTPUT)0;        // ��µ�����
	
	// ��ǥ��ȯ
	Out.Pos = mul(mul(Pos, mWV), mVP);
	
	// ��
	Out.Color = vCol * (0.7*max( dot(vLightDir, Normal), 0)// �g�U�F
						+0.3);// ���F
	
	return Out;
}
// -------------------------------------------------------------
OUTPUT VS_Blur(
      float4 Pos    : POSITION,          // ������
      float3 Normal : NORMAL             // �𵨹���
){
	OUTPUT Out = (OUTPUT)0;        // ��µ�����
	
	// ��ǥ��ȯ
	float4 x1 = mul(Pos, mWV);			// ���� ����ǥ
	float4 x0 = mul(Pos, mLastWV);		// 1������ ���� ����ǥ
	float4 v = x1-x0;					// �ӵ�
	float3 n = mul(Normal, mWV);		// ����ǥ�迡���� ����
	
	bool bFront = (0<=dot(n, v.xyz));	// �ӵ��������� ���ϰ� �ִ°�?
	float4 x = bFront ? x1 : x0;		// ���⿡���� ��ġ����
	
	Out.Pos = mul(x, mVP);				// ������������
	
	// ��
	Out.Color = vCol * (0.7*max( dot(vLightDir, Normal), 0)// Ȯ�걤
						+0.3);// ȯ�汤
	
	// ����������� ���ϰ� ������ ������, �ݴ��̸� ����
	Out.Color.a = bFront ? 1 : 0;
	
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
    }
    pass P1
    {
        // ��Ǻ�
        VertexShader = compile vs_1_1 VS_Blur();
    }
}
