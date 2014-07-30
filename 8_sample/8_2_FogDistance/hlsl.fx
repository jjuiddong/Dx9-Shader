// -------------------------------------------------------------
// �Ÿ�����
// 
// Copyright (c) 2002,2003 IMAGIRE Takashi. All rights reserved.
// -------------------------------------------------------------

// -------------------------------------------------------------
// ��������
// -------------------------------------------------------------
float4x4 mWVP;		// ���ÿ��� �������������� ��ǥ��ȯ
float4	 vLightDir;	// ��������
float4   vCol;		// �޽û�
float4   vFog;		// (Far/(Far-Near), -1/(Far-Near))
float4   vEye;		// ����

// -------------------------------------------------------------
// �������̴����� �ȼ����̴��� �ѱ�� ������
// -------------------------------------------------------------
struct VS_OUTPUT
{
	float4 Pos		: POSITION;
	float4 Col		: COLOR0;
	float2 Tex		: TEXCOORD0;
	float  Fog		: FOG;
};

// -------------------------------------------------------------
// �������̴�
// -------------------------------------------------------------
VS_OUTPUT VS (
	float4 Pos		: POSITION,			// ������
	float4 Normal	: NORMAL,			// �𵨹���
	float2 Tex		: TEXCOORD0
){
	VS_OUTPUT Out = (VS_OUTPUT)0;		// ��µ�����
	
	Out.Pos = mul( Pos, mWVP );          // ��ǥ��ȯ
	
	Out.Col = vCol * max( dot(vLightDir, Normal), 0);	// �������
	
	Out.Tex = Tex;						// �ؽ�ó��ǥ
	
	// �Ÿ�����
	float4 pos = Pos - vEye;
	Out.Fog = vFog.x + length(pos) * vFog.y;
	
	return Out;
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
        
		FogEnable = true;			// ���׻��
        FogVertexMode = Linear;		// ���� ����
        FogColor = 0xd8e3fe;		// ���� ��
    }
}